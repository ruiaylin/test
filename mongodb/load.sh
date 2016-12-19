#!/usr/bin/env bash
# ******************************************************
# DESC    : zookeeper devops script
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : LGPL V3
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2016-05-13 02:01
# FILE    : load.sh
# ******************************************************

name="mongod"
DB_PATH=${MONGODB_HOME}/data
DB_HOST=localhost
DB_PORT=10000
HTTP_PORT=10001
WEB_UI_PORT=`expr $DB_PORT + 1000`

usage() {
    echo "Usage: $0 start"
    echo "       $0 stop"
    echo "       $0 restart"
    echo "       $0 list"
    echo "       $0 client  zk-port"
    exit
}

start() {
    # nohup mongod --dbpath=${DB_PATH} --logpath=${DB_PATH}/mongo.log --fork --logappend=true --rest --bind_ip=${DB_HOST} --port=${DB_PORT} --httpinterface  &
    nohup mongod --dbpath=${DB_PATH} --logpath=${DB_PATH}/mongo.log --fork --logappend --rest --bind_ip=${DB_HOST} --port=${DB_PORT} --httpinterface &
	PID=`ps aux | grep -w $name | grep "${DB_PORT}" | grep -v grep | awk '{print $2}'`
    if [ "$PID" != "" ];
    then
        for p in $PID
        do
            echo "start $name ( pid =" $p ")"
        done
    fi
	echo "web ui interface: http://${DB_HOST}:${WEB_UI_PORT}"
}

stop() {
	PID=`ps aux | grep -w $name | grep "${DB_PORT}" | grep -v grep | awk '{print $2}'`
    if [ "$PID" != "" ];
    then
        for ps in $PID
        do
            echo "kill -SIGINT $name ( pid =" $ps ")"
			kill -2 $ps
        done
    fi
}

term() {
	PID=`ps aux | grep -w $name | grep "${DB_PORT}" | grep -v grep | awk '{print $2}'`
    if [ "$PID" != "" ];
    then
        for ps in $PID
        do
            echo "kill -SIGKILL $name ( pid =" $ps ")"
			kill -9 $ps
        done
    fi
}

list() {
	PID=`ps aux | grep -w $name | grep "${DB_PORT}" | grep -v grep | awk '{printf("%s,%s,%s,%s\n", $1, $2, $9, $10)}'`
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

client() {
	mongo --host=${DB_HOST} --port=${DB_PORT}
}

opt=$1
case C"$opt" in
    Cstart)
		start
        ;;
    Cstop)
		stop
        ;;
    Cterm)
		term
        ;;
    Crestart)
		stop
		term
		start
        ;;
    Clist)
		list
        ;;
    Cclient)
		# if [ $# != 2 ]; then
		# 	usage
		# fi
		client
        ;;
    C*)
        usage
        ;;
esac

