# redis server replication 源码分析#
---

##1 redis主流程 ##

<font color=blue>

**redis启动流程:**

- 1 加载配置；
- 2 初始化redis master、slave以及sentinel的sri；
- 3 注册事件事件serverCron，定时地调用sentinel的逻辑loop函数sentinelTimer。

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

###1.1 初始化server相关参数###

<font color>
初始化server.runid，并给replication相关参数赋值。
</font>

<font color=yellow>

>Linux中的随机数可以从两个特殊的文件中产生，一个是/dev/urandom.另外一个是/dev/random。他们产生随机数的原理是利用当前系统的熵池来计算出固定一定数量的随机比特，然后将这些比特作为字节流返回。
>
 >熵池就是当前系统的环境噪音，熵指的是一个系统的混乱程度，系统噪音可以通过很多参数来评估，如内存的使用，文件的使用量，不同类型的进程数量等等。如果当前环境噪音变化的不是很剧烈或者当前环境噪音很小，比如刚开机的时候，而当前需要大量的随机比特，这时产生的随机数的随机效果就不是很好了。这就是为什么会有/dev/urandom和/dev/random这两种不同的文件，后者在不能产生新的随机数时会阻塞程序，而前者不会（ublock），当然产生的随机数效果就不太好了，这对加密解密这样的应用来说就不是一种很好的选择。
>
>/dev/random会阻塞当前的程序，直到根据熵池产生新的随机字节之后才返回，所以使用/dev/random比使用/dev/urandom产生大量随机数的速度要慢。
>

</font>

