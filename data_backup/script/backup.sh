#!/bin/sh
function backup() {
	remote_host=$1
	remote_port=22
	remote_user=$2
	remote_password=$3
	remote_dir=$4
	local_dir=$5
	/home/gm/bin/expect -c "
	spawn scp -l 8000 -r -P $remote_port $remote_user@$remote_host:$remote_dir/ $local_dir/
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
cluster_id=$5
local_dir=$6
	
local_dir=$local_dir/$cluster_id/$remote_host
mkdir -p $local_dir
cd $local_dir
rm -rf ./data_old
mv data data_old
mkdir data
local_dir=$local_dir/data
mkdir -p $local_dir

backup $remote_host $remote_user $remote_password $remote_dir $local_dir
#echo $remote_host $remote_user $remote_password $remote_dir $local_dir
