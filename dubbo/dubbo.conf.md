

## 1 dubbo-consumer.xml

### 1.1 本地服务配置

	<dubbo:reference id="userService" group="db" interface="com.patty.dubbo.api.service.UserService"
                     timeout="10000" retries="3" mock="true" check="false">
	</dubbo:reference>

   设置 mock="true",表示启用容错处理，当出现RpcException时，服务端会在客户端执行容错逻辑，Mock的方法会被调用。

### 1.2 延迟暴露

如果你的服务需要Warmup时间，比如初始化缓存，等待相关资源就位等，可以使用delay进行延迟暴露，设置delay="5000"表示延迟5s暴露服务，delay="-1"表示延迟到Spring初始化完成后，再暴露服务，这样做可以避免Spring2.x初始化死锁问题。

### 1.3 并发控制

- 服务端并发 
-   

	<dubbo:service interface="com.foo.BarService" executes="10" /> 

表示限制服务的各个方法，服务器端并发执行（或占用线程池线程数）不能超过10个。也可以指定特定方法的并发数，在<dubbo: method>中配置

- 客户端并发
- 
	<dubbo:service interface="com.foo.BarService" actives="10" /> 

表示限制服务的各个方法，客户端并发执行（或占用连接的请求数）不能超过10个。也可以指定特定方法的并发数，在<dubbo: method>中配置。 actives属性也可在<dubbo: reference>中配置，并且如果<dubbo:service>和<dubbo:reference>都配了actives，<dubbo:reference>优先

### 1.4 连接控制

限制服务器端接受的连接不能超过10个：（以连接在Server上，所以配置在Provider上）

	<dubbo:provider protocol="dubbo" accepts="10" />
	
	or
	
	<dubbo:protocol name="dubbo" accepts="10" />
限制客户端服务使用连接连接数：(如果是长连接，比如Dubbo协议，connections表示该服务对每个提供者建立的长连接数)
	
	<dubbo:reference interface="com.foo.BarService" connections="10" />
	
	or
	
	<dubbo:service interface="com.foo.BarService" connections="10" />
accepts是指定服务端的最大可接受连接数， connections则是指定客户端对每个提供者的最大连接数，rmi、http、hessian等短连接协议表示限制连接数，dubbo等长连接协表示建立的长连接个数。

### 1.5 延迟连接

延迟连接，用于减少长连接数，当有调用发起时，再创建长连接，相当于一种懒加载的模式，只对使用长连接的dubbo协议生效。

	<dubbo:protocol name="dubbo" lazy="true" />
	
	
### 1.6 粘滞连接

粘滞连接用于有状态服务，尽可能让客户端总是向同一提供者发起调用，除非该提供者挂了，再连另一台。粘滞连接将自动开启延迟连接，以减少长连接数。

	<dubbo:protocol name="dubbo" sticky="true" />
	
### 1.7 负载均衡策略	
	
	<dubbo:service interface="..." loadbalance="roundrobin" /> 或
	
	<dubbo:reference interface="..." loadbalance="roundrobin" /> 或
	
	<dubbo:service interface="..."> 或
	    <dubbo:method name="..." loadbalance="roundrobin"/> 
	</dubbo:service>
	<dubbo:reference interface="..."> 或
	    <dubbo:method name="..." loadbalance="roundrobin"/>
	</dubbo:reference>
	
一般不再配置文件中写死负载均衡策略， 而是动态使用默认配置，然后在需要时通过dubbo-admin管控台修改策略配置。


一致性Hash，相同参数的请求总是发到同一提供者。
当某一台提供者挂时，原本发往该提供者的请求，基于虚拟节点，平摊到其它提供者，不会引起剧烈变动。
算法参见：http://en.wikipedia.org/wiki/Consistent_hashing。
缺省只对第一个参数Hash，如果要修改，请配置<dubbo:parameter key="hash.arguments" value="0,1" />
缺省用160份虚拟节点，如果要修改，请配置<dubbo:parameter key="hash.nodes" value="320" />

## 2 spring配置解析

### 2.1、<dubbo:service/>

   服务配置，用于暴露一个服务，定义服务的元信息，一个服务可以用多个协议暴露，一个服务也可以注册到多个注册中心

