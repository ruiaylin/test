# redis sentinel 源码分析
---

##0 主要数据结构

<font color=blue>
 >此处首先是sentinel实例，其地位就相当于redis-server模式下的server对象。sentinel实例比较重要的成员就是masters，里面存储了所有的redis主从对的主sri，其key是主sri的name，value就是主sri。
</font>

    struct sentinelState {
        uint64_t current_epoch;   // 当前处在第几个世纪（每次fail over，current_epoch+1）
        dict *masters;            // master实例字典（一个sentinle可监控多个master），key是sri的name，value是sri
        int tilt;                 // 是否在TITL模式中，后面详细介绍TITL模式
        int running_scripts;      // 当前正在执行的脚本
        mstime_t tilt_start_time; // TITL模式开始的时间 */
        mstime_t previous_time;   // 上次执行sentinel周期性执行任务的时间，用以判断是否进入TITL模式
        list *scripts_queue;      // 待执行脚本队列
        char *announce_ip;        // 执行gossip协议的时候标识sentinel的ip
        int announce_port;        // 执行gossip协议的时候标识sentinel的port
    } sentinel;

<font color=blue>
 其次是是sentinel实例，比较重要的成员就是sentinels和slaves，slaves里面存储了所有的redis主从对的从sri，而sentinels则存储了其他sentinel实例的sri，这两个dict的key都是sri的name，name形式是host：port。
</font>

