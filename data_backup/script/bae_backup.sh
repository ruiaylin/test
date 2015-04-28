#!/bin/sh
function backup() {
	remote_host=$1
	remote_port="22"
	remote_dir=$2
	remote_password=WSYX@bae
	local_dir=$3
	for((i=0;i<4;i++));do
		#echo $remote_host $remote_password $remote_dir/$i/data $local_dir/$remote_host/$i/
		mkdir -p $local_dir/$remote_host/$i/
		/home/bae/bin/expect -c "
		  spawn scp -l 8000 -r -P $remote_port bae@$remote_host:$remote_dir/$i/data $local_dir/$remote_host/$i/data
		  expect {
		    \"*assword\" {set timeout 300; send \"$remote_password\r\";}
		    \"yes/no\" {send \"yes\r\"; exp_continue;}
		  }
		  expect eof" 
	done
}
local_dir=/home/bae/redis/data_backup/data
remote_dir=/home/bae/redis/redis-instance-master/
old_dir=$local_dir"-"`date -d"-4 day" +%F`
today_dir=$local_dir"-"`date +%F`"/"`date +%H-%M-%S`
rm -rf $old_dir
mkdir -p $today_dir

#mv $local_dir $local_dir"-"`date +%F-%H-%M-%S`
remote_host=st01-bce-redis00.st01
backup $remote_host $remote_dir $today_dir
##
remote_host=st01-bae-redis-master01.st01
backup $remote_host $remote_dir $local_dir
##
remote_host=st01-bae-redis-master02.st01
backup $remote_host $remote_dir $local_dir
##
remote_host=st01-bae-ssd2-pre03.st01
remote_dir=/home/bae/redis/redis-instance-slave/
backup $remote_host $remote_dir $local_dir