<font color=blue>

    /*
     * 这个函数为一个redis instance生成一个160bit的id
     * (如果未来显示用则是320bit，即40个字母长度的可显示字符串)。
     */
    void getRandomHexChars(char *p, unsigned int len)；
    {
        char *charset = "0123456789abcdef";
        unsigned int j;

        /* Global state. */
        static int seed_initialized = 0;
        static unsigned char seed[20]; /* The SHA1 seed, from /dev/urandom. */
        static uint64_t counter = 0; /* The counter we hash with the seed. */
        //  生成一个随机种子数，存储到seed里面
        if (!seed_initialized) {
            FILE *fp = fopen("/dev/urandom", "r");
            if (fp && fread(seed, sizeof(seed), 1, fp) == 1)
                seed_initialized = 1;
            if (fp) fclose(fp);
        }

        if (seed_initialized) {
            // 如果从/dev/urandom读取到了随机字符串，则利用SHA算法生成一个id
            while (len) {
                // len可能并不是20或者40字节的倍数，所以这里要通过循环把p的内容填满
                unsigned char digest[20];
                SHA1_CTX ctx;
                unsigned int copylen = len > 20 ? 20 : len;

                SHA1Init(&ctx);
                SHA1Update(&ctx, seed, sizeof(seed));
                SHA1Update(&ctx, (unsigned char*)&counter, sizeof(counter));
                SHA1Final(digest, &ctx);
                counter++;

                memcpy(p, digest, copylen);
                /* Convert to hex digits. */
                // 把数字转化为可读字符串，只是这里只用了一个字节的后半部分
                for (j = 0; j < copylen; j++) p[j] = charset[p[j] & 0x0F];

                // 移动光标
                len -= copylen;
                p += copylen;
            }
        }
        else {
            // 如果从/dev/urandom读取随机字符串失败，则利用时间和当前进程的id来生成一个随机字符串
            char *x = p;
            unsigned int l = len;
            struct timeval tv;
            pid_t pid = getpid();

            /* Use time and PID to fill the initial array. */
            // 先在buf中填充时间的秒和微秒两个部分，然后再补充上进程的id
            gettimeofday(&tv, NULL);
            if (l >= sizeof(tv.tv_usec)) {
                memcpy(x, &tv.tv_usec, sizeof(tv.tv_usec));
                l -= sizeof(tv.tv_usec);
                x += sizeof(tv.tv_usec);
            }
            if (l >= sizeof(tv.tv_sec)) {
                memcpy(x, &tv.tv_sec, sizeof(tv.tv_sec));
                l -= sizeof(tv.tv_sec);
                x += sizeof(tv.tv_sec);
            }
            if (l >= sizeof(pid)) {
                memcpy(x, &pid, sizeof(pid));
                l -= sizeof(pid);
                x += sizeof(pid);
            }
            // 再利用随机数进行异或后，转化为16进制可视字符串
            for (j = 0; j < len; j++) {
                p[j] ^= rand();
                p[j] = charset[p[j] & 0x0F];
            }
        }
    }

	void initServerConfig(void) {
	    int j;
	
	    getRandomHexChars(server.runid,REDIS_RUN_ID_SIZE);
	    server.configfile = NULL;
	    server.hz = REDIS_DEFAULT_HZ;
	    server.runid[REDIS_RUN_ID_SIZE] = '\0';
	    server.arch_bits = (sizeof(long) == 8) ? 64 : 32;
	    server.port = REDIS_SERVERPORT;
	    server.repl_ping_slave_period = REDIS_REPL_PING_SLAVE_PERIOD;
	    server.repl_timeout = REDIS_REPL_TIMEOUT;
	    server.repl_min_slaves_to_write = REDIS_DEFAULT_MIN_SLAVES_TO_WRITE;
	    server.repl_min_slaves_max_lag = REDIS_DEFAULT_MIN_SLAVES_MAX_LAG;
	
	    /* Replication related */
	    server.masterauth = NULL;
	    server.masterhost = NULL;
	    server.masterport = 6379;
	    server.master = NULL;
	    server.cached_master = NULL;
	    server.repl_master_initial_offset = -1;
	    server.repl_state = REDIS_REPL_NONE;
	    server.repl_syncio_timeout = REDIS_REPL_SYNCIO_TIMEOUT;
	    server.repl_serve_stale_data = REDIS_DEFAULT_SLAVE_SERVE_STALE_DATA;
	    server.repl_slave_ro = REDIS_DEFAULT_SLAVE_READ_ONLY;
	    server.repl_down_since = 0; /* Never connected, repl is down since EVER. */
	    server.repl_disable_tcp_nodelay = REDIS_DEFAULT_REPL_DISABLE_TCP_NODELAY;
	    server.repl_diskless_sync = REDIS_DEFAULT_REPL_DISKLESS_SYNC;
	    server.repl_diskless_sync_delay = REDIS_DEFAULT_REPL_DISKLESS_SYNC_DELAY;
	    server.slave_priority = REDIS_DEFAULT_SLAVE_PRIORITY;
	    server.master_repl_offset = 0;
	
	    /* Replication partial resync backlog */
	    server.repl_backlog = NULL;
	    server.repl_backlog_size = REDIS_DEFAULT_REPL_BACKLOG_SIZE;
	    server.repl_backlog_histlen = 0;
	    server.repl_backlog_idx = 0;
	    server.repl_backlog_off = 0;
	    server.repl_backlog_time_limit = REDIS_DEFAULT_REPL_BACKLOG_TIME_LIMIT;
	    server.repl_no_slaves_since = time(NULL);
	
		/* write index */
		// server.write_index.start = (long long)((long long)(0XE20150620180647F) * getpid() * server.port * time(NULL));
		server.write_index.start = 0;
		server.write_index.current = server.write_index.start;
		server.slave_write_index = dictCreate(&slaveWriteIndexDictType,NULL);
	}

</font>

###1.2 加载配置###

####1.2.1 读取配置文件####

<font color=blue>
如果进程启动的时候已经被赋予了redis的options，则使用这些options，不再读conf文件。
</font>

