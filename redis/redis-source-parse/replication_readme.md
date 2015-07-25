# redis server replication 源码分析#
---
*written by Alex Stocks on 2015/07/19*

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

###1.5 半道出家 ###

<font color=blue>

>正常运行的实例[master or slave]，收到slaveof命令后更换master，启动slave模式。
>
>先断绝与已有的master以及slaves之间的连接，并放弃收到的或者将要发出的增量同步数据，然后初始化相关配置，设置状态为REDIS_REPL_CONNECT。
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

    /* Set replication to the specified master address and port. */
    void replicationSetMaster(char *ip, int port) {
        sdsfree(server.masterhost);
        server.masterhost = sdsnew(ip);
        server.masterport = port;
        if (server.master) freeClient(server.master); // 断开与旧的master之间的连接
        disconnectAllBlockedClients(); /* Clients blocked in master, now slave. */ // 断开与所有client之间的连接
        disconnectSlaves(); /* Force our slaves to resync with us as well. */ // 断开与所有slave之间的连接
        replicationDiscardCachedMaster(); /* Don't try a PSYNC. */ // 放弃master发来的增量数据
        freeReplicationBacklog(); /* Don't allow our chained slaves to PSYNC. */ // 不再把增量数据同步给slaves
        cancelReplicationHandshake(); // 停止心跳
        // 设置replication初始状态以及相关字段数据，同loadServerConfigFromString分析slaveof配置时设置相关字段的值一样
        server.repl_state = REDIS_REPL_CONNECT;
        server.master_repl_offset = 0;
        server.repl_down_since = 0;
    }

</font>

##2 slave 流程##

###2.1 连接master###

<font color=blue>

redis的timer响应函数ServerCron每秒调用一次replication的周期函数replicationCron。这个函数检查到slave还没有成功连接master时，先进行连接动作。

连接动作由connectWithMaster完成。连接过程中会发出psync命令，尔后把状态更改为REDIS_REPL_CONNECTING。
</font>

<font color=green>

    int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData) {
        /* Replication cron function -- used to reconnect to master and
         * to detect transfer failures. */
        run_with_period(1000) replicationCron();
    }

    /* Replication cron function, called 1 time per second. */
    void replicationCron(void) {
        /* Check if we should connect to a MASTER */
        if (server.repl_state == REDIS_REPL_CONNECT) {
            redisLog(REDIS_NOTICE,"Connecting to MASTER %s:%d",
                server.masterhost, server.masterport);
            if (connectWithMaster() == REDIS_OK) {
                redisLog(REDIS_NOTICE,"MASTER <-> SLAVE sync started");
            }
        }
    }

    int connectWithMaster(void) {
        int fd;

        fd = anetTcpNonBlockBindConnect(NULL,
            server.masterhost,server.masterport,REDIS_BIND_ADDR);
        if (fd == -1) {
            redisLog(REDIS_WARNING,"Unable to connect to MASTER: %s",
                strerror(errno));
            return REDIS_ERR;
        }

        if (aeCreateFileEvent(server.el,fd,AE_READABLE|AE_WRITABLE,syncWithMaster,NULL) ==
                AE_ERR)
        {
            close(fd);
            redisLog(REDIS_WARNING,"Can't create readable event for SYNC");
            return REDIS_ERR;
        }

        server.repl_transfer_lastio = server.unixtime;
        server.repl_transfer_s = fd;
        server.repl_state = REDIS_REPL_CONNECTING;
        return REDIS_OK;
    }

</font>

###2.2 断开与master之间的连接###

<font color=blue>

如果处于REDIS_REPL_CONNECTING or REDIS_REPL_RECEIVE_PONG的状态，而且距离上次接收数据时间已经超时，则断开与master之间的连接，把状态置为REDIS_REPL_CONNECT。

</font>

<font color=green>

    void replicationCron(void) {
        /* Non blocking connection timeout? */
        if (server.masterhost &&
            (server.repl_state == REDIS_REPL_CONNECTING ||
             server.repl_state == REDIS_REPL_RECEIVE_PONG) &&
            (time(NULL)-server.repl_transfer_lastio) > server.repl_timeout)
        {
            redisLog(REDIS_WARNING,"Timeout connecting to the MASTER...");
            undoConnectWithMaster();
        }
    }

    void undoConnectWithMaster(void) {
        int fd = server.repl_transfer_s;

        redisAssert(server.repl_state == REDIS_REPL_CONNECTING ||
                    server.repl_state == REDIS_REPL_RECEIVE_PONG);
        aeDeleteFileEvent(server.el,fd,AE_READABLE|AE_WRITABLE);
        close(fd);
        server.repl_transfer_s = -1;
        server.repl_state = REDIS_REPL_CONNECT;
    }

</font>

###2.3 判断与master之间的连接是否成功并发出PING命令###

<font color=blue>

>正常的connect异步流程是：先connect，而后判断fd是否可写，最后再判断连接是否有误。而上面的连接过程中，connect成功后就直接发出了PSYNC命令，所以收到其reply函数syncWithMaster就意味着server.sync_transfer_s确实可写。
>
>syncWithMaster函数起始逻辑就是判断fd是否有error，这个是继续连接流程的第三步，如果没有error就可以确认连接可读可写而且没有error，此时就可以删除对可写事件的关注。
>
>确定没有错误后再发出PING命令，状态更改为REDIS_REPL_RECEIVE_PONG。

</font>

<font color=green>

    void syncWithMaster(aeEventLoop *el, int fd, void *privdata, int mask) {
        char tmpfile[256], *err;
        int dfd, maxtries = 5;
        int sockerr = 0, psync_result;
        socklen_t errlen = sizeof(sockerr);
        REDIS_NOTUSED(el);
        REDIS_NOTUSED(privdata);
        REDIS_NOTUSED(mask);

        /* If this event fired after the user turned the instance into a master
         * with SLAVEOF NO ONE we must just return ASAP. */
        // 如果收到了SLAVEOF NO ONE命令，则立即关闭与master之间的连接，并退出
        if (server.repl_state == REDIS_REPL_NONE) {
            close(fd);
            return;
        }

        // 检查连接是否有问题
        /* Check for errors in the socket. */
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &sockerr, &errlen) == -1)
            sockerr = errno;
        if (sockerr) {
            aeDeleteFileEvent(server.el,fd,AE_READABLE|AE_WRITABLE);
            redisLog(REDIS_WARNING,"Error condition on socket for SYNC: %s",
                strerror(sockerr));
            goto error;
        }

        /* If we were connecting, it's time to send a non blocking PING, we want to
         * make sure the master is able to reply before going into the actual
         * replication process where we have long timeouts in the order of
         * seconds (in the meantime the slave would block). */
        // 如果还在连接中而未确认连接已经成功，需要确认master能够对PING命令回复PONG，则需要以阻塞形式把PING命令发送出去
        if (server.repl_state == REDIS_REPL_CONNECTING) {
            redisLog(REDIS_NOTICE,"Non blocking connect for SYNC fired the event.");
            /* Delete the writable event so that the readable event remains
             * registered and we can wait for the PONG reply. */
            // 删除写事件，只保留读事件，等地啊PONG响应
            aeDeleteFileEvent(server.el,fd,AE_WRITABLE);
            server.repl_state = REDIS_REPL_RECEIVE_PONG;
            /* Send the PING, don't check for errors at all, we have the timeout
             * that will take care about this. */
            // 此处并不检查是否遇到error，如果超时后内容还没有发送出去，上面2.2小节与处理这个逻辑：超时处理
            syncWrite(fd,"PING\r\n",6,100);
            return;
        }
    }

