[backup.py]
#python
bash -c "$( curl http://jumbo.baidu.com/install_jumbo.sh )"; source ~/.bashrc
#python
jumbo install python
# mysqldb
jumbo install python-mysql

[backup.sh]
paramiko
pexpect

[command example]
backup cmd:
sh load.sh backup publicdb.bce-preinternal.baidu.com 7010 bce_rdsqa_w vZ1UjN0flrAtkrcf bce_scs  /home/yangqi/scs/data_backup/
restore cmd:
sh load.sh restore publicdb.bce-preinternal.baidu.com 7010 bce_rdsqa_w vZ1UjN0flrAtkrcf bce_scs 3711 redis_newe2e_0  /home/yangqi/scs/data_backup 3989 V6DMU7F0RwNteaTqpOl9Pb1kv2XuKi0G_0  /root/agent/data/redis_7070