<font color=green>

    void loadServerConfig(char *filename, char *options) {
        sds config = sdsempty();
        char buf[REDIS_CONFIGLINE_MAX+1];

        /* Load the file content */
        if (filename) {
            FILE *fp;

            // 如果文件名称为空，则从stdin读取
            if (filename[0] == '-' && filename[1] == '\0') {
                fp = stdin;
            } else {
                if ((fp = fopen(filename,"r")) == NULL) {
                    redisLog(REDIS_WARNING,
                        "Fatal error, can't open config file '%s'", filename);
                    exit(1);
                }
            }
            while(fgets(buf,REDIS_CONFIGLINE_MAX+1,fp) != NULL)
                config = sdscat(config,buf);
            if (fp != stdin) fclose(fp);
        }
        /* Append the additional options */
        if (options) {
            config = sdscat(config,"\n");
            config = sdscat(config,options);
        }
        loadServerConfigFromString(config);
        sdsfree(config);
    }

</font>

####1.2.2 对字符流@config逐行拆分####

<font color=blue>
这个函数中比较重要的就是如果redis的role是slave，则获取master的host和port，并把state设置为REDIS_REPL_CONNECT
</font>

<font color=green>

	void loadServerConfigFromString(char *config) {
		char *err = NULL;
		int linenum = 0, totlines, i;
		int slaveof_linenum = 0;
		sds *lines;
	
		// 按照行进行分割，结果存在lines数组中，行数为totlines
		lines = sdssplitlen(config,strlen(config),"\n",1,&totlines);
	
		for (i = 0; i < totlines; i++) {
			sds *argv;
			int argc;
	
			linenum = i+1; //记录行号，一旦出错，下面的loaderr就能说明出错所在的行号
			// 去掉tab、换行、回车等空格键
			lines[i] = sdstrim(lines[i]," \t\r\n");
			// 不处理空行和注释行
			if (lines[i][0] == '#' || lines[i][0] == '\0') continue;
	
			// 把每行再进行分割，分割结果存进@argv数组，数组elem个数为args
			argv = sdssplitargs(lines[i],&argc);
			if (argv == NULL) { // 处理argv为空的情况
				err = "Unbalanced quotes in configuration line";
				goto loaderr;
			}
			if (argc == 0) { // 处理element number为0的情况
				sdsfreesplitres(argv,argc);
				continue;
			}
			sdstolower(argv[0]);  // 把line key转换为消息
	
			/* Execute config directives */
			if (!strcasecmp(argv[0],"slaveof") && argc == 3) {
				slaveof_linenum = linenum;
				server.masterhost = sdsnew(argv[1]);
				server.masterport = atoi(argv[2]);
				server.repl_state = REDIS_REPL_CONNECT;
			} else if (!strcasecmp(argv[0],"repl-ping-slave-period") && argc == 2) {
				server.repl_ping_slave_period = atoi(argv[1]);
				if (server.repl_ping_slave_period <= 0) {
					err = "repl-ping-slave-period must be 1 or greater";
					goto loaderr;
				}
			} else if (!strcasecmp(argv[0],"repl-timeout") && argc == 2) {
				server.repl_timeout = atoi(argv[1]);
				if (server.repl_timeout <= 0) {
					err = "repl-timeout must be 1 or greater";
					goto loaderr;
				}
			} else if (!strcasecmp(argv[0],"repl-disable-tcp-nodelay") && argc==2) {
				if ((server.repl_disable_tcp_nodelay = yesnotoi(argv[1])) == -1) {
					err = "argument must be 'yes' or 'no'"; goto loaderr;
				}
			} else if (!strcasecmp(argv[0],"repl-diskless-sync") && argc==2) {
				if ((server.repl_diskless_sync = yesnotoi(argv[1])) == -1) {
					err = "argument must be 'yes' or 'no'"; goto loaderr;
				}
			} else if (!strcasecmp(argv[0],"repl-diskless-sync-delay") && argc==2) {
				server.repl_diskless_sync_delay = atoi(argv[1]);
				if (server.repl_diskless_sync_delay < 0) {
					err = "repl-diskless-sync-delay can't be negative";
					goto loaderr;
				}
			} else if (!strcasecmp(argv[0],"repl-backlog-size") && argc == 2) {
				long long size = memtoll(argv[1],NULL);
				if (size <= 0) {
					err = "repl-backlog-size must be 1 or greater.";
					goto loaderr;
				}
				resizeReplicationBacklog(size);
			} else if (!strcasecmp(argv[0],"repl-backlog-ttl") && argc == 2) {
				server.repl_backlog_time_limit = atoi(argv[1]);
				if (server.repl_backlog_time_limit < 0) {
					err = "repl-backlog-ttl can't be negative ";
					goto loaderr;
				}
			}
	
			else {
				err = "Bad directive or wrong number of arguments"; goto loaderr;
			}
			// 释放element数组
			sdsfreesplitres(argv,argc);
		}
	
		// 释放line数组
		sdsfreesplitres(lines,totlines);
		return;
	
	loaderr:
		fprintf(stderr, "\n*** FATAL CONFIG FILE ERROR ***\n");
		fprintf(stderr, "Reading the configuration file, at line %d\n", linenum);
		fprintf(stderr, ">>> '%s'\n", lines[i]);
		fprintf(stderr, "%s\n", err);
		exit(1);
	}

