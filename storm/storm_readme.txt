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

tickTime=2000
initLimit=10
syncLimit=5
dataDir=data
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

1 /usr/local/kafka/config/server.properties

broker.id=0
port=9000
zookeeper.connect=h1:2181,h2:2181,h3:2181/kafka
log.dirs=/tmp/kafka0-logs

2 start
bin/kafka-server-start.sh config/server.properties &

3 创建topic
bin/kafka-topics.sh --create --zookeeper 116.211.15.189:2200,116.211.15.189:2201,116.211.15.189:2202/kafka --replication-factor 3 --partitions 5 --topic test-kafka-topic


4 查看创建的topic
bin/kafka-topics.sh --describe --zookeeper 116.211.15.189:2200,116.211.15.189:2201,116.211.15.189:2202/kafka  --topic test-kafka-topic
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

[storm+kafka]
apache-storm-0.9.2-incubating这个版本的Storm已经自带了一个集成Kafka的外部插件程序storm-kafka，可以直接使用

reference doc: http://shiyanjun.cn/archives/934.html