<dubbo:service ref="userService" interface="com.patty.dubbo.api.service.UserService"/>
interface用于指定接口名，ref指定服务引用对象，这两者必填，此外还有一些其他属性可供选择，比如：
group: 服务分组，当一个接口有多个实现，可以用分组区分
timeout: 远程服务调用超时时间(毫秒)，默认1000ms
retries: 远程服务调用重试次数，不包括第一次调用，不需要重试请设为0,默认值为2

### 2.2、<dubbo:reference/> 

  1） 引用配置，用于创建一个远程服务代理，一个引用可以指向多个注册中心。

<dubbo:reference id="userService" interface="com.patty.dubbo.api.service.UserService"
                 timeout="10000" retries="3" check="false"/>
id： 服务所引用的bean id， 对应service中ref属性的值
check:启动时检查提供者是否存在，为false表示关闭检查。
   <dubbo:consumer check="false" /> 关闭所有服务的检查，没有提供者的时候check值为true报错,false忽略
   <dubbo:registry check="false" /> 关闭注册中心启动时检查，注册订阅失败时check值为true报错, false忽略

2）在开发及测试环境下，经常需要绕过注册中心，只测试指定服务提供者，这时候可能需要点对点直连直连服务，通过设置url属性可指定服务提供者地址，多个地址用逗号隔开。
<dubbo:reference id="xxxService" interface="com.alibaba.xxx.XxxService" url="dubbo://localhost:20890" />
也可以通过JVM启动参数设置， 如：java -Dcom.alibaba.xxx.XxxService=dubbo://localhost:20890

### 2.3、<dubbo:protocol/> 协议配置，用于配置提供服务的协议信息，协议由提供方指定，消费方被动接受。

<dubbo:protocol name="${dubbo.protocol.name}" port="${dubbo.protocol.port}"/>
name: 协议名称
port: 服务端口
可选协议有
1) dubbo:// Dubbo缺省协议采用单一长连接和NIO异步通讯，适合于小数据量大并发的服务调用，以及服务消费者机器数远大于服务提供者机器数的情况。
<dubbo:protocol name=“dubbo” port=“9090” server=“netty” client=“netty” codec=“dubbo” serialization=“hessian2” charset=“UTF-8” threadpool=“fixed” threads=“100” queues=“0” iothreads=“9” buffer=“8192” accepts=“1000” payload=“8388608” />
 
      server: 协议的服务器端实现类型

      client：协议的客户端实现类型

      codec: 协议编码方式

　　serialization: 协议序列化方式

　　threadpool: 线程池类型，可选：fixed/cached

 ThreadPool
fixed 固定大小线程池，启动时建立线程，不关闭，一直持有。(缺省)
cached 缓存线程池，空闲一分钟自动删除，需要时重建。
limited 可伸缩线程池，但池中的线程数只会增长不会收缩。(为避免收缩时突然来了大流量引起的性能问题)。
　　threads: 服务线程池大小(固定大小)，默认100

　　accepts: 服务提供者最大可接受连接数

　　缺省协议，使用基于mina1.1.7+hessian3.2.1的tbremoting交互。

　　连接个数：单连接
　　连接方式：长连接
　　传输协议：TCP
　　传输方式：NIO异步传输
　　序列化：Hessian二进制序列化
　　适用范围：传入传出参数数据包较小（建议小于100K），消费者比提供者个数多，单一消费者无法压满提供者，尽量不要用dubbo协议传输大文件或超大字符串。
　　适用场景：常规远程服务方法调用
  2) rmi://

  3）hessian://

  4) http:// 

连接个数：多连接
连接方式：短连接
传输协议：HTTP
传输方式：同步传输
序列化：表单序列化
适用范围：传入传出参数数据包大小混合，提供者比消费者个数多，可用浏览器查看，可用表单或URL传入参数，暂不支持传文件。
适用场景：需同时给应用程序和浏览器JS使用的服务。
     可选的server有jetty和servlet.

  5) thrift://

  6) memcached://

  7）redis://

  8) webservice://


### 2.4、<dubbo:application/> 应用配置，用于配置当前应用信息，不管该应用是提供者还是消费者。

    <dubbo:application name="${dubbo.application.name}"/>
   name:当前应用名称，用于注册中心计算应用间依赖关系，服务方和消费方不必保持一样，该名称不是匹配条件
   version:当前应用的版本

### 2.5、<dubbo:registry/> 注册中心配置，用于配置连接注册中心相关信息。