</font>


###1.3 检查内存###

####1.3.1 检查overcommit_memory ####
<font color=blue>

overcommit_memory文件指定了内核针对内存分配的策略，overcommit_memory值可以是0、1、2。                               

- 0， 表示内核将检查是否有足够的可用内存供应用进程使用；如果有足够的可用内存，内存申请允许；否则，内存申请失败，并把错误返回给应用进程。 
- 1， 表示内核允许分配所有的物理内存，而不管当前的内存状态如何。
- 2， 表示内核允许分配超过所有物理内存和交换空间总和的内存

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

####1.3.2 检查Huge Page ####

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

####1.3.3 综合流程 ####

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

###1.4 加载磁盘数据###

<font color=green>

	/* Function called at startup to load RDB or AOF file in memory. */
	void loadDataFromDisk(void) {
	    long long start = ustime();
	    if (server.aof_state == REDIS_AOF_ON) {
			// aof模式下记载aof文件
	        if (loadAppendOnlyFile(server.aof_filename) == REDIS_OK)
	            redisLog(REDIS_NOTICE,"DB loaded from append only file: %.3f seconds",(float)(ustime()-start)/1000000);
	    } else {
			// 加载rdb文件
	        if (rdbLoad(server.rdb_filename) == REDIS_OK) {
	            redisLog(REDIS_NOTICE,"DB loaded from disk: %.3f seconds",
	                (float)(ustime()-start)/1000000);
	        } else if (errno != ENOENT) {
	            redisLog(REDIS_WARNING,"Fatal error loading the DB: %s. Exiting.",strerror(errno));
	            exit(1);
	        }
	    }
	}

</font>

####1.4.1 加载aof文件 ####

#####1.4.1.1 创建fake client#####

<font color=blue>

fake client用于aof模式下load aof文件的时候，重放客户端请求然后把数据写进内存。redis不会对其进行回复。

</font>