</font>

###2.3.1 以阻塞方式发出PING命令 ###

<font color=green>

    /* Wait for milliseconds until the given file descriptor becomes
     * writable/readable/exception */
    // 借用poll函数阻塞@milliseconds，然后判断@fd是否满足条件@mask
    int aeWait(int fd, int mask, long long milliseconds) {
        struct pollfd pfd;
        int retmask = 0, retval;

        memset(&pfd, 0, sizeof(pfd));
        pfd.fd = fd;
        if (mask & AE_READABLE) pfd.events |= POLLIN;
        if (mask & AE_WRITABLE) pfd.events |= POLLOUT;

        if ((retval = poll(&pfd, 1, milliseconds))== 1) {
            if (pfd.revents & POLLIN) retmask |= AE_READABLE;
            if (pfd.revents & POLLOUT) retmask |= AE_WRITABLE;
        if (pfd.revents & POLLERR) retmask |= AE_WRITABLE;
            if (pfd.revents & POLLHUP) retmask |= AE_WRITABLE;
            return retmask;
        } else {
            return retval;
        }
    }

    /* Redis performs most of the I/O in a nonblocking way, with the exception
     * of the SYNC command where the slave does it in a blocking way, and
     * the MIGRATE command that must be blocking in order to be atomic from the
     * point of view of the two instances (one migrating the key and one receiving
     * the key). This is why need the following blocking I/O functions.
     *
     * All the functions take the timeout in milliseconds. */
    // redis执行大部分命令都是以异步方式运行，但sync和migrate任务除外。
    // 因为migrate任务是执行数据同步工作，命令执行完就意味着两端的数据是一样的，所以须以同步方式执行

    #define REDIS_SYNCIO_RESOLUTION 10 /* Resolution in milliseconds */

    /* Write the specified payload to 'fd'. If writing the whole payload will be
     * done within 'timeout' milliseconds the operation succeeds and 'size' is
     * returned. Otherwise the operation fails, -1 is returned, and an unspecified
     * partial write could be performed against the file descriptor. */
    // 以阻塞形式把ptr[size]通过连接@fd发送出去
    ssize_t syncWrite(int fd, char *ptr, ssize_t size, long long timeout) {
        ssize_t nwritten, ret = size;
        long long start = mstime();
        long long remaining = timeout;

        while(1) {
            // 修正等待时间为10ms，因为linux给每个进程分配的时间片长度是10ms
            long long wait = (remaining > REDIS_SYNCIO_RESOLUTION) ?
                              remaining : REDIS_SYNCIO_RESOLUTION;
            long long elapsed;

            /* Optimistically try to write before checking if the file descriptor
             * is actually writable. At worst we get EAGAIN. */
            nwritten = write(fd,ptr,size);
            if (nwritten == -1) {
                if (errno != EAGAIN) return -1;
            } else {
                ptr += nwritten;
                size -= nwritten;
            }
            if (size == 0) return ret;

            /* Wait */
            aeWait(fd,AE_WRITABLE,wait);
            elapsed = mstime() - start;
            if (elapsed >= timeout) {
                errno = ETIMEDOUT;
                return -1;
            }
            remaining = timeout - elapsed;
        }
    }

</font>

###2.4 接收PING响应并进行数据同步 ###

<font color=blue>

>再次收到对PSYNC命令的响应，就是收到PONG响应。如果需要进行密码验证，就进行发送密码进行验证，注意发送的密码就是slave自己的密码，这里隐含着一个条件：master-slave级联模式下主从的密码须一致。
>
>尔后通过AUTH & REPLCONF命令发送密码验证和自己的listenning port后，先尝试进行增量同步。这一步其实涉及到 redis 2.8版本以前的一个bug：如果master和slave之间正在执行数据同步的时候网络闪断，那么连接重新建立以后每次都要重新全量的接收数据！所以redis 2.8以后的版本就有了这个patch。
>
> 函数的流程为：

- 1 如果server.repl_state为REDIS_REPL_RECEIVE_PONG，则以阻塞的方式读取回复；
- 2 如果需要验证密码，则发送AUTH passwd进行密码验证；
- 3 把自己的监听端口告诉master；
- 4 尝试进行增量同步，注册增量同步回函数readQueryFromClient；
- 5 如果增量同步失败，发送SYNC命令进行全量同步；
- 6 创建临时文件，并注册增量同步回调函数readSyncBulkPayload。

</font>

