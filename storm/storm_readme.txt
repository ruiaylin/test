[zookeeper]

1 zoo.cfg

zoo.cfg规则比较严格，注释的#后面要有空格，赋值时等号左右不能有空格。

[conf配置说明]
tickTime=2000:
tickTime这个时间是作为Zookeeper服务器之间或客户端与服务器之间维持心跳的时间间隔,也就是每个tickTime时间就会发送一个心跳；

initLimit=10:
initLimit这个配置项是用来配置Zookeeper接受客户端（这里所说的客户端不是用户连接Zookeeper服务器的客户端,而是Zookeeper服务器集群中连接到Leader的Follower 服务器）初始化连接时最长能忍受多少个心跳时间间隔数。
当已经超过10个心跳的时间（也就是tickTime）长度后 Zookeeper 服务器还没有收到客户端的返回信息,那么表明这个客户端连接失败。总的时间长度就是 10*2000=20 秒；

syncLimit=5:
syncLimit这个配置项标识Leader与Follower之间发送消息,请求和应答时间长度,最长不能超过多少个tickTime的时间长度,总的时间长度就是5*2000=10秒；

dataDir=/export/search/zookeeper-cluster/zookeeper-3.4.6-node1/data
dataDir顾名思义就是Zookeeper保存数据的目录,默认情况下Zookeeper将写数据的日志文件也保存在这个目录里；

clientPort=2181
clientPort这个端口就是客户端连接Zookeeper服务器的端口,Zookeeper会监听这个端口接受客户端的访问请求；

server.1=localhost:2887:3887
server.2=localhost:2888:3888
server.3=localhost:2889:3889
server.A=B：C：D：
A是一个数字,表示这个是第几号服务器,B是这个服务器的ip地址
C第一个端口用来集群成员的信息交换,表示的是这个服务器与集群中的Leader服务器交换信息的端口
D是在leader挂掉时专门用来进行选举leader所用

[zoo.cfg 示例]

# 最基本的tick时间单元, ms
tickTime=2000
# follower启动后会从leader拉取全量配置，花费时间可能比较长，时间长度是initLimit * tickTime
initLimit=10
# leader与follower之间增量同步的超时时长，可以认为是心跳包超时时间，时间长度是syncLimit * tickTime
syncLimit=5
dataDir=data
# log数据的目录尽量与data数据的目录不要放在一个磁盘上，以达到最高性能
dataLogDir=data/log
autopurge.snapRetainCount=3
autopurge.purgeInterval=1
clientPort=2201
server.1=116.211.15.192:2001:3001
server.2=116.211.15.192:2002:3002
server.3=116.211.15.192:2003:3003

2 myid

echo "1">zookeeper1/data/myid
echo "2">zookeeper2/data/myid
echo "3">zookeeper3/data/myid

3 start
bin/zkServer.sh start

4 test
bin/zkServer.sh status {
    JMX enabled by default
    Using config: /home/zhaoxin/test/java/zk-node1/bin/../conf/zoo.cfg
    Mode: follower
}

echo stat|nc localhost 2203 {
    Zookeeper version: 3.4.6-1569965, built on 02/20/2014 09:09 GMT
    Clients:
     /0:0:0:0:0:0:0:1:7054[0](queued=0,recved=1,sent=0)

    Latency min/avg/max: 0/0/0
    Received: 2
    Sent: 1
    Connections: 1
    Outstanding: 0
    Zxid: 0x0
    Mode: follower
    Node count: 4
}

jps {
    5914 QuorumPeerMain
    4019 QuorumPeerMain
    3074 Elasticsearch
    3158 Elasticsearch
    6124 Jps
    5749 QuorumPeerMain
}


sh zkCli.sh -server localhost:2201

[kafka]

0 在zk中为kafka创建路径
./zkCli.sh -server localhost:2201
create /kafka ''
ls /

1 /usr/local/kafka/config/server.properties

broker.id=0
# host.name=localhost 
port=9000
zookeeper.connect=h1:2181,h2:2181,h3:2181/kafka
dataDir=/tmp/data/kafka0-logs  
log.dirs=/tmp/kafka0-logs
delete.topic.enable=true

注意：

(1) host.name注释掉的原因
http://stackoverflow.com/questions/17668262/kafka-cant-connect-to-zookeeper-fatal-fatal-error-during-kafkaserverstable-star
http://stackoverflow.com/questions/25497279/unknownhostexception-kafka

http://m.oschina.net/blog/279052:

kafka会用到hostname，所以 需要修改操作系统的hostname，否者后面执行kafka的shell命令时会报 unknownhostname的异常 
centos的hostname修改方式：
#1 root权限
hostname bogon
#2 修改/etc/hosts
127.0.0.1       localhost.localdomain localhost
::1             localhost6.localdomain6 localhost6

10.10.113.120 bogon bogon
#3 修改/etc/sysconfig/network
NETWORKING=yes
NETWORKING_IPV6=no
HOSTNAME=bogon

(2) 如果delete.topic.enable没有设置为true，则删除topic时会出现下面的提示：
linux-vt7e:~/test/java/kafka> sh bin/kafka-topics.sh --delete --zookeeper localhost:2201/kafka --topic test-kafka-topic
Topic test-kafka-topic is marked for deletion.
Note: This will have no impact if delete.topic.enable is not set to true.

解决方法就是把delete.topic.enable设置为true，然后重启kafka：
bin/kafka-server-stop.sh config/server.properties 
bin/kafka-server-start.sh config/server.properties
[2016-05-12 22:51:07,545] INFO Deleting index /tmp/kafka0-oogs/test-kafka-topic-0/00000000000000000000.index (kafka.log.OffsetIndex)
[2016-05-12 22:51:07,547] INFO Deleted log for partition [test-kafka-topic,0] in /tmp/kafka0-oogs/test-kafka-topic-0. (kafka.log.LogManager)

(3) jvm的一些参数
http://m.oschina.net/blog/279052
jvm参数中-XX标识的是实验性参数，kafka用了很多用来优化运行的jvm参数，而你安装的 jdk所带的jvm不一定支持这些参数，比如： -XX:+UseCompressedOops 
如果你遇到
Unrecognized VM option '+UseCompressedOops'
的错误，请在bin/kafka-run-class.sh中移除相关参数
上述两个步骤比较重要。



2 start
bin/kafka-server-start.sh config/server.properties &

stop
bin/kafka-server-stop.sh config/server.properties   

3 创建topic
bin/kafka-topics.sh --create --zookeeper 116.211.15.189:2201,116.211.15.189:2202,116.211.15.189:2203/kafka --replication-factor 3 --partitions 5 --topic test-kafka-topic

replica数目不能大于broker数目
bin/kafka-topics.sh --create --zookeeper localhost:2201/kafka --replication-factor 3 --partitions 5  --topic test-kafka-topic  Error while executing topic command replication factor: 3 larger than available brokers: 1
kafka.admin.AdminOperationException: replication factor: 3 larger than available brokers: 1
        at kafka.admin.AdminUtils$.assignReplicasToBrokers(AdminUtils.scala:70)
        at kafka.admin.AdminUtils$.createTopic(AdminUtils.scala:171)
        at kafka.admin.TopicCommand$.createTopic(TopicCommand.scala:93)
        at kafka.admin.TopicCommand$.main(TopicCommand.scala:55)
        at kafka.admin.TopicCommand.main(TopicCommand.scala)

bin/kafka-topics.sh --create --zookeeper localhost:2201/kafka --replication-factor 1 --partitions 5  --topic test-kafka-topic
[2016-05-12 22:55:24,860] WARN Partition [test-kafka-topic,4] on broker 0: No checkpointed highwatermark is found for partition [test-kafka-topic,4] (kafka.cluster.Partition)
[2016-05-12 22:55:24,797] WARN Partition [test-kafka-topic,2] on broker 0: No checkpointed highwatermark is found for partition [test-kafka-topic,2] (kafka.cluster.Partition)

上面这条日志id解释：This shouldn't be a problem. The broker logs this message when it doesn't have information about the current offset for a topic, but that is expected when the topic is new. This has since been changed to INFO level since the WARN level is misleading, but that change hasn't made it into a release yet.
https://groups.google.com/forum/#!topic/confluent-platform/4ikdndHhIfI

4 查看创建的topic

sh bin/kafka-topics.sh --zookeeper localhost:2201/kafka --list

bin/kafka-topics.sh --describe --zookeeper 116.211.15.189:2201,116.211.15.189:2202,116.211.15.189:2203/kafka  --topic test-kafka-topic

