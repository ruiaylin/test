[backup.py]
#python
yum install -y python
# mysqldb
yum install -y python-mysql

[backup.sh]
paramiko
pexpect

[command example]
backup cmd:
sh load.sh backup 127.0.0.1 7010 bce_rdsqa_w vZ1UjN0flrAtkrcf bce_scs  /home/test/scs/data_backup/
restore cmd:
sh load.sh restore 127.0.0.1 7010 bce_rdsqa_w vZ1UjN0flrAtkrcf bce_scs 3711 redis_newe2e_0  /home/test/scs/data_backup 3989 V6DM /root/agent/data/redis_7070
