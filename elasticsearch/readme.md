# elasticsearch set #


---
## 各个压缩包说明
* es.bz2 - es模板  
  es的版本是2.4.6，另外有load.sh和plugins(bigdesk, head, kopf, license, marvel-agent, sql)
* es_stat.bz2 - 统计脚本

---
## 集群生成
* 创建一个文件夹
* 把elasticsearch放入
* 按照load.sh的命令提示创建并启动即可
* es版本是2.4.6，对应的kibana版本是4.6.5，去https://www.elastic.co/downloads/past-releases下载

—-
## 注意事项
* 注意修改load.sh中host一行中的网卡名称；
* 注意修改bin/elasticsearch的ES_HEAP_SIZE；
* 注意修改config/elasticsearch.yml中的path.data & path.logs & discovery.zen.ping.unicast.hosts三个参数；
* 
* 把es从mac通过rz上传到linux的时候，注意删除”./lib/._lucene-sandbox-5.5.4.jar”一类的无用文件，否则elasticsearch启动时候会报这些问题打不开的error；
* 创建index的模板可参考[这里](https://github.com/alexstocks/python-practice/blob/master/mysql_redis_es_flume/es/es_test.sh);
* kibana使用的index中要有一个index字段，类型是date，日志中的时间末尾应该是”+08:00”;
* 

---
* written by Alex Stocks on 2017/04/01
* modified on 2017/11/02

