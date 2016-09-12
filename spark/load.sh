#!/usr/bin/env bash
# ******************************************************
# DESC    : spark devops script
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : LGPL V3
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2016-09-09 11:19
# FILE    : load.sh
# ******************************************************

name="spark"
host="localhost.localdomain"
rpc_port="7077"

usage() {
    echo "Usage: $0 start {master|worker|shell}"
    echo "Usage: $0 stop  {master|worker}"
    echo "Usage: $0 list  {master|worker}"
    echo "Usage: $0 shell"
    exit
}

start() {
    local role=$1
    if [ $role = "master" ]; then
        ${SPARK_HOME}/sbin/start-master.sh
    elif [ $role = "worker" ]; then
        nohup ${SPARK_HOME}/bin/spark-class org.apache.spark.deploy.worker.Worker spark://${host}:${rpc_port} &
    fi
     PID=`ps aux | grep -w $name | grep -w $role | grep -v grep | awk '{print $2}'`
    if [ "$PID" != "" ];
    then
        for p in $PID
        do
            echo "start $role ( pid =" $p ")"
        done
    fi
}

stop() {
    local role=$1
    PID=`ps aux | grep -w $name | grep -w $role | grep -v grep | awk '{print $2}'`
    if [ "$PID" != "" ];
    then
        for ps in $PID
        do
            echo "kill -9 $role ( pid =" $ps ")"
            kill -9 $ps
        done
    fi
    sleep 3
}

shell() {
    # sh ${SPARK_HOME}/bin/spark-shell --master spark://${host}:${rpc_port}
    sh ${SPARK_HOME}/bin/pyspark --master spark://${host}:${rpc_port}
}

run() {
    local script=$1
    sh ${SPARK_HOME}/bin/spark-submit ${script}
}


list() {
    local role=$1
    PID=`ps aux | grep -w $name | grep -w $role | grep -v grep | awk '{printf("%s,%s,%s,%s\n", $1, $2, $9, $10)}'`
    if [ "$PID" != "" ];
    then
        echo "list ${name} $role"
        echo "index: user, pid, start, duration"
        idx=0
        for ps in $PID
        do
            echo "$idx: $ps"
	    ((idx ++))
        done
    fi
}

opt=$1
case C"$opt" in
    Cstart)
        if [ $# != 2 ]; then
            usage
        fi
        start $2
        ;;
    Cstop)
        if [ $# != 2 ]; then
            usage
        fi
        stop $2
        ;;
    Clist)
    	if [ $# != 2 ]; then
            usage
    	fi
    	list $2
    ;;
    Crun)
    	if [ $# != 2 ]; then
            usage
    	fi
    	run $2
    ;;
 
    Cshell)
        shell
        ;;
    C*)
        usage
        ;;
esac