<font color=green>

    void syncWithMaster(aeEventLoop *el, int fd, void *privdata, int mask) {
        /* Receive the PONG command. */
        if (server.repl_state == REDIS_REPL_RECEIVE_PONG) {
            char buf[1024];

            /* Delete the readable event, we no longer need it now that there is
             * the PING reply to read. */
            aeDeleteFileEvent(server.el,fd,AE_READABLE);

            /* Read the reply with explicit timeout. */
            buf[0] = '\0';
            // syncReadLine是以阻塞地方式读取回复，同syncWrite
            if (syncReadLine(fd,buf,sizeof(buf),
                server.repl_syncio_timeout*1000) == -1)
            {
                redisLog(REDIS_WARNING,
                    "I/O error reading PING reply from master: %s",
                    strerror(errno));
                goto error;
            }

            /* We accept only two replies as valid, a positive +PONG reply
             * (we just check for "+") or an authentication error.
             * Note that older versions of Redis replied with "operation not
             * permitted" instead of using a proper error code, so we test
             * both. */
            // 检查回复内容，处理除却noauth之类的其他错误
            if (buf[0] != '+' &&
                strncmp(buf,"-NOAUTH",7) != 0 &&
                strncmp(buf,"-ERR operation not permitted",28) != 0)
            {
                redisLog(REDIS_WARNING,"Error reply to PING from master: '%s'",buf);
                goto error;
            } else {
                redisLog(REDIS_NOTICE,
                    "Master replied to PING, replication can continue...");
            }
        }

        /* AUTH with the master if required. */
        if(server.masterauth) {
            err = sendSynchronousCommand(fd,"AUTH",server.masterauth,NULL);
            if (err[0] == '-') {
                redisLog(REDIS_WARNING,"Unable to AUTH to MASTER: %s",err);
                sdsfree(err);
                goto error;
            }
            sdsfree(err);
        }

        /* Set the slave port, so that Master's INFO command can list the
         * slave listening port correctly. */
        // 向server汇报自己的接收数据的端口
        {
            sds port = sdsfromlonglong(server.port);
            err = sendSynchronousCommand(fd,"REPLCONF","listening-port",port,
                                             NULL);
            sdsfree(port);
            /* Ignore the error if any, not all the Redis versions support
             * REPLCONF listening-port. */
            if (err[0] == '-') {
                redisLog(REDIS_NOTICE,"(Non critical) Master does not understand REPLCONF listening-port: %s", err);
            }
            sdsfree(err);
        }

        /* Try a partial resynchonization. If we don't have a cached master
         * slaveTryPartialResynchronization() will at least try to use PSYNC
         * to start a full resynchronization so that we get the master run id
         * and the global offset, to try a partial resync at the next
         * reconnection attempt. */
        // 尝试增量同步
        psync_result = slaveTryPartialResynchronization(fd);
        if (psync_result == PSYNC_CONTINUE) {
            redisLog(REDIS_NOTICE, "MASTER <-> SLAVE sync: Master accepted a Partial Resynchronization.");
            return;
        }

        /* Fall back to SYNC if needed. Otherwise psync_result == PSYNC_FULLRESYNC
         * and the server.repl_master_runid and repl_master_initial_offset are
         * already populated. */
        // 如果不支持增量同步，那么发送SYNC命令，准备进行全量同步
        if (psync_result == PSYNC_NOT_SUPPORTED) {
            redisLog(REDIS_NOTICE,"Retrying with SYNC...");
            if (syncWrite(fd,"SYNC\r\n",6,server.repl_syncio_timeout*1000) == -1) {
                redisLog(REDIS_WARNING,"I/O error writing to MASTER: %s",
                    strerror(errno));
                goto error;
            }
        }

        /* Prepare a suitable temp file for bulk transfer */
        // 创建一个临时文件，用于接收全量数据
        while(maxtries--) {
            snprintf(tmpfile,256,
                "temp-%d.%ld.rdb",(int)server.unixtime,(long int)getpid());
            dfd = open(tmpfile,O_CREAT|O_WRONLY|O_EXCL,0644);
            if (dfd != -1) break;
            sleep(1);
        }
        if (dfd == -1) {
            redisLog(REDIS_WARNING,"Opening the temp file needed for MASTER <-> SLAVE synchronization: %s",strerror(errno));
            goto error;
        }

        /* Setup the non blocking download of the bulk file. */
        // 注册readSyncBulkPayload，以用于接收全量数据
        if (aeCreateFileEvent(server.el,fd, AE_READABLE,readSyncBulkPayload,NULL)
                == AE_ERR)
        {
            redisLog(REDIS_WARNING,
                "Can't create readable event for SYNC: %s (fd=%d)",
                strerror(errno),fd);
            goto error;
        }

        server.repl_state = REDIS_REPL_TRANSFER;
        server.repl_transfer_size = -1;
        server.repl_transfer_read = 0;
        server.repl_transfer_last_fsync_off = 0;
        server.repl_transfer_fd = dfd;
        server.repl_transfer_lastio = server.unixtime;
        server.repl_transfer_tmpfile = zstrdup(tmpfile);
        return;

    error:
        close(fd);
        server.repl_transfer_s = -1;
        server.repl_state = REDIS_REPL_CONNECT;
        return;
    }

</font>

####2.4.1 以同步方式发送AUTH & REPLCONF命令，并等待reply ####

    /* Send a synchronous command to the master. Used to send AUTH and
     * REPLCONF commands before starting the replication with SYNC.
     *
     * The command returns an sds string representing the result of the
     * operation. On error the first byte is a "-".
     */
    char *sendSynchronousCommand(int fd, ...) {
        va_list ap;
        sds cmd = sdsempty();
        char *arg, buf[256];

        /* Create the command to send to the master, we use simple inline
         * protocol for simplicity as currently we only send simple strings. */
        va_start(ap,fd);
        while(1) {
            arg = va_arg(ap, char*);
            if (arg == NULL) break;

            if (sdslen(cmd) != 0) cmd = sdscatlen(cmd," ",1);
            cmd = sdscat(cmd,arg);
        }
        cmd = sdscatlen(cmd,"\r\n",2);

        /* Transfer command to the server. */
        if (syncWrite(fd,cmd,sdslen(cmd),server.repl_syncio_timeout*1000) == -1) {
            sdsfree(cmd);
            return sdscatprintf(sdsempty(),"-Writing to master: %s",
                    strerror(errno));
        }
        sdsfree(cmd);

        /* Read the reply from the server. */
        if (syncReadLine(fd,buf,sizeof(buf),server.repl_syncio_timeout*1000) == -1)
        {
            return sdscatprintf(sdsempty(),"-Reading from master: %s",
                    strerror(errno));
        }
        return sdsnew(buf);
    }

</font>

####2.4.2 增量同步尝试 ####

<font color=blue>

有增量同步特性的主服务器为被发送的复制流创建一个内存缓冲区（in-memory backlog）， 并且主服务器和所有从服务器之间都记录一个复制偏移量（replication offset）和一个主服务器 ID （master run id），当出现闪断但是slave又重新连接成功后，如果：

- 如果从服务器记录的主服务器 ID 和当前要连接的主服务器的 ID 相同
- 并且从服务器记录的偏移量所指定的数据仍然保存在主服务器的复制流缓冲区里面

满足以上两个条件，那么主服务器会向从服务器发送断线时缺失的那部分数据。否则的话，从服务器就要执执行全量同步操作。

</font>

