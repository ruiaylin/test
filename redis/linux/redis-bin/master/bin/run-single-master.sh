name="redis-server"
port=0
dirname=""

database_num=40
maxmemory=80mb

start() {
	stop
#--requirepass redis-instance-password 
	nohup ./bin/$name$dirname conf/redis.conf --port $port --databases $database_num --maxmemory $maxmemory --save 300 10 --dir $(pwd)/data --logfile $(pwd)/log/server.log --dbfilename dump$dirname.rdb 2>&1 1>/dev/null & 
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
			echo "Please Input: dirname"
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
