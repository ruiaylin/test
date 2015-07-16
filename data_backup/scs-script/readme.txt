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
1 modify configure file:conf.py
2 python backup.py

[crontab]
1 */4 * * * source /home/work/.bash_profile && python /home/test/backup.py