<font color=green>

    //master拒绝增量同步，释放与master之间的连接
    void replicationDiscardCachedMaster(void) {
        if (server.cached_master == NULL) return;

        redisLog(REDIS_NOTICE,"Discarding previously cached master state.");
        server.cached_master->flags &= ~REDIS_MASTER;
        freeClient(server.cached_master);
        server.cached_master = NULL;
    }

    /* Turn the cached master into the current master, using the file descriptor
     * passed as argument as the socket for the new master.
     *
     * This function is called when successfully setup a partial resynchronization
     * so the stream of data that we'll receive will start from were this
     * master left. */
    // 如果master答应增量同步，则把state更改为REDIS_REPL_CONNECTED，然后注册增量同步回调函数readQueryFromClient
    void replicationResurrectCachedMaster(int newfd) {
        server.master = server.cached_master;
        server.cached_master = NULL;
        server.master->fd = newfd;
        server.master->flags &= ~(REDIS_CLOSE_AFTER_REPLY|REDIS_CLOSE_ASAP);
        server.master->authenticated = 1;
        server.master->lastinteraction = server.unixtime;
        server.repl_state = REDIS_REPL_CONNECTED;

        /* Re-add to the list of clients. */
        // 把master作为新的client放在client链表尾部，然后注册增量同步回调函数readQueryFromClient
        listAddNodeTail(server.clients,server.master);
        if (aeCreateFileEvent(server.el, newfd, AE_READABLE,
                              readQueryFromClient, server.master)) {
            redisLog(REDIS_WARNING,"Error resurrecting the cached master, impossible to add the readable handler: %s", strerror(errno));
            // 注册失败，就尽快关闭与master之间的连接
            freeClientAsync(server.master); /* Close ASAP. */
        }

        /* We may also need to install the write handler as well if there is
         * pending data in the write buffers. */
        // 如果还有待reply的数据没有发送出去，就注册reply函数sendReplyToClient
        if (server.master->bufpos || listLength(server.master->reply)) {
            if (aeCreateFileEvent(server.el, newfd, AE_WRITABLE,
                              sendReplyToClient, server.master)) {
                redisLog(REDIS_WARNING,"Error resurrecting the cached master, impossible to add the writable handler: %s", strerror(errno));
                freeClientAsync(server.master); /* Close ASAP. */
            }
        }
    }

    /*
     * 这个函数用于应对与master之间的增量同步。如果没有cached_master，则PSYNC的参
     * 数可以设置为"-1"，至少可以全量同步所需要的两个参数：master的server id和
     * offset。这个函数用来被函数syncWithMaster调用，所以应满足下面两个条件:
     *
     * 1) master和slave之间的连接已经建立起来。
     * 2) 这个函数不会close掉这个连接，接下来的增量同步还会使用到它。
     *
     * 函数的返回值:
     *
     * PSYNC_CONTINUE: 准备好进行增量同步，此时可以通过
     *                 replicationResurrectCachedMaster函数保存
     *                 与master之间的连接
     * PSYNC_FULLRESYNC: master虽然支持增量同步，但是与slave之前并没有进行过数
     *                   据同步，二者之间应该进行全量数据同步，master会把master
     *                   run_id和全局复制offset告知slave
     * PSYNC_NOT_SUPPORTED: master不支持PSYNC命令
     */

    #define PSYNC_CONTINUE 0
    #define PSYNC_FULLRESYNC 1
    #define PSYNC_NOT_SUPPORTED 2

    int slaveTryPartialResynchronization(int fd) {
        char *psync_runid;
        char psync_offset[32];
        sds reply;

        /* Initially set repl_master_initial_offset to -1 to mark the current
         * master run_id and offset as not valid. Later if we'll be able to do
         * a FULL resync using the PSYNC command we'll set the offset at the
         * right value, so that this information will be propagated to the
         * client structure representing the master into server.master. */
        // 把repl_master_initial_offset赋值为-1，以说明master run_id和offset无效
        server.repl_master_initial_offset = -1;

        // 把自己记录的server runid和offset发送给master
        if (server.cached_master) {
            psync_runid = server.cached_master->replrunid;
            snprintf(psync_offset,sizeof(psync_offset),"%lld", server.cached_master->reploff+1);
            redisLog(REDIS_NOTICE,"Trying a partial resynchronization (request %s:%s).", psync_runid, psync_offset);
        } else {
            redisLog(REDIS_NOTICE,"Partial resynchronization not possible (no cached master)");
            psync_runid = "?";
            memcpy(psync_offset,"-1",3);
        }

        /* Issue the PSYNC command */
        // 以同步方式发出PSYNC命令以及其参数runid & offset，并获取reply
        reply = sendSynchronousCommand(fd,"PSYNC",psync_runid,psync_offset,NULL);

        //  如果回复是FULLRESYNC，则分析回复的runid & offset
        if (!strncmp(reply,"+FULLRESYNC",11)) {
            char *runid = NULL, *offset = NULL;

            /* FULL RESYNC, parse the reply in order to extract the run id
             * and the replication offset. */
            runid = strchr(reply,' ');
            if (runid) {
                runid++;
                offset = strchr(runid,' ');
                if (offset) offset++;
            }
            if (!runid || !offset || (offset-runid-1) != REDIS_RUN_ID_SIZE) {
                redisLog(REDIS_WARNING,
                    "Master replied with wrong +FULLRESYNC syntax.");
                /* This is an unexpected condition, actually the +FULLRESYNC
                 * reply means that the master supports PSYNC, but the reply
                 * format seems wrong. To stay safe we blank the master
                 * runid to make sure next PSYNCs will fail. */
                // master是支持psync的，但是由于发送给master的runid & offset不正确，所以为了数据完整性还是进行全量同步为妥
                memset(server.repl_master_runid,0,REDIS_RUN_ID_SIZE+1);
            } else {
                // 记录全量同步其实参数runid & offset
                memcpy(server.repl_master_runid, runid, offset-runid-1); // offset 和 runid分别是reply字符串起始处的指针
                server.repl_master_runid[REDIS_RUN_ID_SIZE] = '\0';
                server.repl_master_initial_offset = strtoll(offset,NULL,10);
                redisLog(REDIS_NOTICE,"Full resync from master: %s:%lld",
                    server.repl_master_runid,
                    server.repl_master_initial_offset);
            }
            /* We are going to full resync, discard the cached master structure. */
            // 释放master与client之间的连接，开始进行全量同步
            replicationDiscardCachedMaster();
            sdsfree(reply);
            return PSYNC_FULLRESYNC;
        }

        // master答应增量同步
        if (!strncmp(reply,"+CONTINUE",9)) {
            /* Partial resync was accepted, set the replication state accordingly */
            redisLog(REDIS_NOTICE,
                "Successful partial resynchronization with master.");
            sdsfree(reply);
            replicationResurrectCachedMaster(fd);
            return PSYNC_CONTINUE;
        }

        /* If we reach this point we receied either an error since the master does
         * not understand PSYNC, or an unexpected reply from the master.
         * Return PSYNC_NOT_SUPPORTED to the caller in both cases. */
        // master不支持增量同步或者发送了其他错误
        if (strncmp(reply,"-ERR",4)) {
            /* If it's not an error, log the unexpected event. */
            redisLog(REDIS_WARNING,
                "Unexpected reply to PSYNC from master: %s", reply);
        } else {
            redisLog(REDIS_NOTICE,
                "Master does not support PSYNC or is in "
                "error state (reply: %s)", reply);
        }
        sdsfree(reply);
        replicationDiscardCachedMaster();
        return PSYNC_NOT_SUPPORTED;
    }

</font>

###2.5 全量同步 ###

<font color=blue>

>slave启动之后，刚开始进行的数据同步只能以全量的方式进行，尔后才有增量同步的可能。所以先分析全量同步的流程。
>
>全量同步函数流程：

- 1 确定数据长度和数据读取模式[精确 & 模糊];
- 2 读取数据；
- 3 write & sync至磁盘；
- 4 读取完毕，修改tmp file为正式文件名称，清空内存中的数据，把数据load到内存;
- 5 依据repl_transfer_s创建一个client：server.master，并修改状态为CONNECTED；
- 6 启动aof模式。

</font>