<font color=green>

	int prepareClientToWrite(redisClient *c) {
	    if (c->fd <= 0) return REDIS_ERR; /* Fake client for AOF loading. */
	}
	
    // 在给client回复的时候，如果判别出是fake clent则不会对client进行回复
	void addReply(redisClient *c, robj *obj) {
	    if (prepareClientToWrite(c) != REDIS_OK) return;
	}

	/* In Redis commands are always executed in the context of a client, so in
	 * order to load the append only file we need to create a fake client. */
	// 创建一个fake client，用于重放aof文件
	struct redisClient *createFakeClient(void) {
	    struct redisClient *c = zmalloc(sizeof(*c));
	
	    selectDb(c,0);
	    c->fd = -1;  // redis根据fd判断其是否为一个fake client
	    c->name = NULL;
	    c->querybuf = sdsempty();
	    c->querybuf_peak = 0;
	    c->argc = 0;
	    c->argv = NULL;
	    c->bufpos = 0;
	    c->flags = 0;
	    c->btype = REDIS_BLOCKED_NONE;
	    /* We set the fake client as a slave waiting for the synchronization
	     * so that Redis will not try to send replies to this client. */
	    c->replstate = REDIS_REPL_WAIT_BGSAVE_START;
	    c->reply = listCreate();
	    c->reply_bytes = 0;
	    c->obuf_soft_limit_reached_time = 0;
	    c->watched_keys = listCreate();
	    c->peerid = NULL;
	    listSetFreeMethod(c->reply,decrRefCountVoid);
	    listSetDupMethod(c->reply,dupClientReplyValue);
	    initClientMultiState(c);
	    return c;
	}
		
</font>

#####1.4.1.2 加载aof文件，并重放相关的命令#####

