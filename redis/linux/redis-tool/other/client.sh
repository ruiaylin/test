#!/bin/sh
#host=$1
#port=$2
#password=$3
host="127.0.0.1"
port=$1
password=redis-instance-password
#cat data | ./redis-cli -h $host -p $port -a $password --pipe
./redis-cli -h $host -p $port -a $password