<font color=green>

	// 向disk同步数据的时候，如果是linux系统则采用其特有的函数rdb_fsync_range
    #ifdef HAVE_SYNC_FILE_RANGE
    #define rdb_fsync_range(fd,off,size) sync_file_range(fd,off,size,SYNC_FILE_RANGE_WAIT_BEFORE|SYNC_FILE_RANGE_WRITE)
    #else
    #define rdb_fsync_range(fd,off,size) fsync(fd)
    #endif
    
    /* Asynchronously read the SYNC payload we receive from a master */
    // 如果slave从master同步过来的数据超过8M，就要进行fsync
    #define REPL_MAX_WRITTEN_BEFORE_FSYNC (1024*1024*8) /* 8 MB */
    
    void readSyncBulkPayload(aeEventLoop *el, int fd, void *privdata, int mask) {
        char buf[4096];
        ssize_t nread, readlen;
        off_t left;
        REDIS_NOTUSED(el);
        REDIS_NOTUSED(privdata);
        REDIS_NOTUSED(mask);

        /* Static vars used to hold the EOF mark, and the last bytes received
         * form the server: when they match, we reached the end of the transfer. */
        // 如果无法获取到bulk的长度，则master会给出数据末尾的标志符集，存于eofmark
        static char eofmark[REDIS_RUN_ID_SIZE];
        static char lastbytes[REDIS_RUN_ID_SIZE];
        // 用于说明是否已经精确地获取到了数据的长度[1:否；0:是]
        // 注意：无法精确知道数据长度的模式可称之为模糊模式
        static int usemark = 0;

        // 获取数据块的长度
        /* If repl_transfer_size == -1 we still have to read the bulk length
         * from the master reply. */
        // repl_transfer_size值初始为-1，见函数syncWithMaster。
        // 如果repl_transfer_size为-1，说明刚开始读取master回复
        if (server.repl_transfer_size == -1) {
            // 以同步的方式读取数据，超时时间为REDIS_REPL_SYNCIO_TIMEOUT(5s)
            if (syncReadLine(fd,buf,1024,server.repl_syncio_timeout*1000) == -1) {
                redisLog(REDIS_WARNING,
                    "I/O error reading bulk count from MASTER: %s",
                    strerror(errno));
                goto error;
            }

            if (buf[0] == '-') {
                redisLog(REDIS_WARNING,
                    "MASTER aborted replication with an error: %s",
                    buf+1);
                goto error;
            } else if (buf[0] == '\0') {
                /* At this stage just a newline works as a PING in order to take
                 * the connection live. So we refresh our last interaction
                 * timestamp. */
                // 收到的内容为空，则master仅仅是为了连接有效
                server.repl_transfer_lastio = server.unixtime;
                return;
            } else if (buf[0] != '$') {
                redisLog(REDIS_WARNING,"Bad protocol from MASTER, the first byte is not '$' (we received '%s'), are you sure the host and port are right?", buf);
                goto error;
            }

            // 读取长度数值
            /* There are two possible forms for the bulk payload. One is the
             * usual $<count> bulk format. The other is used for diskless transfers
             * when the master does not know beforehand the size of the file to
             * transfer. In the latter case, the following format is used:
             *
             * $EOF:<40 bytes delimiter>
             *
             * At the end of the file the announced delimiter is transmitted. The
             * delimiter is long and random enough that the probability of a
             * collision with the actual file content can be ignored. */
            // 可能收到两种形式的回复。一种是$<count>，指明了数据长度。另一种则是
            // $EOF:<40 bytes>，这种情况是master没有启动磁盘存储，它无法计算要传输的Bulk的值
            if (strncmp(buf+1,"EOF:",4) == 0 && strlen(buf+5) >= REDIS_RUN_ID_SIZE) {
                usemark = 1;
                memcpy(eofmark,buf+5,REDIS_RUN_ID_SIZE);
                memset(lastbytes,0,REDIS_RUN_ID_SIZE);
                /* Set any repl_transfer_size to avoid entering this code path
                 * at the next call. */
                // 把值设为0，以避免在进入这个分支
                server.repl_transfer_size = 0;
                redisLog(REDIS_NOTICE,
                    "MASTER <-> SLAVE sync: receiving streamed RDB from master");
            } else {
                usemark = 0;
                server.repl_transfer_size = strtol(buf+1,NULL,10);
                redisLog(REDIS_NOTICE,
                    "MASTER <-> SLAVE sync: receiving %lld bytes from master",
                    (long long) server.repl_transfer_size);
            }
            return;
        }

        /* Read bulk data */
        if (usemark) {
            // 模糊模式下不知道到底该读多长，就以buf长度为限
            readlen = sizeof(buf);
        } else {
            left = server.repl_transfer_size - server.repl_transfer_read;
            // 判断left与readlen的关系，如果超过buf的长度就取buf长度为上限
            readlen = (left < (signed)sizeof(buf)) ? left : (signed)sizeof(buf);
        }

        nread = read(fd,buf,readlen);
        if (nread <= 0) {
            redisLog(REDIS_WARNING,"I/O error trying to sync with MASTER: %s",
                (nread == -1) ? strerror(errno) : "connection lost");
            replicationAbortSyncTransfer();
            return;
        }
        server.stat_net_input_bytes += nread;

        // 判断模糊模式下是否读取到了数据末尾
        /* When a mark is used, we want to detect EOF asap in order to avoid
         * writing the EOF mark into the file... */
        int eof_reached = 0;
        
        if (usemark) {
            /* Update the last bytes array, and check if it matches our delimiter.*/
            if (nread >= REDIS_RUN_ID_SIZE) {
                memcpy(lastbytes,buf+nread-REDIS_RUN_ID_SIZE,REDIS_RUN_ID_SIZE);
            } else {
                int rem = REDIS_RUN_ID_SIZE-nread;
                memmove(lastbytes,lastbytes+nread,rem);
                memcpy(lastbytes+rem,buf,nread);
            }
            if (memcmp(lastbytes,eofmark,REDIS_RUN_ID_SIZE) == 0) eof_reached = 1;
        }

        // 把收到的数据写到disk上
        server.repl_transfer_lastio = server.unixtime;
        if (write(server.repl_transfer_fd,buf,nread) != nread) {
            redisLog(REDIS_WARNING,"Write error or short write writing to the DB dump file needed for MASTER <-> SLAVE synchronization: %s", strerror(errno));
            goto error;
        }
        server.repl_transfer_read += nread;

        // 模糊模式下如果数据读取完毕，则删除最后的40B
        /* Delete the last 40 bytes from the file if we reached EOF. */
        if (usemark && eof_reached) {
            if (ftruncate(server.repl_transfer_fd,
                server.repl_transfer_read - REDIS_RUN_ID_SIZE) == -1)
            {
                redisLog(REDIS_WARNING,"Error truncating the RDB file received from the master for SYNC: %s", strerror(errno));
                goto error;
            }
        }

        // 把数据同步到磁盘，以免造成数据堆积
        /* Sync data on disk from time to time, otherwise at the end of the transfer
         * we may suffer a big delay as the memory buffers are copied into the
         * actual disk. */
        if (server.repl_transfer_read >=
            server.repl_transfer_last_fsync_off + REPL_MAX_WRITTEN_BEFORE_FSYNC)
        {
            off_t sync_size = server.repl_transfer_read -
                              server.repl_transfer_last_fsync_off;
            rdb_fsync_range(server.repl_transfer_fd,
                server.repl_transfer_last_fsync_off, sync_size);
            server.repl_transfer_last_fsync_off += sync_size;
        }

        /* Check if the transfer is now complete */
        if (!usemark) {
            if (server.repl_transfer_read == server.repl_transfer_size)
                eof_reached = 1;
        }

        if (eof_reached) {
            // 把临时文件rename为rdb文件
            if (rename(server.repl_transfer_tmpfile,server.rdb_filename) == -1) {
                redisLog(REDIS_WARNING,"Failed trying to rename the temp DB into dump.rdb in MASTER <-> SLAVE synchronization: %s", strerror(errno));
                replicationAbortSyncTransfer();
                return;
            }
            redisLog(REDIS_NOTICE, "MASTER <-> SLAVE sync: Flushing old data");
            signalFlushedDb(-1);
            // load到内存之前，先把内存数据清空
            emptyDb(replicationEmptyDbCallback);
            /* Before loading the DB into memory we need to delete the readable
             * handler, otherwise it will get called recursively since
             * rdbLoad() will call the event loop to process events from time to
             * time for non blocking loading. */
            // 在把数据load从磁盘load到内存之前，暂时不再从master读取数据
            aeDeleteFileEvent(server.el,server.repl_transfer_s,AE_READABLE);
            redisLog(REDIS_NOTICE, "MASTER <-> SLAVE sync: Loading DB in memory");
            if (rdbLoad(server.rdb_filename) != REDIS_OK) {
                redisLog(REDIS_WARNING,"Failed trying to load the MASTER synchronization DB from disk");
                replicationAbortSyncTransfer();
                return;
            }
            // 把replication状态修改为CONNECTED
            /* Final setup of the connected slave <- master link */
            zfree(server.repl_transfer_tmpfile);
            close(server.repl_transfer_fd);
            server.master = createClient(server.repl_transfer_s);
            server.master->flags |= REDIS_MASTER;
            server.master->authenticated = 1;
            server.repl_state = REDIS_REPL_CONNECTED;
            // 在slaveTryPartialResynchronization中可以获取下面两个值
            server.master->reploff = server.repl_master_initial_offset;
            memcpy(server.master->replrunid, server.repl_master_runid,
                sizeof(server.repl_master_runid));
            /* If master offset is set to -1, this master is old and is not
             * PSYNC capable, so we flag it accordingly. */
            if (server.master->reploff == -1)
                server.master->flags |= REDIS_PRE_PSYNC;
            redisLog(REDIS_NOTICE, "MASTER <-> SLAVE sync: Finished with success");
            /* Restart the AOF subsystem now that we finished the sync. This
             * will trigger an AOF rewrite, and when done will start appending
             * to the new file. */
            if (server.aof_state != REDIS_AOF_OFF) {
                int retry = 10;

                stopAppendOnly();
                while (retry-- && startAppendOnly() == REDIS_ERR) {
                    redisLog(REDIS_WARNING,"Failed enabling the AOF after successful master synchronization! Trying it again in one second.");
                    sleep(1);
                }
                if (!retry) {
                    redisLog(REDIS_WARNING,"FATAL: this slave instance finished the synchronization with its master, but the AOF can't be turned on. Exiting now.");
                    exit(1);
                }
            }
        }

        return;

    error:
        replicationAbortSyncTransfer();
        return;
    }

