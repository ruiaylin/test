#!/usr/bin/env bash  
# ******************************************************
# DESC    : PalmChat httpd instance devops script
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : LGPL V3
# EMAIL   : zhaoxin@zenmen.com
# MOD     : 2016-05-09 14:54
# FILE    : httpd-run.sh
# ******************************************************

name="httpd"
export ROOT=`pwd`
export HOST=`/sbin/ifconfig -a|grep inet|grep -v 127.0.0.1|grep -v 10.|grep -v inet6|awk '{print $2}'|tr -d "addr:"`
export STATICROOT="/home/zhaoxin/test/php/memadmin"

start() {
	stop
	mkdir -p run logs pid
	$name -d ./
	PID=`ps aux | grep -w $name | grep $USER | grep -v grep | awk '{print $2}'`
	if [ "$PID" != "" ];
	then
		for p in $PID
		do
			echo "start $name ( pid =" $p ")"
		done
	fi
}

stop() {
	# ps aux | grep -w $name | grep -v grep | awk '{print $2}' | xargs kill -9
	PID=`ps aux | grep -w $name | grep $USER | grep -v grep | awk '{print $2}'`
	if [ "$PID" != "" ];
	then
		for ps in $PID
		do
			echo "kill -9 $name ( pid =" $ps ")"
			kill -9 $ps
		done
	fi
	sleep 3
}

clean() {
    stop
	rm -rf run logs pid
}

case C"$1" in
	C)
		echo "Usage: $0 {start|stop|clean}"
		;;
	Cstart)
		start
		echo "Done!"
		;;
	Cstop)
		stop
		echo "Done!"
		;;
	Cclean)
		clean
		echo "Done!"
		;;
	C*)
		echo "Usage: $0 {start|stop|clean}"
		;;
esac