Topic:test-kafka-topic  PartitionCount:5        ReplicationFactor:3     Configs:
        Topic: test-kafka-topic Partition: 0    Leader: 0       Replicas: 0,1,2 Isr: 0,1,2
        Topic: test-kafka-topic Partition: 1    Leader: 1       Replicas: 1,2,0 Isr: 1,2,0
        Topic: test-kafka-topic Partition: 2    Leader: 2       Replicas: 2,0,1 Isr: 2,0,1
        Topic: test-kafka-topic Partition: 3    Leader: 0       Replicas: 0,2,1 Isr: 0,2,1
        Topic: test-kafka-topic Partition: 4    Leader: 1       Replicas: 1,0,2 Isr: 1,0,2
上面Leader、Replicas、Isr的含义如下：


Partition： 分区
Leader   ： 负责读写指定分区的节点
Replicas ： 复制该分区log的节点列表
Isr      ： "in-sync" replicas，当前活跃的副本列表（是一个子集），并且可能成为Leader

5 test

producer
bin/kafka-console-producer.sh --broker-list 116.211.15.189:19000,116.211.15.189:19001,116.211.15.189:19002 --topic test-kafka-topic

consumer
bin/kafka-console-consumer.sh --zookeeper 116.211.15.189:2200,116.211.15.189:2201,116.211.15.189:2202/kafka  --from-beginning --topic test-kafka-topic

[storm]

0 在zk中为storm创建路径
./zkCli.sh -server localhost:2201
create /storm ''
ls /

1 下载storm
wget http://mirror.bit.edu.cn/apache/storm/apache-storm-1.0.1/apache-storm-1.0.1.tar.gz

2 配置conf/storm.yaml
storm.local.dir: "/tmp/storm0"

storm.zookeeper.servers:
    - "116.211.15.189"

storm.zookeeper.port: 2203

nimbus.host: "116.211.15.190"

supervisor.slots.ports:
    - 6700
    - 6701
    - 6702
    - 6703

storm.local.dir: "/tmp/storm0"

3 启动nimbus & supervisor & ui

nohup bin/storm nimbus >/dev/null 2>&1 &
nohup bin/storm supervisor >/dev/null 2>&1 &
nohup bin/storm ui >/dev/null 2>&1  &

4 test

启动Storm Topology：
storm jar allmycode.jar org.me.MyTopology arg1 arg2 arg3
其中，allmycode.jar是包含Topology实现代码的jar包，org.me.MyTopology的main方法是Topology的入口，arg1、arg2和arg3为org.me.MyTopology执行时需要传入的参数。

停止Storm Topology：
storm kill {toponame}
其中，{toponame}为Topology提交到Storm集群时指定的Topology任务名称。

6 storm的成员
disruptor\drpc\thrift\netty\zookeeper\zmq\simple-acl

[storm+kafka]
apache-storm-0.9.2-incubating这个版本的Storm已经自带了一个集成Kafka的外部插件程序storm-kafka，可以直接使用

1 在提交Topology程序到Storm集群之前，因为用到了Kafka，需要拷贝一下依赖jar文件到Storm集群中的lib目录下面

cp /usr/local/kafka/libs/kafka_2.9.2-0.8.1.1.jar /usr/local/storm/lib/
cp /usr/local/kafka/libs/scala-library-2.9.2.jar /usr/local/storm/lib/
cp /usr/local/kafka/libs/metrics-core-2.2.0.jar /usr/local/storm/lib/
cp /usr/local/kafka/libs/snappy-java-1.0.5.jar /usr/local/storm/lib/
cp /usr/local/kafka/libs/zkclient-0.3.jar /usr/local/storm/lib/
cp /usr/local/kafka/libs/log4j-1.2.15.jar /usr/local/storm/lib/
cp /usr/local/kafka/libs/slf4j-api-1.7.2.jar /usr/local/storm/lib/
cp /usr/local/kafka/libs/jopt-simple-3.2.jar /usr/local/storm/lib/

2 执行测试程序

bin/storm jar test/friend-recommend-0.0.1.jar im.youni.friend_recommend.MyKafkaTopology 116.211.15.189

可以通过查看日志文件（logs/目录下）或者Storm UI来监控Topology的运行状况。如果程序没有错误，可以使用前面我们使用的Kafka Producer来生成消息，就能看到我们开发的Storm Topology能够实时接收到并进行处理。
上面Topology实现代码中，有一个很关键的配置对象SpoutConfig，配置属性如下所示：

spoutConf.forceFromStart = false;

该配置是指，如果该Topology因故障停止处理，下次正常运行时是否从Spout对应数据源Kafka中的该订阅Topic的起始位置开始读取，如果forceFromStart=true，则之前处理过的Tuple还要重新处理一遍，否则会从上次处理的位置继续处理，保证Kafka中的Topic数据不被重复处理，是在数据源的位置进行状态记录。