</font>

###2.6 增量同步 ###

<font color=green>

    void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask) {
        redisClient *c = (redisClient*) privdata;
        int nread, readlen;
        size_t qblen;
        REDIS_NOTUSED(el);
        REDIS_NOTUSED(mask);
    
        server.current_client = c;
        readlen = REDIS_IOBUF_LEN;
        /* If this is a multi bulk request, and we are processing a bulk reply
         * that is large enough, try to maximize the probability that the query
         * buffer contains exactly the SDS string representing the object, even
         * at the risk of requiring more read(2) calls. This way the function
         * processMultiBulkBuffer() can avoid copying buffers to create the
         * Redis Object representing the argument. */
        if (c->reqtype == REDIS_REQ_MULTIBULK && c->multibulklen && c->bulklen != -1
            && c->bulklen >= REDIS_MBULK_BIG_ARG)
        {
            int remaining = (unsigned)(c->bulklen+2)-sdslen(c->querybuf);
    
            if (remaining < readlen) readlen = remaining;
        }

        // 读取reply data    
        qblen = sdslen(c->querybuf);
        if (c->querybuf_peak < qblen) c->querybuf_peak = qblen;
        c->querybuf = sdsMakeRoomFor(c->querybuf, readlen);
        nread = read(fd, c->querybuf+qblen, readlen);
        if (nread == -1) {
            if (errno == EAGAIN) {
                nread = 0;
            } else {
                redisLog(REDIS_VERBOSE, "Reading from client: %s",strerror(errno));
                freeClient(c);
                return;
            }
        } else if (nread == 0) {
            redisLog(REDIS_VERBOSE, "Client closed connection");
            freeClient(c);
            return;
        }
        if (nread) {
            sdsIncrLen(c->querybuf,nread);
            c->lastinteraction = server.unixtime;
            if (c->flags & REDIS_MASTER) c->reploff += nread;
            server.stat_net_input_bytes += nread;
        } else {
            server.current_client = NULL;
            return;
        }
        if (sdslen(c->querybuf) > server.client_max_querybuf_len) {
            sds ci = catClientInfoString(sdsempty(),c), bytes = sdsempty();
    
            bytes = sdscatrepr(bytes,c->querybuf,64);
            redisLog(REDIS_WARNING,"Closing client that reached max query buffer length: %s (qbuf initial bytes: %s)", ci, bytes);
            sdsfree(ci);
            sdsfree(bytes);
            freeClient(c);
            return;
        }
        printf("readQueryFromClient call processInlineBuffer start\n");
        // 像处理client请求那样处理收到的数据
        processInputBuffer(c);
        printf("readQueryFromClient call processInlineBuffer over\n");
        server.current_client = NULL;
    }

</font>

###2.7 server.cached_master与server.master ###

<font color=blue>

>server.master代表slave与master之间的连接句柄，当这个连接超时后连接会被关闭，但是句柄这个连接所用到的内存资源会被赋值给server.cached_master。待需要重新与master建立连接的时候，server.master只需要从server.cached_master处获取到这个句柄就可以了。
>
>通过二者实现了slave与master之间连接句柄的循环利用。cached_master可以认为是一个“迷你型”的资源回收池。

</font>

###2.7.1 创建连接句柄 ###

<font color=blue>

> slave在与master进行连接并同步数据的过程中修改相关的状态，待全量同步完成，会调用createClient，并把状态修改为CONNECTED.
>
> 相关的代码可以到/** 2.5 全量同步 **/一节参考函数readSyncBulkPayload。

</font>

###2.7.2 关闭连接，释放句柄 ###

<font color=blue>

slave每次与master之间有通信时，server.master->lastinteraction都会被更新。

</font>