<font color=green>

	/* Mark that we are loading in the global state and setup the fields
	 * needed to provide loading stats. */
	// 设置loading flag为1
	void startLoading(FILE *fp) {
	    struct stat sb;
	
	    /* Load the DB */
	    server.loading = 1;
	    server.loading_start_time = time(NULL);
	    server.loading_loaded_bytes = 0;
	    if (fstat(fileno(fp), &sb) == -1) {
	        server.loading_total_bytes = 0;
	    } else {
	        server.loading_total_bytes = sb.st_size;
	    }
	}
	
	/* Loading finished */
	// 设置loading flag为0
	void stopLoading(void) {
	    server.loading = 0;
	}

	/* This function is called by Redis in order to process a few events from
	 * time to time while blocked into some not interruptible operation.
	 * This allows to reply to clients with the -LOADING error while loading the
	 * data set at startup or after a full resynchronization with the master
	 * and so forth.
	 *
	 * It calls the event loop in order to process a few events. Specifically we
	 * try to call the event loop for times as long as we receive acknowledge that
	 * some event was processed, in order to go forward with the accept, read,
	 * write, close sequence needed to serve a client.
	 *
	 * The function returns the total number of events processed. */
	// 阻塞在某个任务上时，抽出时间执行一些外部的请求任务
	int processEventsWhileBlocked(void) {
	    int iterations = 4; /* See the function top-comment. */
	    int count = 0;
	    while (iterations--) {
	        int events = aeProcessEvents(server.el, AE_FILE_EVENTS|AE_DONT_WAIT);
	        if (!events) break;
	        count += events;
	    }
	    return count;
	}

	/* Replay the append log file. On success REDIS_OK is returned. On non fatal
	 * error (the append only file is zero-length) REDIS_ERR is returned. On
	 * fatal error an error message is logged and the program exists. */
	// 重放aof文件，成功返回REDIS_OK，遇到fatal error[aof文件大小为0]则返回REDIS_ERR
	// 遇到fatal error，则把错误信息写入log，redis退出
	int loadAppendOnlyFile(char *filename) {
	    struct redisClient *fakeClient;
	    FILE *fp = fopen(filename,"r");
	    struct redis_stat sb;
	    int old_aof_state = server.aof_state;
	    long loops = 0;
	    off_t valid_up_to = 0; /* Offset of the latest well-formed command loaded. */
	
	    // 文件大小为0
	    if (fp && redis_fstat(fileno(fp),&sb) != -1 && sb.st_size == 0) {
	        server.aof_current_size = 0;
	        fclose(fp);
	        return REDIS_ERR;
	    }
	
	    if (fp == NULL) {
	        redisLog(REDIS_WARNING,"Fatal error: can't open the append log file for reading: %s",strerror(errno));
	        exit(1);
	    }
	
	    /* Temporarily disable AOF, to prevent EXEC from feeding a MULTI
	     * to the same file we're about to read. */
		// 设置状态为REDIS_AOF_OFF，防止redis向aof文件写数据
	    server.aof_state = REDIS_AOF_OFF;
	
	    fakeClient = createFakeClient();
	    startLoading(fp); // 设置server.loading为1
	
	    while(1) {
	        int argc, j;
	        unsigned long len;
	        robj **argv;
	        char buf[128];
	        sds argsds;
	        struct redisCommand *cmd;
	
	        /* Serve the clients from time to time */
	        if (!(loops++ % 1000)) {
	            loadingProgress(ftello(fp)); // server.loading_loaded_bytes = pos;
	            processEventsWhileBlocked();
	        }
	
	        if (fgets(buf,sizeof(buf),fp) == NULL) {
	            if (feof(fp))
	                break;
	            else
	                goto readerr;
	        }
	        if (buf[0] != '*') goto fmterr;
	        if (buf[1] == '\0') goto readerr;
	        argc = atoi(buf+1); // 有效参数个数
	        if (argc < 1) goto fmterr;
	
	        argv = zmalloc(sizeof(robj*)*argc);
	        fakeClient->argc = argc;
	        fakeClient->argv = argv;
	
			// 循环读出所有的参数
	        for (j = 0; j < argc; j++) {
	            if (fgets(buf,sizeof(buf),fp) == NULL) {
	                fakeClient->argc = j; /* Free up to j-1. */
	                freeFakeClientArgv(fakeClient);
	                goto readerr;
	            }
	            if (buf[0] != '$') goto fmterr;
	            len = strtol(buf+1,NULL,10);
	            argsds = sdsnewlen(NULL,len);
	            if (len && fread(argsds,len,1,fp) == 0) {
	                sdsfree(argsds);
	                fakeClient->argc = j; /* Free up to j-1. */
	                freeFakeClientArgv(fakeClient);
	                goto readerr;
	            }
	            argv[j] = createObject(REDIS_STRING,argsds); // 为某个参数赋值
	            if (fread(buf,2,1,fp) == 0) {
	                fakeClient->argc = j+1; /* Free up to j. */
	                freeFakeClientArgv(fakeClient);
	                goto readerr; /* discard CRLF */
	            }
	        }
	
	        /* Command lookup */
			// 查找命令是否有效
	        cmd = lookupCommand(argv[0]->ptr);
	        if (!cmd) {
	            redisLog(REDIS_WARNING,"Unknown command '%s' reading the append only file", (char*)argv[0]->ptr);
	            exit(1);
	        }
	
	        /* Run the command in the context of a fake client */
			// 重放命令
	        cmd->proc(fakeClient);
	
	        /* The fake client should not have a reply */
			// fake client不应该收到reply
	        redisAssert(fakeClient->bufpos == 0 && listLength(fakeClient->reply) == 0);
	        /* The fake client should never get blocked */
			// fake client不应该处于blocked模式
	        redisAssert((fakeClient->flags & REDIS_BLOCKED) == 0);
	
	        /* Clean up. Command code may have changed argv/argc so we use the
	         * argv/argc of the client instead of the local variables. */
			// 释放深圳的argv资源
	        freeFakeClientArgv(fakeClient);
	        if (server.aof_load_truncated) valid_up_to = ftello(fp);
	    }
	
	    /* This point can only be reached when EOF is reached without errors.
	     * If the client is in the middle of a MULTI/EXEC, log error and quit. */
	    if (fakeClient->flags & REDIS_MULTI) goto uxeof;
	
	loaded_ok: /* DB loaded, cleanup and return REDIS_OK to the caller. */
	    fclose(fp);
	    freeFakeClient(fakeClient);
	    server.aof_state = old_aof_state;
	    stopLoading();
	    aofUpdateCurrentSize();
	    server.aof_rewrite_base_size = server.aof_current_size;
	    return REDIS_OK;
	
	readerr: /* Read error. If feof(fp) is true, fall through to unexpected EOF. */
	    if (!feof(fp)) {
	        redisLog(REDIS_WARNING,"Unrecoverable error reading the append only file: %s", strerror(errno));
	        exit(1);
	    }
	
	uxeof: /* Unexpected AOF end of file. */
	    if (server.aof_load_truncated) {
	        redisLog(REDIS_WARNING,"!!! Warning: short read while loading the AOF file !!!");
	        redisLog(REDIS_WARNING,"!!! Truncating the AOF at offset %llu !!!",
	            (unsigned long long) valid_up_to);
	        if (valid_up_to == -1 || truncate(filename,valid_up_to) == -1) {
	            if (valid_up_to == -1) {
	                redisLog(REDIS_WARNING,"Last valid command offset is invalid");
	            } else {
	                redisLog(REDIS_WARNING,"Error truncating the AOF file: %s",
	                    strerror(errno));
	            }
	        } else {
	            /* Make sure the AOF file descriptor points to the end of the
	             * file after the truncate call. */
	            if (server.aof_fd != -1 && lseek(server.aof_fd,0,SEEK_END) == -1) {
	                redisLog(REDIS_WARNING,"Can't seek the end of the AOF file: %s",
	                    strerror(errno));
	            } else {
	                redisLog(REDIS_WARNING,
	                    "AOF loaded anyway because aof-load-truncated is enabled");
	                goto loaded_ok;
	            }
	        }
	    }
	    redisLog(REDIS_WARNING,"Unexpected end of file reading the append only file. You can: 1) Make a backup of your AOF file, then use ./redis-check-aof --fix <filename>. 2) Alternatively you can set the 'aof-load-truncated' configuration option to yes and restart the server.");
	    exit(1);
	
	fmterr: /* Format error. */
	    redisLog(REDIS_WARNING,"Bad file format reading the append only file: make a backup of your AOF file, then use ./redis-check-aof --fix <filename>");
	    exit(1);
	}
	
