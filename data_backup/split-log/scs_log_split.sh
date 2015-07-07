#!/bin/sh
datetime=`date "+%Y%m%d%H"`
lasthour=`date "+%Y%m%d%H" -d "-1 hour"`
daytime=`date "+%Y%m%d"`
for dir in `cat $1`
do
	for i in `ls  $dir/*.log`
	do
    	mv $i $i.$datetime
	done
	for i in `ls $dir/*.log.wf`
	do
    	mv $i $i.$datetime
	done

	find $dir  -mtime +7 -name "*.log.*"  -exec rm -f {} \;

done