<font color=green>

    void replicationCron(void) {
        /* Timed out master when we are an already connected slave? */
        // 如果超过REDIS_REPL_TIMEOUT(60s)与master之间没有通信，则关闭与master之间的连接
        if (server.masterhost && server.repl_state == REDIS_REPL_CONNECTED &&
            (time(NULL)-server.master->lastinteraction) > server.repl_timeout)
        {
            redisLog(REDIS_WARNING,"MASTER timeout: no data nor PING received...");
            freeClient(server.master);
        }
    }

    void freeClient(redisClient *c) {
        /* If it is our master that's beging disconnected we should make sure
         * to cache the state to try a partial resynchronization later.
         *
         * Note that before doing this we make sure that the client is not in
         * some unexpected state, by checking its flags. */
        if (server.master && c->flags & REDIS_MASTER) {
            redisLog(REDIS_WARNING,"Connection with master lost.");
            if (!(c->flags & (REDIS_CLOSE_AFTER_REPLY|
                              REDIS_CLOSE_ASAP|
                              REDIS_BLOCKED|
                              REDIS_UNBLOCKED)))
            {
                replicationCacheMaster(c);
                return;
            }
        }
    }
    
    /*
     * 这个函数会被freeClient调用，以把与master之间的连接缓存起来。
     *
     * 另外两个函数会分别在不同的状况下处理cached_master:
     *
     * 1 如果以后不会再用与master之间的连接，replicationDiscardCachedMaster()
     * 会释放掉这个链接；
     * 2 如果发出了PSYNC命令，replicationResurrectCachedMaster()则会重新激活cached_master
     */
    void replicationCacheMaster(redisClient *c) {
        listNode *ln;

        redisAssert(server.master != NULL && server.cached_master == NULL);
        redisLog(REDIS_NOTICE,"Caching the disconnected master state.");

        /* Remove from the list of clients, we don't want this client to be
         * listed by CLIENT LIST or processed in any way by batch operations. */
        // 把连接从server.clients这个list中删除掉，以免客户端用CLIENT LIST命令获取到这个连接
        ln = listSearchKey(server.clients,c);
        redisAssert(ln != NULL);
        listDelNode(server.clients,ln);

        /* Save the master. Server.master will be set to null later by
         * replicationHandleMasterDisconnection(). */
        // 放入缓存池，在函数replicationHandleMasterDisconnection()里master会被置为nil
        server.cached_master = server.master;
    
        /* Remove the event handlers and close the socket. We'll later reuse
         * the socket of the new connection with the master during PSYNC. */
        // 删除掉与连接相关的读写事件，并close掉连接
        aeDeleteFileEvent(server.el,c->fd,AE_READABLE);
        aeDeleteFileEvent(server.el,c->fd,AE_WRITABLE);
        close(c->fd);

        /* Set fd to -1 so that we can safely call freeClient(c) later. */
        c->fd = -1;

        /* Invalidate the Peer ID cache. */
        if (c->peerid) {
            sdsfree(c->peerid);
            c->peerid = NULL;
        }

        /* Caching the master happens instead of the actual freeClient() call,
         * so make sure to adjust the replication state. This function will
         * also set server.master to NULL. */
        // 把master置为nil，并置state为REDIS_REPL_CONNECT
        replicationHandleMasterDisconnection();
    }
    
    /* This function is called when the slave lose the connection with the
     * master into an unexpected way. */
    void replicationHandleMasterDisconnection(void) {
        server.master = NULL;
        server.repl_state = REDIS_REPL_CONNECT;
        server.repl_down_since = server.unixtime;
        /* We lost connection with our master, force our slaves to resync
         * with us as well to load the new data set.
         *
         * If server.masterhost is NULL the user called SLAVEOF NO ONE so
         * slave resync is not needed. */
        if (server.masterhost != NULL) disconnectSlaves();
    }
        
    /* Close all the slaves connections. This is useful in chained replication
     * when we resync with our own master and want to force all our slaves to
     * resync with us as well. */
    void disconnectSlaves(void) {
        while (listLength(server.slaves)) {
            listNode *ln = listFirst(server.slaves);
            freeClient((redisClient*)ln->value);
        }
    }

</font>

###2.7.3 增量同步激活server.cached_master ###

<font color=blue>

当slave与master之间进行增量同步的时候，会激活server.cached_master。具体流程请参考 /** 2.4.2 增量同步尝试 **/ 一节的函数replicationResurrectCachedMaster()。

</font>

##3 master流程##

<font color=blue>



</font>

