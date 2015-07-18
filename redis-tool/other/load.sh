#!/bin/sh
rm data/*
#rm bin/redis-server
#cp ../../redis-3.0.0/src/redis-server ./bin/
ps aux | grep redis | grep 40000 | grep -v grep | awk '{print $2}' | xargs kill -9
nohup bin/redis-server4 conf/redis.conf --port 40000 --databases 400 --maxmemory 8g --appendonly yes --appendfilename appendonly4.aof --dir $(pwd)/data --logfile $(pwd)/log/server.log --dbfilename dump4.rdb --appendfsync everysec --requirepass redis-instance-password  &
