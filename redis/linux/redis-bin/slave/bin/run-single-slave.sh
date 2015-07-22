name="redis-slave"
port=0
dirname=""

database_num=40
maxmemory=80mb

master_ip=10.50.144.17

#--slaveof  $master_ip $port
start() {
	stop
	nohup ./bin/$name$dirname conf/redis.conf --port $port --databases $database_num --maxmemory $maxmemory --appendonly yes --appendfilename appendonly$dirname.aof --dir $(pwd)/data --logfile $(pwd)/log/server.log --dbfilename dump$dirname.rdb --appendfsync everysec --masterauth redis-instance-password --requirepass redis-instance-password 2>&1 1>/dev/null & 
}

stop() {
	killall -9 $name$dirname
}

case C"$1" in
	C)
		echo "Usage: $0 {start|stop}"
		;;
	Cstart)
		if [ $# != 3 ]; then
			echo "Please Input: dirname port"
		else
			dirname=$2
			port=$3
			start
			echo "Done!"
		fi
		;;
	Cstop)
		if [ "-$2" = "-" ]; then
			echo "Please Input: dirname "
		else
			dirname=$2
			stop
			echo "Done!"
		fi
		;;
	C*)
		echo "Usage: $0 {start|stop}"
		;;
esac
