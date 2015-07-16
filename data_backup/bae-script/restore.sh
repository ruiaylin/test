#!/bin/sh
# restore tool, written by zhaoxin08 on 2015/04/27
function restore() {
	remote_host=$1
	remote_port=22
	remote_user=$2
	remote_password=$3
	remote_dir=$4
	local_dir=$5
	expect -c "
	spawn scp -P $remote_port -r $local_dir/ $remote_user@$remote_host:$remote_dir/ 
	expect {
	    \"*assword\" {set timeout 300; send \"$remote_password\r\";}
	    \"yes/no\" {send \"yes\r\"; exp_continue;}
	}
	expect eof" 
}

remote_host=$1
remote_user=$2
remote_password=$3
remote_dir=$4
local_dir=$5

restore $remote_host $remote_user $remote_password $remote_dir $local_dir
#echo $remote_host $remote_user $remote_password $remote_dir $local_dir