<font color=green>

    typedef struct sentinelRedisInstance {
        int flags;                 // sentinel的身份，查看SRI_... 系列的定义
        char *name;                // sri的名称
        char *runid;               // sri的实例
        uint64_t config_epoch;     // Configuration epoch.
        sentinelAddr *addr;        // Master host.
        redisAsyncContext *cc;     // Hiredis context for commands
        redisAsyncContext *pc;     // Hiredis context for Pub / Sub
        int pending_commands;      // 等待回复的命令的个数
        mstime_t cc_conn_time;     // cc connection time.
        mstime_t pc_conn_time;     // pc connection time.
        mstime_t pc_last_activity; // Last time we received any message.
        mstime_t last_avail_time;  // Last time the instance replied to ping with a reply we consider valid.
        mstime_t last_ping_time;  /* Last time a pending ping was sent in the
                                  context of the current command connection
                                  with the instance. 0 if still not sent or
                                  if pong already received. */
        mstime_t last_pong_time;  /* Last time the instance replied to ping,
                                  whatever the reply was. That's used to check
                                  if the link is idle and must be reconnected. */
        mstime_t last_pub_time;   /* Last time we sent hello via Pub/Sub. */
        mstime_t last_hello_time; /* Only used if SRI_SENTINEL is set. Last time
                                  we received a hello from this Sentinel
                                  via Pub/Sub. */
        mstime_t last_master_down_reply_time; /* Time of last reply to
                                              SENTINEL is-master-down command. */
        mstime_t s_down_since_time; /* Subjectively down since time. */
        mstime_t o_down_since_time; /* Objectively down since time. */
        mstime_t down_after_period; /* Consider it down after that period. */
        mstime_t info_refresh;  /* Time at which we received INFO output from it. */

        /* Role and the first time we observed it.
        * This is useful in order to delay replacing what the instance reports
        * with our own configuration. We need to always wait some time in order
        * to give a chance to the leader to report the new configuration before
        * we do silly things. */
        int role_reported;
        mstime_t role_reported_time;
        mstime_t slave_conf_change_time; /* Last time slave master addr changed. */

        /* Master specific. */
        dict *sentinels;                 // 监控同一个ri master的其他sentinel实例的哈希表
        dict *slaves;                    // ri master 的slave 哈希表
        unsigned int quorum;             // 同意ri master由sdown进入odown状态的最少票数
        int parallel_syncs;              // 同时可以进行多少个failover
        char *auth_pass;                 // auth的password

        /* Slave specific. */
        mstime_t master_link_down_time;  // Slave replication link down time. */
        int slave_priority; /* Slave priority according to its INFO output. */
        mstime_t slave_reconf_sent_time; /* Time at which we sent SLAVE OF <new> */
        struct sentinelRedisInstance *master; // slave sri的master sri
        char *slave_master_host;              // slave ri的host[从info命令获取到]
        int slave_master_port;                // slave ri的port[从info命令获取到]
        int slave_master_link_status;         // slave ri的连接状态
        unsigned long long slave_repl_offset; // slave ri的replication offset

        /* Failover */
        char *leader;       /* If this is a master instance, this is the runid of
                            the Sentinel that should perform the failover. If
                            this is a Sentinel, this is the runid of the Sentinel
                            that this Sentinel voted as leader. */
        uint64_t leader_epoch; /* Epoch of the 'leader' field. */
        uint64_t failover_epoch; /* Epoch of the currently started failover. */
        int failover_state; /* See SENTINEL_FAILOVER_STATE_* defines. */
        mstime_t failover_state_change_time;
        mstime_t failover_start_time;   /* Last failover attempt start time. */
        mstime_t failover_timeout;      /* Max time to refresh failover state. */
        mstime_t failover_delay_logged; /* For what failover_start_time value we
                                        logged the failover delay. */
        struct sentinelRedisInstance *promoted_slave; /* Promoted slave instance. */
        /* Scripts executed to notify admin or reconfigure clients: when they
        * are set to NULL no script is executed. */
        char *notification_script;
        char *client_reconfig_script;
    } sentinelRedisInstance;`

</font>


##1 main

<font color=blue>
 sentinel启动流程
</font>

<font color=green>

    int main(int argc, char **argv) {
        // sentinel模式判断
        server.sentinel_mode = checkForSentinelMode(argc,argv);

        // sentinel模式下需要完成的初始化工作
        initServerConfig();
        if (server.sentinel_mode) {
            initSentinelConfig();
            initSentinel();
        }

        // 加载配置
        if (argc >= 2) {
            loadServerConfig(configfile,options);
            sdsfree(options);
        }
        // 启动server，加好油门sentinelTimer
        initServer();

        // 检查setinel是否做好了准备，如configfile可写等
        if (!server.sentinel_mode) {
        } else {
            sentinelIsRunning();
        }

        // 开足马力，运转发动机
        aeSetBeforeSleepProc(server.el,beforeSleep);
        aeMain(server.el);
        aeDeleteEventLoop(server.el);

        return 0;
    }

</font>

##2 检查程序是否进入sentinel模式以及sentinel模式下需要完成的初始化工作

<font color=green>

    // 创建一个server id
    int checkForSentinelMode(int argc, char **argv) {
        int j;

        if (strstr(argv[0],"redis-sentinel") != NULL) return 1;
        for (j = 1; j < argc; j++)
            if (!strcmp(argv[j],"--sentinel")) return 1;
        return 0;
    }

</font>

<font color=blue>
        
 >Linux中的随机数可以从两个特殊的文件中产生，一个是/dev/urandom.另外一个是/dev/random。他们产生随机数的原理是利用当前系统的熵池来计算出固定一定数量的随机比特，然后将这些比特作为字节流返回。
 >
 >熵池就是当前系统的环境噪音，熵指的是一个系统的混乱程度，系统噪音可以通过很多参数来评估，如内存的使用，文件的使用量，不同类型的进程数量等等。如果当前环境噪音变化的不是很剧烈或者当前环境噪音很小，比如刚开机的时候，而当前需要大量的随机比特，这时产生的随机数的随机效果就不是很好了。这就是为什么会有/dev/urandom和/dev/random这两种不同的文件，后者在不能产生新的随机数时会阻塞程序，而前者不会（ublock），当然产生的随机数效果就不太好了，这对加密解密这样的应用来说就不是一种很好的选择。
 >
 >/dev/random会阻塞当前的程序，直到根据熵池产生新的随机字节之后才返回，所以使用/dev/random比使用/dev/urandom产生大量随机数的速度要慢。
 >
</font>

<font color=green>

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
        /*  生成一个随机种子数，存储到seed里面
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
    // 给server分配一个id
    void initServerConfig(void) {
        // ./redis.h:95:#define REDIS_RUN_ID_SIZE 40
        getRandomHexChars(server.runid,REDIS_RUN_ID_SIZE);
    }
    // 初始化server.port
    void initSentinelConfig(void) {
        // #define REDIS_SENTINEL_PORT 26379
        server.port = REDIS_SENTINEL_PORT;
    }
    // 初始化sentinel对象
    void initSentinel(void) {
        unsigned int j;

        /* Remove usual Redis commands from the command table, then just add
         * the SENTINEL command. */
        // 初始化sentinel的命令字典
        dictEmpty(server.commands,NULL);
        for (j = 0; j < sizeof(sentinelcmds)/sizeof(sentinelcmds[0]); j++) {
            int retval;
            struct redisCommand *cmd = sentinelcmds+j;

            retval = dictAdd(server.commands, sdsnew(cmd->name), cmd);
            redisAssert(retval == DICT_OK);
        }

        /* Initialize various data structures. */
        sentinel.current_epoch = 0;
        // 初始化sentinel的master字典
        sentinel.masters = dictCreate(&instancesDictType,NULL);
        sentinel.tilt = 0;
        sentinel.tilt_start_time = 0;
        sentinel.previous_time = mstime();
        sentinel.running_scripts = 0;
        sentinel.scripts_queue = listCreate();
        sentinel.announce_ip = NULL;
        sentinel.announce_port = 0;
    }

</font>

##3 初始化配置

<font color=green>

    // 从文件读取所有的字符流
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
    // 对字符流@config逐行拆分
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
            if (!strcasecmp(argv[0],"sentinel")) {
                /* argc == 1 is handled by main() as we need to enter the sentinel
                 * mode ASAP. */
                if (argc != 1) {
                    if (!server.sentinel_mode) {
                        err = "sentinel directive while not in sentinel mode";
                        goto loaderr;
                    }
                    err = sentinelHandleConfiguration(argv+1,argc-1);
                    if (err) goto loaderr;
                }
            } else {
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

    typedef struct sentinelAddr {
        char *ip;
        int port;
    } sentinelAddr;
    // 创建sentinel的地址
    sentinelAddr *createSentinelAddr(char *hostname, int port) {
        char ip[REDIS_IP_STR_LEN];
        sentinelAddr *sa;

        if (port <= 0 || port > 65535) {
            errno = EINVAL;
            return NULL;
        }
        if (anetResolve(NULL,hostname,ip,sizeof(ip)) == ANET_ERR) {
            errno = ENOENT;
            return NULL;
        }
        sa = zmalloc(sizeof(*sa));
        sa->ip = sdsnew(ip);
        sa->port = port;
        return sa;
    }

</font>

<font color=blue>

    // 创建一个sri
    /*
     * 这个函数用于创建一个sentinel instance，用于代表一个sentinel的监控或者联系对象，
     * 联系对象可以是一个redis master\redis slave\redis sentinel，下面两个参数在接到info命令时再赋值:
     * runid: 初始化的时候被赋值为nil；
     * info_refresh: 初始化的时候值为0，表示还没有接到过info命令；
     *
     * 如果flags值为SRI_MASTER，则sri被创建后，将被放在sentinel的sentinel.masters哈希表；
     * 如果flags值为SRI_SLAVE or SRI_SENTINEL，则@name无用，它将用hostname:port作为自己的name,
     *        同时@master一定不能为nil，创建的sri将被放入master->slaves or master->sentinels哈希表;
     *
     * 如果hostname不能被解析或者port溢出，则返回nil并且errno会被置为相关值；
     * 如果master和某个slave的name一样，则返回nil且errno为EBUSY。因为相关的hash表以name作为hash key。
     */
</font>

<font color=green>

    sentinelRedisInstance *createSentinelRedisInstance(char *name, int flags, char *hostname, int port, int quorum, sentinelRedisInstance *master) {
        sentinelRedisInstance *ri;
        sentinelAddr *addr;
        dict *table = NULL;
        char slavename[128], *sdsname;

        redisAssert(flags & (SRI_MASTER | SRI_SLAVE | SRI_SENTINEL));
        redisAssert((flags & SRI_MASTER) || master != NULL);

        /* Check address validity. */
        addr = createSentinelAddr(hostname, port);
        if (addr == NULL) return NULL;

        /* For slaves and sentinel we use ip:port as name. */
        if (flags & (SRI_SLAVE | SRI_SENTINEL)) {
            snprintf(slavename, sizeof(slavename),
                strchr(hostname, ':') ? "[%s]:%d" : "%s:%d",
                hostname, port);
            name = slavename;
        }

        /* Make sure the entry is not duplicated. This may happen when the same
        * name for a master is used multiple times inside the configuration or
        * if we try to add multiple times a slave or sentinel with same ip/port
        * to a master. */
        // sentinel监控的所有的主都在sentinel{masters}里面，而每个master的slave
        // 以及相关的sentinel则在sentinelRedisInstance{slaves, sentinels}里面
        if (flags & SRI_MASTER) table = sentinel.masters;
        else if (flags & SRI_SLAVE) table = master->slaves;
        else if (flags & SRI_SENTINEL) table = master->sentinels;
        sdsname = sdsnew(name);
        if (dictFind(table, sdsname)) {
            releaseSentinelAddr(addr);
            sdsfree(sdsname);
            errno = EBUSY;
            return NULL;
        }

        /* Create the instance object. */
        ri = zmalloc(sizeof(*ri));
        /* Note that all the instances are started in the disconnected state,
        * the event loop will take care of connecting them. */
        ri->flags = flags | SRI_DISCONNECTED;
        ri->name = sdsname;
        ri->runid = NULL;
        ri->config_epoch = 0;
        ri->addr = addr;
        ri->cc = NULL;
        ri->pc = NULL;
        ri->pending_commands = 0;
        ri->cc_conn_time = 0;
        ri->pc_conn_time = 0;
        ri->pc_last_activity = 0;
        /* We set the last_ping_time to "now" even if we actually don't have yet
        * a connection with the node, nor we sent a ping.
        * This is useful to detect a timeout in case we'll not be able to connect
        * with the node at all. */
        ri->last_ping_time = mstime();
        ri->last_avail_time = mstime();
        ri->last_pong_time = mstime();
        ri->last_pub_time = mstime();
        ri->last_hello_time = mstime();
        ri->last_master_down_reply_time = mstime();
        ri->s_down_since_time = 0;
        ri->o_down_since_time = 0;
        ri->down_after_period = master ? master->down_after_period :
            SENTINEL_DEFAULT_DOWN_AFTER;
        ri->master_link_down_time = 0;
        ri->auth_pass = NULL;
        ri->slave_priority = SENTINEL_DEFAULT_SLAVE_PRIORITY;
        ri->slave_reconf_sent_time = 0;
        ri->slave_master_host = NULL;
        ri->slave_master_port = 0;
        ri->slave_master_link_status = SENTINEL_MASTER_LINK_STATUS_DOWN;
        ri->slave_repl_offset = 0;
        ri->sentinels = dictCreate(&instancesDictType, NULL);
        ri->quorum = quorum;
        ri->parallel_syncs = SENTINEL_DEFAULT_PARALLEL_SYNCS;
        ri->master = master;
        ri->slaves = dictCreate(&instancesDictType, NULL);
        ri->info_refresh = 0;

        /* Failover state. */
        ri->leader = NULL;
        ri->leader_epoch = 0;
        ri->failover_epoch = 0;
        ri->failover_state = SENTINEL_FAILOVER_STATE_NONE;
        ri->failover_state_change_time = 0;
        ri->failover_start_time = 0;
        ri->failover_timeout = SENTINEL_DEFAULT_FAILOVER_TIMEOUT;
        ri->failover_delay_logged = 0;
        ri->promoted_slave = NULL;
        ri->notification_script = NULL;
        ri->client_reconfig_script = NULL;

        /* Role */
        ri->role_reported = ri->flags & (SRI_MASTER | SRI_SLAVE);
        ri->role_reported_time = mstime();
        ri->slave_conf_change_time = mstime();

        /* Add into the right table. */
        dictAdd(table, ri->name, ri);
        return ri;
    }
    // 分析config的每一行，创建sri
    char *sentinelHandleConfiguration(char **argv, int argc) {
        sentinelRedisInstance *ri;

        if (!strcasecmp(argv[0],"monitor") && argc == 5) {
            /* monitor <name> <host> <port> <quorum> */
            int quorum = atoi(argv[4]);

            if (quorum <= 0) return "Quorum must be 1 or greater.";
            // 创建monitor sri
            if (createSentinelRedisInstance(argv[1],SRI_MASTER,argv[2],
                                            atoi(argv[3]),quorum,NULL) == NULL)
            {
                switch(errno) {
                case EBUSY: return "Duplicated master name.";
                case ENOENT: return "Can't resolve master instance hostname.";
                case EINVAL: return "Invalid port number";
                }
            }
        } else if (!strcasecmp(argv[0],"down-after-milliseconds") && argc == 3) {
            /* down-after-milliseconds <name> <milliseconds> */
            // 根据master的name获取sri，然后给其参数down_after_period赋值
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            ri->down_after_period = atoi(argv[2]);
            if (ri->down_after_period <= 0)
                return "negative or zero time parameter.";
            sentinelPropagateDownAfterPeriod(ri);
        } else if (!strcasecmp(argv[0],"failover-timeout") && argc == 3) {
            /* failover-timeout <name> <milliseconds> */
            // 根据master的name获取sri，然后给其参数failover-timeout赋值
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            ri->failover_timeout = atoi(argv[2]);
            if (ri->failover_timeout <= 0)
                return "negative or zero time parameter.";
       } else if (!strcasecmp(argv[0],"parallel-syncs") && argc == 3) {
            /* parallel-syncs <name> <milliseconds> */
            // 根据master的name获取sri，然后给其参数parallel_syncs赋值
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            ri->parallel_syncs = atoi(argv[2]);
       } else if (!strcasecmp(argv[0],"notification-script") && argc == 3) {
            /* notification-script <name> <path> */
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            if (access(argv[2],X_OK) == -1)
                return "Notification script seems non existing or non executable.";
            ri->notification_script = sdsnew(argv[2]);
       } else if (!strcasecmp(argv[0],"client-reconfig-script") && argc == 3) {
            /* client-reconfig-script <name> <path> */
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            if (access(argv[2],X_OK) == -1)
                return "Client reconfiguration script seems non existing or "
                       "non executable.";
            ri->client_reconfig_script = sdsnew(argv[2]);
       } else if (!strcasecmp(argv[0],"auth-pass") && argc == 3) {
            /* auth-pass <name> <password> */
            // 根据master的name获取sri，然后给其参数auth_pass赋值
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            ri->auth_pass = sdsnew(argv[2]);
        } else if (!strcasecmp(argv[0],"current-epoch") && argc == 2) {
            /* current-epoch <epoch> */
            unsigned long long current_epoch = strtoull(argv[1],NULL,10);
            if (current_epoch > sentinel.current_epoch)
                sentinel.current_epoch = current_epoch;
        } else if (!strcasecmp(argv[0],"config-epoch") && argc == 3) {
            /* config-epoch <name> <epoch> */
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            ri->config_epoch = strtoull(argv[2],NULL,10);
            /* The following update of current_epoch is not really useful as
             * now the current epoch is persisted on the config file, but
             * we leave this check here for redundancy. */
            if (ri->config_epoch > sentinel.current_epoch)
                sentinel.current_epoch = ri->config_epoch;
        } else if (!strcasecmp(argv[0],"leader-epoch") && argc == 3) {
            /* leader-epoch <name> <epoch> */
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            ri->leader_epoch = strtoull(argv[2],NULL,10);
        } else if (!strcasecmp(argv[0],"known-slave") && argc == 4) {
            sentinelRedisInstance *slave;

            /* known-slave <name> <ip> <port> */
            // 给redis slave创建sri
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            if ((slave = createSentinelRedisInstance(NULL,SRI_SLAVE,argv[2],
                        atoi(argv[3]), ri->quorum, ri)) == NULL)
            {
                return "Wrong hostname or port for slave.";
            }
        } else if (!strcasecmp(argv[0],"known-sentinel") &&
                   (argc == 4 || argc == 5)) {
            sentinelRedisInstance *si;

            /* known-sentinel <name> <ip> <port> [runid] */
            // 给sentinel创建sri
            ri = sentinelGetMasterByName(argv[1]);
            if (!ri) return "No such master with specified name.";
            if ((si = createSentinelRedisInstance(NULL,SRI_SENTINEL,argv[2],
                        atoi(argv[3]), ri->quorum, ri)) == NULL)
            {
                return "Wrong hostname or port for sentinel.";
            }
            if (argc == 5) si->runid = sdsnew(argv[4]);
        } else if (!strcasecmp(argv[0],"announce-ip") && argc == 2) {
            /* announce-ip <ip-address> */
            if (strlen(argv[1]))
                // 配置sentinel的announce_ip
                sentinel.announce_ip = sdsnew(argv[1]);
        } else if (!strcasecmp(argv[0],"announce-port") && argc == 2) {
            /* announce-port <port> */
            // 配置sentinel的announce_port
            sentinel.announce_port = atoi(argv[1]);
        } else {
            return "Unrecognized sentinel configuration statement.";
        }
        return NULL;
    }

</font>

##4 启动server，启动定时函数serverCron

<font color=green>

    /* Return the UNIX time in microseconds */
    long long ustime(void) {
        struct timeval tv;
        long long ust;

        gettimeofday(&tv, NULL);
        ust = ((long long)tv.tv_sec)*1000000;
        ust += tv.tv_usec;
        return ust;
    }

    /* Return the UNIX time in milliseconds */
    long long mstime(void) {
        return ustime()/1000;
    }

    /* We take a cached value of the unix time in the global state because with
     * virtual memory and aging there is to store the current time in objects at
     * every object access, and accuracy is not needed. To access a global var is
     * a lot faster than calling time(NULL) */
    void updateCachedTime(void) {
        server.unixtime = time(NULL);
        server.mstime = mstime();
    }

    /* Using the following macro you can run code inside serverCron() with the
     * specified period, specified in milliseconds.
     * The actual resolution depends on server.hz. */
    // 即一秒内执行10次，每次的interval是100ms
    #define REDIS_DEFAULT_HZ        10      /* Time interrupt calls/sec. */
    // 下面宏中，(1000/server.hz)为一个动作的time interval，由于serverCron的执行周期是100ms，所以有了宏的第一个判断条件
    // ((_ms_)/(1000/server.hz)) 则是每多少次执行一次相关的检查，等式等效为((_ms_ * server.hz) / (1000))
    // serverCron函数每执行一次，server.cronloops就增加一次，(server.cronloops%((_ms_)/(1000/server.hz))
    #define run_with_period(_ms_) if ((_ms_ <= 1000/server.hz) || !(server.cronloops%((_ms_)/(1000/server.hz))))

    /* 这个是redis的定时任务函数，每1ms会被执行一次，一秒内执行server.hz[10]次
     * 下面列出了一些定时任务列表：
     *
     * - 过期kv的收集删除 (lazy模式).
     * - Software watchdog.
     * - 更新统计值
     * - 对所有的db进行rehashing，进行负载均衡
     * - 触发BGSAVE / AOF rewrite, and handling of terminated children.
     * - 删除过期的连接
     * - Replication，主从复制
     * - 其他任务
     *
     * 由于所有的任务都是执行同样的检查频率，为了对检查频率进行控制，
     *  这里定义了一个起到油门作用的宏run_with_period(milliseconds)
     */
    int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData) {
        int j;
        REDIS_NOTUSED(eventLoop);
        REDIS_NOTUSED(id);
        REDIS_NOTUSED(clientData);

        /* Software watchdog: deliver the SIGALRM that will reach the signal
         * handler if we don't return here fast enough. */
        if (server.watchdog_period) watchdogScheduleSignal(server.watchdog_period);

        /* Update the time cache. */
        updateCachedTime();

        /* Run the Sentinel timer if we are in sentinel mode. */
        // 如果在sentinel模式下，则执行sentinel相关的周期性任务
        run_with_period(100) {
            if (server.sentinel_mode) sentinelTimer();
        }

        server.cronloops++;
        // hz默认值为10，但是sentinel的定时函数sentinelTimer会修改这个值，原因见流程分析6
        // 参考下面的processTimeEvents，此处返回1000/10 = 100ms会导致serverCron
        // 被定时执行时间由初始的1ms被修改为100ms，即函数的定时处理时长为100ms
        return 1000/server.hz;
    }

    // 创建event loop，监听各个端口，并启动定时函数serverCron执行定时任务，定时间隔是1ms
    void initServer(void) {
        int j;

        // 信号处理
        signal(SIGHUP, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
        setupSignalHandlers();

        if (server.syslog_enabled) {
            openlog(server.syslog_ident, LOG_PID | LOG_NDELAY | LOG_NOWAIT,
                server.syslog_facility);
        }

        // 初始化主要成员
        server.pid = getpid();
        server.monitors = listCreate();

        createSharedObjects();
        adjustOpenFilesLimit();
        // 创建event loop
        server.el = aeCreateEventLoop(server.maxclients+REDIS_EVENTLOOP_FDSET_INCR);
        // 创建root db
        server.db = zmalloc(sizeof(redisDb)*server.dbnum);

        // 监听端口
        if (server.port != 0 &&
            listenToPort(server.port,server.ipfd,&server.ipfd_count) == REDIS_ERR)
            exit(1);

        /* Abort if there are no listening sockets at all. */
        if (server.ipfd_count == 0 && server.sofd < 0) {
            redisLog(REDIS_WARNING, "Configured to not listen anywhere, exiting.");
            exit(1);
        }

        /* Create the Redis databases, and initialize other internal state. */
        // 创建db数组
        for (j = 0; j < server.dbnum; j++) {
            server.db[j].dict = dictCreate(&dbDictType,NULL);
        }
        server.pubsub_channels = dictCreate(&keylistDictType,NULL);
        server.pubsub_patterns = listCreate();
        server.dirty = 0;
        resetServerStats();
        /* A few stats we don't want to reset: server startup time, and peak mem. */
        server.stat_starttime = time(NULL);
        updateCachedTime();

        /* Create the serverCron() time event, that's our main way to process
         * background operations. */
        // 启动定时器
        if(aeCreateTimeEvent(server.el, 1, serverCron, NULL, NULL) == AE_ERR) {
            redisPanic("Can't create the serverCron time event.");
            exit(1);
        }

        /* Create an event handler for accepting new connections in TCP and Unix
         * domain sockets. */
        for (j = 0; j < server.ipfd_count; j++) {
            printf("initServer invoke acceptTcpHandler\n");
            if (aeCreateFileEvent(server.el, server.ipfd[j], AE_READABLE,
                acceptTcpHandler,NULL) == AE_ERR)
                {
                    redisPanic(
                        "Unrecoverable error creating server.ipfd file event.");
                }
        }
    }

</font>

<font color=red>
    !!!!注意：上面initServer中初始注册定时器函数serverCron的时候，用的时间间隔是1ms后调用这个函数，但是后面再次调用时，时间间隔是函数的返回值：
</font>

<font color=green>

	/* Process time events */
    static int processTimeEvents(aeEventLoop *eventLoop) {
        int processed = 0;
        aeTimeEvent *te;
        long long maxId;
        time_t now = time(NULL);

        te = eventLoop->timeEventHead;
        maxId = eventLoop->timeEventNextId-1;
        while(te) {
            long now_sec, now_ms;
            long long id;

            if (te->id > maxId) {
                te = te->next;
                continue;
            }
            aeGetTime(&now_sec, &now_ms);
            if (now_sec > te->when_sec ||
                (now_sec == te->when_sec && now_ms >= te->when_ms))
            {
                int retval;

                id = te->id;
                retval = te->timeProc(eventLoop, id, te->clientData);
                processed++;

                if (retval != AE_NOMORE) {  // 根据retval，修改定时任务的重新执行时间@te->when_sec&@te->when_ms
                    aeAddMillisecondsToNow(retval,&te->when_sec,&te->when_ms);
                } else {  // 如果返回值是-1，则删除定时任务
                    aeDeleteTimeEvent(eventLoop, id);
                }
                te = eventLoop->timeEventHead;
            } else {
                te = te->next;
            }
        }
        return processed;
    }

</font>

##5 检查监控条件

<font color=green>

    // 检查configfile是否存在以及是否可写
    void sentinelIsRunning(void) {
        redisLog(REDIS_WARNING,"Sentinel runid is %s", server.runid);

        if (server.configfile == NULL) {
            // 检查configfile是否存在
            // configfile用于序列化sentinel的监控内容
            redisLog(REDIS_WARNING,
                "Sentinel started without a config file. Exiting...");
            exit(1);
        } else if (access(server.configfile,W_OK) == -1) {
            // 检查configfile是否可写
            redisLog(REDIS_WARNING,
                "Sentinel config file %s is not writable: %s. Exiting...",
                server.configfile,strerror(errno));
            exit(1);
        }

        /* We want to generate a +monitor event for every configured master
         * at startup. */
        sentinelGenerateInitialMonitorEvents();
    }

    /* 记录event log，执行pub/sub, 执行notification脚本
     *
     * 'level'是log level。只有log level是REDIS_WARNING时，才会执行notification脚本
     * 'type'是message type, 也用作pub/sub channel name.
     * 'ri'是sri，可以获取到path of the notification script
     * 'fmt' printf-alike.
     *       如果fmt以"%@"开头并且ri不是NULL，则log内容的开头以如下格式展开：
     *       <instance type> <instance name> <ip> <port>
     *       如果@ri不是master，则后面要补充上mmaster的信息:
     *       @ <master name> <master ip> <master port>
     *       剩余部分的处理跟printf类似
     */
    void sentinelEvent(int level, char *type, sentinelRedisInstance *ri,
                       const char *fmt, ...) {
        va_list ap;
        char msg[REDIS_MAX_LOGMSG_LEN];  // log内容最长1024B
        robj *channel, *payload;

        /* Handle %@ */
        // 拼写msg pre
        if (fmt[0] == '%' && fmt[1] == '@') {
            sentinelRedisInstance *master = (ri->flags & SRI_MASTER) ?
                                             NULL : ri->master;

            if (master) {
                snprintf(msg, sizeof(msg), "%s %s %s %d @ %s %s %d",
                    sentinelRedisInstanceTypeStr(ri),
                    ri->name, ri->addr->ip, ri->addr->port,
                    master->name, master->addr->ip, master->addr->port);
            } else {
                snprintf(msg, sizeof(msg), "%s %s %s %d",
                    sentinelRedisInstanceTypeStr(ri),
                    ri->name, ri->addr->ip, ri->addr->port);
            }
            fmt += 2;
        } else {
            msg[0] = '\0';
        }

        /* Use vsprintf for the rest of the formatting if any. */
        // 拼凑msg内容
        if (fmt[0] != '\0') {
            va_start(ap, fmt);
            vsnprintf(msg+strlen(msg), sizeof(msg)-strlen(msg), fmt, ap);
            va_end(ap);
        }

        /* Log the message if the log level allows it to be logged. */
        // 记录log
        if (level >= server.verbosity)
            redisLog(level,"%s %s",type,msg);

        /* Publish the message via Pub/Sub if it's not a debugging one. */
        // 执行pub/sub，例如发生了failover时候向hello channel发送通知
        if (level != REDIS_DEBUG) {
            channel = createStringObject(type,strlen(type));  // type是channel
            payload = createStringObject(msg,strlen(msg));    // msg内容
            pubsubPublishMessage(channel,payload); // pubsub
            decrRefCount(channel);
            decrRefCount(payload);
        }

        /* Call the notification script if applicable. */
        // 执行notify script，例如发生了failover时候可以执行一些script
        if (level == REDIS_WARNING && ri != NULL) {
            sentinelRedisInstance *master = (ri->flags & SRI_MASTER) ?
                                             ri : ri->master;
            if (master->notification_script) {
                sentinelScheduleScriptExecution(master->notification_script,
                    type,msg,NULL); // notify script
            }
        }
    }

    /* This function is called only at startup and is used to generate a
     * +monitor event for every configured master. The same events are also
     * generated when a master to monitor is added at runtime via the
     * SENTINEL MONITOR command. */
    void sentinelGenerateInitialMonitorEvents(void) {
        dictIterator *di;
        dictEntry *de;

        di = dictGetIterator(sentinel.masters);
        while((de = dictNext(di)) != NULL) {
            // 获取实例
            sentinelRedisInstance *ri = dictGetVal(de);
            // 检查每个实例并把实例信息log下来
            sentinelEvent(REDIS_WARNING,"+monitor",ri,"%@ quorum %d",ri->quorum);
        }
        dictReleaseIterator(di);
    }

</font>

##6 sentinel的定时处理任务

<font color=blue>

 上面五个流程分析主要说明了sentinel启动过程中的执行步骤，下面分析sentinel启动之后的流程。
</font>

<font color=green>

    void sentinelTimer(void) {
        sentinelCheckTiltCondition();
        sentinelHandleDictOfRedisInstances(sentinel.masters);
        sentinelRunPendingScripts();
        sentinelCollectTerminatedScripts();
        sentinelKillTimedoutScripts();

        /* We continuously change the frequency of the Redis "timer interrupt"
         * in order to desynchronize every Sentinel from every other.
         * This non-determinism avoids that Sentinels started at the same time
         * exactly continue to stay synchronized asking to be voted at the
         * same time again and again (resulting in nobody likely winning the
         * election because of split brain voting). */
        server.hz = REDIS_DEFAULT_HZ + rand() % REDIS_DEFAULT_HZ;
    }

</font>

###6.1 tilt模式

<font color=green>

    /* 这个函数用于判断是否进入tilt模式
    *
    * 如果两次sentinelTimer调用的时间间隔太多（SENTINEL_TILT_TRIGGER = 2s）或者
    * 时间发生错乱（当前时间比上次时间小）那么就进入tilt模式。 出现这些现象的可
    * 能原因如下：
    *
    * 1) Sentiel进程由于一些原因被阻塞住了，如load的文件太大、由于一些I/O阻塞
    * 原因os反应速度减缓、进程收到一些信号后stop住等等。
    * 2) 系统的时钟被调整了。
    *
    * 如果不加以处理，那么redis的所有的timer事件都会超时或者处理失败。redis的
    * 处理方式就是进入tilt模式，除了收集系统信息的任务外不再处理任何事物(脑子
    * 清醒一番)，直到tilt模式超时(SENTINEL_TILT_PERIOD = 30s)为止。
    */
    void sentinelCheckTiltCondition(void) {
        mstime_t now = mstime();
        mstime_t delta = now - sentinel.previous_time;

        if (delta < 0 || delta > SENTINEL_TILT_TRIGGER) {
            sentinel.tilt = 1;
            sentinel.tilt_start_time = mstime();
            // 进入tilt模式后，在+tilt channel上发出通知
            sentinelEvent(REDIS_WARNING, "+tilt", NULL, "#tilt mode entered");
        }
        sentinel.previous_time = mstime();
    }
    /*
    *在sentinel的timer处理函数sentinelHandleRedisInstance中，判断tilt模式是否超时
    */
    void sentinelHandleRedisInstance(sentinelRedisInstance *ri) {
        /* ============== ACTING HALF ============= */
        /* We don't proceed with the acting half if we are in TILT mode.
        * TILT happens when we find something odd with the time, like a
        * sudden change in the clock. */
        if (sentinel.tilt) {
            if (mstime() - sentinel.tilt_start_time < SENTINEL_TILT_PERIOD) return;
            sentinel.tilt = 0;
            sentinelEvent(REDIS_WARNING, "-tilt", NULL, "#tilt mode exited");
        }
    }

    在sentinel tilt模式下，下面两个函数的行为受到了影响：
    /*
     *不能进行master是否进入down状态的判断
     */
    void sentinelCommand(redisClient *c) {
        if (!strcasecmp(c->argv[1]->ptr, "is-master-down-by-addr")) {
            /* SENTINEL IS-MASTER-DOWN-BY-ADDR <ip> <port> <current-epoch> <runid>*/
            int isdown = 0;
            ri = getSentinelRedisInstanceByAddrAndRunID(sentinel.masters,
                c->argv[2]->ptr, port, NULL);

            /* It exists? Is actually a master? Is subjectively down? It's down.
             * Note: if we are in tilt mode we always reply with "0". */
            if (!sentinel.tilt && ri && (ri->flags & SRI_S_DOWN) &&
                (ri->flags & SRI_MASTER))
                isdown = 1;
        }
    }

    /*
     * 不能进行master到slave状态的切换
     * 不能进行slave到master状态的切换
     * 不能对slave变换新的master
     * 不能让slave处理客户端发来的config命令
     */
    void sentinelRefreshInstanceInfo(sentinelRedisInstance *ri, const char *info) {
        /* Process line by line. */
        lines = sdssplitlen(info, strlen(info), "\r\n", 2, &numlines);
        for (j = 0; j < numlines; j++) {
            /* None of the following conditions are processed when in tilt mode, so
             * return asap. */
            if (sentinel.tilt) return;

            /* Handle master -> slave role switch. */
            /* Handle slave -> master role switch. */
            /* Handle slaves replicating to a different master address. */
            /* Detect if the slave that is in the process of being reconfigured
             * changed state. */
        }
    }

</font>

###6.2 检查所有的redis instance的状态

<font color=green>

    /* Perform scheduled operations for all the instances in the dictionary.
    * Recursively call the function against dictionaries of slaves. */
    void sentinelHandleDictOfRedisInstances(dict *instances) {
        dictIterator *di;
        dictEntry *de;
        sentinelRedisInstance *switch_to_promoted = NULL;

        /* There are a number of things we need to perform against every master. */
        di = dictGetIterator(instances);
        while ((de = dictNext(di)) != NULL) {
            sentinelRedisInstance *ri = dictGetVal(de);

            sentinelHandleRedisInstance(ri);
            if (ri->flags & SRI_MASTER) {
                sentinelHandleDictOfRedisInstances(ri->slaves);
                sentinelHandleDictOfRedisInstances(ri->sentinels);
                if (ri->failover_state == SENTINEL_FAILOVER_STATE_UPDATE_CONFIG) {
                    switch_to_promoted = ri;
                }
            }
        }
        if (switch_to_promoted)
            sentinelFailoverSwitchToPromotedSlave(switch_to_promoted);
        dictReleaseIterator(di);
    }

</font>

##主要参考文档：
    1 redis/src/sentinel.c
    2 http://blog.csdn.net/yfkiss/article/details/22151175
    3 http://blog.csdn.net/yfkiss/article/details/22687771
    4 http://redisdoc.com/topic/sentinel.html