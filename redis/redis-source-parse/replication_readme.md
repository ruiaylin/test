# redis server replication 源码分析
---

##1 redis主流程

<font color=blue>

> redis启动流程:
>> 1 加载配置；
>
>> 2 初始化redis master、slave以及sentinel的sri；
>
>> 3 注册事件事件serverCron，定时地调用sentinel的逻辑loop函数sentinelTimer。

</font>

<font color=green>

    int main(int argc, char **argv) {
        // 初始化conf
        initServerConfig();

        // 加载配置
        if (argc >= 2) {
            loadServerConfig(configfile,options);
            sdsfree(options);
        }
        // 启动server
        initServer();

        // 启动linux的内存优化项[overcommit_memory & transparent_huge_page]，从磁盘加载数据 
        if (!server.sentinel_mode) {
			/* Things not needed when running in Sentinel mode. */
			redisLog(REDIS_WARNING,"Server started, Redis version " REDIS_VERSION);
	#ifdef __linux__
			linuxMemoryWarnings();
	#endif
			checkTcpBacklogSettings();
			loadDataFromDisk();
        }

        // 开足马力，运转发动机
        aeSetBeforeSleepProc(server.el,beforeSleep);
        aeMain(server.el);
        aeDeleteEventLoop(server.el);

        return 0;
    }

</font>


###1.1 检查内存

<font color=blue>

overcommit_memory文件指定了内核针对内存分配的策略

>overcommit_memory值可以是0、1、2。                               
>
>>0， 表示内核将检查是否有足够的可用内存供应用进程使用；如果有足够的可用内存，内存申请允许；否则，内存申请失败，并把错误返回给应用进程。 
>
>>1， 表示内核允许分配所有的物理内存，而不管当前的内存状态如何。
>
>>2， 表示内核允许分配超过所有物理内存和交换空间总和的内存

</font>

<font color=green>

	int linuxOvercommitMemoryValue(void) {
	    FILE *fp = fopen("/proc/sys/vm/overcommit_memory","r");
	    char buf[64];
	
	    if (!fp) return -1;
	    if (fgets(buf,64,fp) == NULL) {
	        fclose(fp);
	        return -1;
	    }
	    fclose(fp);
	
	    return atoi(buf);
	}

</font>

<font color=blue>

Transparent Huge Page用户合并物理内存的page

>内核线程khugepaged周期性自动扫描内存，自动将地址连续可以合并的4KB的普通Page并成2MB的Huge Page。
>
>Redhat系统
>
>>通过内核参数/sys/kernel/mm/redhat_transparent_hugepage/enabled打开.
>
>其他Linux系统
>
>>通过内核参数/sys/kernel/mm/transparent_hugepage/enabled打开.

</font>

<font color=green>
	
	/* Returns 1 if Transparent Huge Pages support is enabled in the kernel.
	 * Otherwise (or if we are unable to check) 0 is returned. */
	int THPIsEnabled(void) {
	    char buf[1024];
	
	    FILE *fp = fopen("/sys/kernel/mm/transparent_hugepage/enabled","r");
	    if (!fp) return 0;
	    if (fgets(buf,sizeof(buf),fp) == NULL) {
	        fclose(fp);
	        return 0;
	    }
	    fclose(fp);
	    return (strstr(buf,"[never]") == NULL) ? 1 : 0;
	}

</font>

<font color=blue>

综合检查linux memory流程

</font>

<font color=green>
	
	void linuxMemoryWarnings(void) {
	    if (linuxOvercommitMemoryValue() == 0) {
	        redisLog(REDIS_WARNING,"WARNING overcommit_memory is set to 0! Background save may fail under low memory condition. To fix this issue add 'vm.overcommit_memory = 1' to /etc/sysctl.conf and then reboot or run the command 'sysctl vm.overcommit_memory=1' for this to take effect.");
	    }
	    if (THPIsEnabled()) {
	        redisLog(REDIS_WARNING,"WARNING you have Transparent Huge Pages (THP) support enabled in your kernel. This will create latency and memory usage issues with Redis. To fix this issue run the command 'echo never > /sys/kernel/mm/transparent_hugepage/enabled' as root, and add it to your /etc/rc.local in order to retain the setting after a reboot. Redis must be restarted after THP is disabled.");
	    }
	}