</font>
	
####1.4.2 加载rdb文件 ####
	
<font color=green>

	int rdbLoad(char *filename) {
	    uint32_t dbid;
	    int type, rdbver;
	    redisDb *db = server.db+0;
	    char buf[1024];
	    long long expiretime, now = mstime();
	    FILE *fp;
	    rio rdb;
	
	    if ((fp = fopen(filename,"r")) == NULL) return REDIS_ERR;
	
	    rioInitWithFile(&rdb,fp);
	    rdb.update_cksum = rdbLoadProgressCallback;
	    rdb.max_processing_chunk = server.loading_process_events_interval_bytes;
	    if (rioRead(&rdb,buf,9) == 0) goto eoferr;
	    buf[9] = '\0';
		// 检查文件开头
	    if (memcmp(buf,"REDIS",5) != 0) {
	        fclose(fp);
	        redisLog(REDIS_WARNING,"Wrong signature trying to load DB from file");
	        errno = EINVAL;
	        return REDIS_ERR;
	    }
	    rdbver = atoi(buf+5);
	    if (rdbver < 1 || rdbver > REDIS_RDB_VERSION) {
	        fclose(fp);
	        redisLog(REDIS_WARNING,"Can't handle RDB format version %d",rdbver);
	        errno = EINVAL;
	        return REDIS_ERR;
	    }
	
	    startLoading(fp);
	    while(1) {
	        robj *key, *val;
	        expiretime = -1;
	
	        /* Read type. */
			// 读取key的超时时间
	        if ((type = rdbLoadType(&rdb)) == -1) goto eoferr;
	        if (type == REDIS_RDB_OPCODE_EXPIRETIME) {
	            if ((expiretime = rdbLoadTime(&rdb)) == -1) goto eoferr;
	            /* We read the time so we need to read the object type again. */
	            if ((type = rdbLoadType(&rdb)) == -1) goto eoferr;
	            /* the EXPIRETIME opcode specifies time in seconds, so convert
	             * into milliseconds. */
	            expiretime *= 1000;
	        } else if (type == REDIS_RDB_OPCODE_EXPIRETIME_MS) {
	            /* Milliseconds precision expire times introduced with RDB
	             * version 3. */
	            if ((expiretime = rdbLoadMillisecondTime(&rdb)) == -1) goto eoferr;
	            /* We read the time so we need to read the object type again. */
	            if ((type = rdbLoadType(&rdb)) == -1) goto eoferr;
	        }
	
			// 到了文件末尾，终止流程
	        if (type == REDIS_RDB_OPCODE_EOF)
	            break;
	
	        /* Handle SELECT DB opcode as a special case */
			// 切换db
	        if (type == REDIS_RDB_OPCODE_SELECTDB) {
	            if ((dbid = rdbLoadLen(&rdb,NULL)) == REDIS_RDB_LENERR)
	                goto eoferr;
	            if (dbid >= (unsigned)server.dbnum) {
	                redisLog(REDIS_WARNING,"FATAL: Data file was created with a Redis server configured to handle more than %d databases. Exiting\n", server.dbnum);
	                exit(1);
	            }
	            db = server.db+dbid;
	            continue;
	        }
	        /* Read key */
			// 获取key
	        if ((key = rdbLoadStringObject(&rdb)) == NULL) goto eoferr;
	        /* Read value */
			// 获取value
	        if ((val = rdbLoadObject(type,&rdb)) == NULL) goto eoferr;
	        /* Check if the key already expired. This function is used when loading
	         * an RDB file from disk, either at startup, or when an RDB was
	         * received from the master. In the latter case, the master is
	         * responsible for key expiry. If we would expire keys here, the
	         * snapshot taken by the master may not be reflected on the slave. */
			// 判断key是否超时
	        if (server.masterhost == NULL && expiretime != -1 && expiretime < now) {
	            decrRefCount(key);
	            decrRefCount(val);
	            continue;
	        }
	        /* Add the new object in the hash table */
			// 如果没有超时，则插入数据库
	        dbAdd(db,key,val);
	
	        /* Set the expire time if needed */
			// 设置key的超时时间
	        if (expiretime != -1) setExpire(db,key,expiretime);
	
			// 释放key
	        decrRefCount(key);
	    }
	    /* Verify the checksum if RDB version is >= 5 */
		// 读取文件的checksum，判断其rdb是否完整
	    if (rdbver >= 5 && server.rdb_checksum) {
	        uint64_t cksum, expected = rdb.cksum;
	
	        if (rioRead(&rdb,&cksum,8) == 0) goto eoferr;
	        memrev64ifbe(&cksum);
	        if (cksum == 0) {
	            redisLog(REDIS_WARNING,"RDB file was saved with checksum disabled: no check performed.");
	        } else if (cksum != expected) {
	            redisLog(REDIS_WARNING,"Wrong RDB checksum. Aborting now.");
	            exit(1);
	        }
	    }
	
	    fclose(fp);
	    stopLoading();
	    return REDIS_OK;
	
	eoferr: /* unexpected end of file is handled here with a fatal exit */
	    redisLog(REDIS_WARNING,"Short read or OOM loading DB. Unrecoverable error, aborting now.");
	    exit(1);
	    return REDIS_ERR; /* Just to avoid warning */
	}

</font>

##2 slave 流程##

###2.1 slave模式###

<font color=blue>
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
	
##3 master流程##


##4 replication综合流程##

###4.1 replication流程文字描述###

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

###4.2 replication过程中slave与master的函数流程图###

![](./redis_replication.png)

###4.3 replication过程中slave与master的状态变化图###

![](./redis_replication_interactive.png)


## 参考文档：##

- 1 redis/src/replication.c
- 2 http://redis.readthedocs.org/en/latest/topic/replication.html
- 3 http://www.360doc.com/content/11/1205/14/7936054_169834858.shtml