1）<dubbo:registry protocol="${dubbo.registry.protocol}" address="${dubbo.registry.address}"/>
protocol: 注同中心地址协议，支持dubbo, http, local三种协议，分别表示，dubbo地址，http地址，本地注册中心
address:注册中心服务器地址，如果地址没有端口缺省为9090，同一集群内的多个地址用逗号分隔，如：ip:port,ip:port，不同集群的注册中心，请配置多个<dubbo:registry>标签

2）为方便开发测试，经常会在线下共用一个所有服务可用的注册中心，这时，如果一个正在开发中的服务提供者注册，可能会影响消费者不能正常运行。可以让服务提供者开发方，只订阅服务(开发的服务可能依赖其它服务)，而不注册正在开发的服务，通过直连测试正在开发的服务。
<dubbo:registry address="10.20.153.10:9090" register="false" />
3）同样设置属性subscribe="false"，则服务提供方将不进行订阅操作。

### 2.6、其他的一些配置项

<dubbo:module/> 模块配置，用于配置当前模块信息，可选。
<dubbo:monitor/> 监控中心配置，用于配置连接监控中心相关信息，可选。
<dubbo:provider/> 提供方的缺省值，当ProtocolConfig和ServiceConfig某属性没有配置时，采用此缺省值，可选。
<dubbo:consumer/> 消费方缺省配置，当ReferenceConfig某属性没有配置时，采用此缺省值，可选。
<dubbo:method/> 方法配置，用于ServiceConfig和ReferenceConfig指定方法级的配置信息。
<dubbo:argument/> 用于指定方法参数配置。

### 2.7、属性配置方式

1）dubbo.properties文件

如果公共配置很简单，没有多注册中心，多协议等情况，或者想多个Spring容器想共享配置，可以使用dubbo.properties作为缺省配置

Dubbo将自动加载classpath根目录下的dubbo.properties，可以通过JVM启动参数：-Ddubbo.properties.file=xxx.properties 改变缺省配置位置

覆盖策略：

JVM启动-D参数优先，这样可以使用户在部署和启动时进行参数重写，比如在启动时需改变协议的端口。
XML次之，如果在XML中有配置，则dubbo.properties中的相应配置项无效。
Properties最后，相当于缺省值，只有XML没有配置时，dubbo.properties的相应配置项才会生效，通常用于共享公共配置，比如应用名。

### 2.8、多协议服务配置

实际应用中， 不同服务在性能上适用不同协议进行传输，比如大数据用短连接协议，小数据大并发用长连接协议。

    <!-- 多协议配置 -->
    <dubbo:protocol name="dubbo" port="20880" />
    <dubbo:protocol name="rmi" port="1099" />
 
    <!-- 使用dubbo协议暴露服务 -->
    <dubbo:service interface="com.alibaba.hello.api.HelloService" version="1.0.0" ref="helloService" protocol="dubbo" />
    <!-- 使用rmi协议暴露服务 -->
    <dubbo:service interface="com.alibaba.hello.api.DemoService" version="1.0.0" ref="demoService" protocol="rmi" />
当然也可以同一服务使用多协议暴露出去，如：
<!-- 使用多个协议暴露服务 -->
    <dubbo:service id="helloService" interface="com.alibaba.hello.api.HelloService" version="1.0.0" protocol="dubbo,rmi" />
 
### 2.9、多注册中心
    <!-- 多注册中心配置 -->
    <dubbo:registry id="hangzhouRegistry" address="10.20.141.150:9090" />
    <dubbo:registry id="qingdaoRegistry" address="10.20.141.151:9010" default="false" />
 
    <!-- 同一服务向多个注册中心注册 -->
    <dubbo:service interface="com.alibaba.hello.api.HelloService" version="1.0.0" ref="helloService" registry="hangzhouRegistry,qingdaoRegistry" />
    
    <!-- 不同服务向不同的注册中心注册 -->
    <!-- 向中文站注册中心注册 -->
    <dubbo:service interface="com.alibaba.hello.api.TestService" version="1.0.0" ref="testService" registry="chinaRegistry" />
 
    <!-- 向国际站注册中心注册 -->
    <dubbo:service interface="com.alibaba.hello.api.DemoService" version="1.0.0" ref="demoService" registry="intlRegistry" />
 