</font>

##2 slave 流程

###2.1 slave模式

<font color=green>
收到slaveof命令，启动slave模式
</font>

<font color=green>

	void slaveofCommand(redisClient *c) {
		long port;
	
		if ((getLongFromObjectOrReply(c, c->argv[2], &port, NULL) != REDIS_OK))
		  return;
	
		/* Check if we are already attached to the specified slave */
		// 如果已经attach到指定的master[host:port]，则拒绝再次重连，返回ok
		if (server.masterhost && !strcasecmp(server.masterhost,c->argv[1]->ptr)
					&& server.masterport == port) {
			redisLog(REDIS_NOTICE,"SLAVE OF would result into synchronization with the master we are already connected with. No operation performed.");
			addReplySds(c,sdsnew("+OK Already connected to specified master\r\n"));
			return;
		}
		/* There was no previous master or the user specified a different one,
		 * we can continue. */
		// 连接指定的redis instance
		replicationSetMaster(c->argv[1]->ptr, port);
		redisLog(REDIS_NOTICE,"SLAVE OF %s:%d enabled (user request)",
					addReply(c,shared.ok);
	}

</font>
	
##3 master流程


##4 replication综合流程

###4.1 replication流程文字描述

>无论是初次连接还是重新连接， 当建立一个从服务器时， 从服务器都将向主服务器发送一个 SYNC 命令。
>
>接到 SYNC 命令的主服务器将开始执行 BGSAVE ， 并在保存操作执行期间， 将所有新执行的写入命令都保存到一个缓冲区里面。
>
>当 BGSAVE 执行完毕后， 主服务器将执行保存操作所得的 .rdb 文件发送给从服务器， 从服务器接收这个 .rdb 文件， 并将文件中的数据载入到内存中。
>
>之后主服务器会以 Redis 命令协议的格式， 将写命令缓冲区中积累的所有内容都发送给从服务器。
>
>你可以通过 telnet 命令来亲自验证这个同步过程： 首先连上一个正在处理命令请求的 Redis 服务器， 然后向它发送 SYNC 命令， 过一阵子， 你将看到 telnet 会话（session）接收到服务器发来的大段数据（.rdb 文件）， 之后还会看到， 所有在服务器执行过的写命令， 都会重新发送到 telnet 会话来。
>
>即使有多个从服务器同时向主服务器发送 SYNC ， 主服务器也只需执行一次 BGSAVE 命令， 就可以处理所有这些从服务器的同步请求。
>
>从服务器可以在主从服务器之间的连接断开时进行自动重连， 在 Redis 2.8 版本之前， 断线之后重连的从服务器总要执行一次完整重同步（full resynchronization）操作， 但是从 Redis 2.8 版本开始， 从服务器可以根据主服务器的情况来选择执行完整重同步还是部分重同步（partial resynchronization）。
>
>另外，有关配置项的注意事项：如果主服务器通过 requirepass 选项设置了密码， 那么为了让从服务器的同步操作可以顺利进行， 我们也必须为从服务器进行相应的身份验证设置。

###4.2 replication过程中slave与master的函数流程图

![](http://www.hoterran.info/wp-content/uploads/2011/06/redis_replication.png)

###4.3 replication过程中slave与master的状态变化图

![](http://www.hoterran.info/wp-content/uploads/2011/07/redis_replication_interactive.png)


## 参考文档：
   1 redis/src/replication.c
   2 http://redis.readthedocs.org/en/latest/topic/replication.html
   3 http://www.360doc.com/content/11/1205/14/7936054_169834858.shtml

