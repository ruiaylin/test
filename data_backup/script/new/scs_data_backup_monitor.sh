#!/bin/sh
# data backup monitor script
# written by Alex Stocks on 2015/05/26 12:00

ps_num=$(ps aux | grep csmaster | grep -vE 'noah|grep' | wc -l)
if [[ ${ps_num} -eq 0 ]]
then
    exit
fi  

dir="/home/disk1/nfs_data"
for sub_dir in $(find ${dir} -maxdepth 1)
do 
    cluster_id=$(basename ${sub_dir})
    if [[ ${cluster_id} != *[!0-9]* ]]; then                                                
        #echo $sub_dir $cluster_id
        data_file_num=$(find ${sub_dir} -cmin -260 -type f | wc -l)
        old_data_file_num=$(find ${sub_dir} -cmin -500 -type f | wc -l)
        if [ ${data_file_num} -eq 0 -a ${old_data_file_num} -eq 0 ]
        then
            echo cluster_id:$cluster_id
        fi  
    fi 
done