<font color=green>
	
	// 向slave发送基于数据库@dictid的数据集argv[0 ~ argv-1]
	void replicationFeedSlaves(list *slaves, int dictid, robj **argv, int argc) {
	    listNode *ln;
	    listIter li;
	    int j, len;
	    char llstr[REDIS_LONGSTR_SIZE];
	
	    /* If there aren't slaves, and there is no backlog buffer to populate,
	     * we can return ASAP. */
	    if (server.repl_backlog == NULL && listLength(slaves) == 0) return;
	
	    /* We can't have slaves attached and no backlog. */
	    redisAssert(!(listLength(slaves) != 0 && server.repl_backlog == NULL));
	
	    /* Send SELECT command to every slave if needed. */
		// 向每个slave发送select命令
	    if (server.slaveseldb != dictid) {
	        robj *selectcmd;
	
	        /* For a few DBs we have pre-computed SELECT command. */
	        if (dictid >= 0 && dictid < REDIS_SHARED_SELECT_CMDS) {
				// 如果dictid在[0, 19)之间，则直接使用预先生成的select命令字符集
	            selectcmd = shared.select[dictid];
	        } else {
				// 否则就动态生成
	            int dictid_len;
	
	            dictid_len = ll2string(llstr,sizeof(llstr),dictid);
	            selectcmd = createObject(REDIS_STRING,
	                sdscatprintf(sdsempty(),
	                "*2\r\n$6\r\nSELECT\r\n$%d\r\n%s\r\n",
	                dictid_len, llstr));
	        }
	
	        /* Add the SELECT command into the backlog. */
			// 把select命令放入backlog之中
	        if (server.repl_backlog) feedReplicationBacklogWithObject(selectcmd);
	
	        /* Send it to slaves. */
			// 遍历slave列表，分别向每个slave发送select dbid命令
	        listRewind(slaves,&li);
	        while((ln = listNext(&li))) {
	            redisClient *slave = ln->value;
	            addReply(slave,selectcmd);
	        }
	
	        if (dictid < 0 || dictid >= REDIS_SHARED_SELECT_CMDS)
				// 如果dictid不在[0, 10)内，则释放刚刚生成的字符串
	            decrRefCount(selectcmd);
	    }
	    server.slaveseldb = dictid;
	
	    /* Write the command to the replication backlog if any. */
		// 不管数据的内容，只要server的backlog存在，就把数据放进去
	    if (server.repl_backlog) {
	        char aux[REDIS_LONGSTR_SIZE+3]; // REDIS_LONGSTR_SIZE 21
	
	        /* Add the multi bulk reply length. */
	        aux[0] = '*';
	        len = ll2string(aux+1,sizeof(aux)-1,argc);
	        aux[len+1] = '\r';
	        aux[len+2] = '\n';
	        feedReplicationBacklog(aux,len+3);
	
	        for (j = 0; j < argc; j++) {
	            long objlen = stringObjectLen(argv[j]);
	
	            /* We need to feed the buffer with the object as a bulk reply
	             * not just as a plain string, so create the $..CRLF payload len
	             * and add the final CRLF */
	            aux[0] = '$';
	            len = ll2string(aux+1,sizeof(aux)-1,objlen);
	            aux[len+1] = '\r';
	            aux[len+2] = '\n';
	            feedReplicationBacklog(aux,len+3);
	            feedReplicationBacklogWithObject(argv[j]);
	            feedReplicationBacklog(aux+len+1,2);
	        }
	    }
	
	    /* Write the command to every slave. */
		// 遍历每个slave，把数据同步给slave
	    listRewind(server.slaves,&li);
	    while((ln = listNext(&li))) {
	        redisClient *slave = ln->value;
	
	        /* Don't feed slaves that are still waiting for BGSAVE to start */
			// 如果slave仍在等待BGSAVE，则跳过
	        if (slave->replstate == REDIS_REPL_WAIT_BGSAVE_START) continue;
	
	        /* Feed slaves that are waiting for the initial SYNC (so these commands
	         * are queued in the output buffer until the initial SYNC completes),
	         * or are already in sync with the master. */
	
	        /* Add the multi bulk length. */
			// 先把数据长度发送过去
	        addReplyMultiBulkLen(slave,argc);
	
	        /* Finally any additional argument that was not stored inside the
	         * static buffer if any (from j to argc). */
			// 发送数据内容
	        for (j = 0; j < argc; j++)
	            addReplyBulk(slave,argv[j]);
	    }
	}

	/* Replication cron function, called 1 time per second. */
	void replicationCron(void) {
	    /* If we have attached slaves, PING them from time to time.
	     * So slaves can implement an explicit timeout to masters, and will
	     * be able to detect a link disconnection even if the TCP connection
	     * will not actually go down. */
		// 每100个server周期(每个周期是100ms)向master的每个slave发送PING命令，
		// 通过这种心跳方式，slave可以探知master是否还活着
	    if (!(server.cronloops % (server.repl_ping_slave_period * server.hz))) {
	        listIter li;
	        listNode *ln;
	        robj *ping_argv[1];
	
	        /* First, send PING */
	        ping_argv[0] = createStringObject("PING",4);
			// 向每个slave发送PING命令
	        replicationFeedSlaves(server.slaves, server.slaveseldb, ping_argv, 1);
	        decrRefCount(ping_argv[0]);
	
	        /* Second, send a newline to all the slaves in pre-synchronization
	         * stage, that is, slaves waiting for the master to create the RDB file.
	         * The newline will be ignored by the slave but will refresh the
	         * last-io timer preventing a timeout. */
	        listRewind(server.slaves,&li);
	        while((ln = listNext(&li))) {
	            redisClient *slave = ln->value;
	
	            if (slave->replstate == REDIS_REPL_WAIT_BGSAVE_START ||
	                (slave->replstate == REDIS_REPL_WAIT_BGSAVE_END &&
	                 server.rdb_child_type != REDIS_RDB_CHILD_TYPE_SOCKET))
	            {
	                if (write(slave->fd, "\n", 1) == -1) {
	                    /* Don't worry, it's just a ping. */
	                }
	            }
	        }
	    }
	
	    /* Disconnect timedout slaves. */
	    if (listLength(server.slaves)) {
	        listIter li;
	        listNode *ln;
	
	        listRewind(server.slaves,&li);
	        while((ln = listNext(&li))) {
	            redisClient *slave = ln->value;
	
	            if (slave->replstate != REDIS_REPL_ONLINE) continue;
	            if (slave->flags & REDIS_PRE_PSYNC) continue;
	            if ((server.unixtime - slave->repl_ack_time) > server.repl_timeout)
	            {
	                redisLog(REDIS_WARNING, "Disconnecting timedout slave: %s",
	                    replicationGetSlaveName(slave));
	                freeClient(slave);
	            }
	        }
	    }
	
	    /* If we have no attached slaves and there is a replication backlog
	     * using memory, free it after some (configured) time. */
	    if (listLength(server.slaves) == 0 && server.repl_backlog_time_limit &&
	        server.repl_backlog)
	    {
	        time_t idle = server.unixtime - server.repl_no_slaves_since;
	
	        if (idle > server.repl_backlog_time_limit) {
	            freeReplicationBacklog();
	            redisLog(REDIS_NOTICE,
	                "Replication backlog freed after %d seconds "
	                "without connected slaves.",
	                (int) server.repl_backlog_time_limit);
	        }
	    }
	
	    /* If AOF is disabled and we no longer have attached slaves, we can
	     * free our Replication Script Cache as there is no need to propagate
	     * EVALSHA at all. */
	    if (listLength(server.slaves) == 0 &&
	        server.aof_state == REDIS_AOF_OFF &&
	        listLength(server.repl_scriptcache_fifo) != 0)
	    {
	        replicationScriptCacheFlush();
	    }
	
	    /* If we are using diskless replication and there are slaves waiting
	     * in WAIT_BGSAVE_START state, check if enough seconds elapsed and
	     * start a BGSAVE.
	     *
	     * This code is also useful to trigger a BGSAVE if the diskless
	     * replication was turned off with CONFIG SET, while there were already
	     * slaves in WAIT_BGSAVE_START state. */
	    if (server.rdb_child_pid == -1 && server.aof_child_pid == -1) {
	        time_t idle, max_idle = 0;
	        int slaves_waiting = 0;
	        listNode *ln;
	        listIter li;
	
	        listRewind(server.slaves,&li);
	        while((ln = listNext(&li))) {
	            redisClient *slave = ln->value;
	            if (slave->replstate == REDIS_REPL_WAIT_BGSAVE_START) {
	                idle = server.unixtime - slave->lastinteraction;
	                if (idle > max_idle) max_idle = idle;
	                slaves_waiting++;
	            }
	        }
	
	        if (slaves_waiting && max_idle > server.repl_diskless_sync_delay) {
	            /* Start a BGSAVE. Usually with socket target, or with disk target
	             * if there was a recent socket -> disk config change. */
	            if (startBgsaveForReplication() == REDIS_OK) {
	                /* It started! We need to change the state of slaves
	                 * from WAIT_BGSAVE_START to WAIT_BGSAVE_END in case
	                 * the current target is disk. Otherwise it was already done
	                 * by rdbSaveToSlavesSockets() which is called by
	                 * startBgsaveForReplication(). */
	                listRewind(server.slaves,&li);
	                while((ln = listNext(&li))) {
	                    redisClient *slave = ln->value;
	                    if (slave->replstate == REDIS_REPL_WAIT_BGSAVE_START)
	                        slave->replstate = REDIS_REPL_WAIT_BGSAVE_END;
	                }
	            }
	        }
	    }
	
	    /* Refresh the number of slaves with lag <= min-slaves-max-lag. */
	    refreshGoodSlavesCount();
	}

</font>

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

![](./pic/redis_replication.png)

###4.3 replication过程中slave与master的状态变化图###

![](./pic/redis_replication_interactive.png)


## 参考文档：##

- 1 redis/src/replication.c
- 2 http://redis.readthedocs.org/en/latest/topic/replication.html
- 3 http://www.360doc.com/content/11/1205/14/7936054_169834858.shtml

