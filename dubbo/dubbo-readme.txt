[zookeeper]

zoo.cfg以及data/myid具体内容参考storm/zk-script,这里只启动就可以了：
sh load.sh start

[monitor]

1 tar -zxvf dubbo-monitor-simple-2.4.10-assembly.tar.gz

2 vim conf/dubbo.properties
修改注册中心的配置，注释掉广播地址，改为zookeeper作为注册中心:
#dubbo.registry.address=multicast://224.5.6.7:1234
dubbo.registry.address=zookeeper://127.0.0.1:2181

3 其它参数如有需要可自行修改。（注意：如果是本机测试，将monitor和admin都撞倒一台机器上时，可以将dubbo.jetty.port=8083）

4 sh bin/start.sh

5 访问地址：http://127.0.0.1:8083 (默认为8080，8083是修改后的值)

[provider]
1 tar -zxvf dubbo-demo-provider-2.4.10-assembly.tar.gz

2 修改注册中心的配置地址为zookeeper，如下：
#dubbo.registry.address=multicast://224.5.6.7:1234
dubbo.registry.address=zookeeper://127.0.0.1:2181

3 执行
sh bin/start.sh

[consumer]

1 tar -zxvf dubbo-demo-consumer-2.4.10-assembly.tar.gz

2 修改注册中心的配置地址为zookeeper，如下：
#dubbo.registry.address=multicast://224.5.6.7:1234
dubbo.registry.address=zookeeper://127.0.0.1:2181

3 执行
sh bin/start.sh