reference doc: http://shiyanjun.cn/archives/934.html

[storm knowledge]

1 storm-redis from 0.10.0

https://github.com/apache/storm/tree/master/external/storm-redis

2 Topology并行度计算

有关Topology的并行度的计算，官网上有一篇文章介绍（后面参考链接中已附上），我们这里详细解释一下，对于理解Storm UI上面的一些统计数据也会有很大帮助。在编写代码设置并行度的时候，并行度只是一个提示信息，Storm会根据这个提示信息并结合其他一些参数配置（Task个数、Worker个数），去计算运行时的并行度，这个并行度实际上描述的是，组成一个Topology的多个Spout/Bolt的运行时表现实体Task的分布，所以我们可能会想关注从一个Topology的角度去看，这些设置了并行度的Spout/Bolt对应的运行时Task，在集群的多个Worker进程之间，以及Executor内部是如何分布的。
下面是例子给出的Topology的设计，如下图所示：
storm-example-topology
对该例子Topology配置了2个Worker，对应的代码示例如下所示：


conf.setNumWorkers(2); // 该Topology运行在Supervisor节点的2个Worker进程中
conf.setMaxSpoutPending(5000);
conf.setMessageTimeoutSecs(60); // 提交Topology 时设置适当的消息超时时间，比默认消息超时时间（30秒）更长。
topologyBuilder.setSpout("blue-spout", new BlueSpout(), 2); // 设置并行度为2，则Task个数为2*1
topologyBuilder.setBolt("green-bolt", new GreenBolt(), 2)
               .setNumTasks(4)
               .shuffleGrouping("blue-spout"); // 设置并行度为2，设置Task个数为4 ，则Task个数为4
topologyBuilder.setBolt("yellow-bolt", new YellowBolt(), 6)
               .shuffleGrouping("green-bolt"); // 设置并行度为6，则Task个数为6*1
               
那么，下面我们看Storm是如何计算一个Topology运行时的并行度，并分配到2个Worker中的：

计算Task总数：2*1+4+6*1=12（总计创建12个Task实例）
计算运行时Topology并行度：10/2=5（每个Worker对应5个Executor）
将12个Task分配到2个Worker中的5*2个Executor中：应该是每个Worker上5个Executor，将6个Task分配到5个Executor中
每个Worker中分配6个Task，应该是分配3个Yellow Task、2个Green Task、1个Blue Task
Storm内部优化：会把同类型的Task尽量放到同一个Executor中运行
分配过程：从Task个数最少的开始，1个Blue Task只能放到一个Executor，总计1个Executor被占用；2个Green Task可以放到同一个Executor中，总计2个Executor被占用；最后看剩下的3个Yellow Task能否分配到5-2=3个Executor中，显然每个Yellow Task对应一个Executor
从直观上看，其实分配Task到多个Executor中的分配结果有很多种，都能满足尽量让同类型Task在相同的Executor中，有关Storm的计算实现可以参考源码。

3 feilds grouping
In order to send the message to the same task every time storm will mod the hashcode of the value with the number of tasks (hashcode(values)% #tasks). 

4 关于task与executor的关系
在我们配置storm的时候，不知大家是否主要到了一个问题，就是我们在配置的时候会加几个worker的端口( supervisor.slots.ports:)，比如众多文档中提到的6700/6701等等类似的东西，没错这就是我们定义了该supervisor最多的worker数，worker中执行一个bolt或者spout线程，我们就称之为taks，而executor是物理上的线程概念，而task更多是逻辑概念上的，有时候bolt与spout的task会共用executor，特别是在系统负荷比较高的时候。

5 IRichBolt与IBasicBolt接口区别
首先从类组成上进行分析可以看到，IBasicBolt接口只有execute方法和declareOutputFields方法，而IRichBolt接口上除了以上几个方法还有prepare方法和cleanup及Map方法。而且其中execute方法是有些不一样的，其参数列表不同。

总体来说Rich方法比较晚上，我们可以使用prepare方法进行该Bolt类的初始化工作，例如我们链接数据库时，需要进行一次数据库连接操作，我们就可以把该操作放入prepare中，只需要执行一次就可以了。而cleanup方法能在该类调用结束时进行收尾工作，往往在处理数据的时候用到，例如在写hdfs（hadoop的文件系统）数据的时候，在结束时需要进行数据clear，则需要进行数据收尾。当然，根据官网及本人的实验，该方法往往是执行失败的。

