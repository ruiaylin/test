[backup.py]
#python
bash -c "$( curl http://jumbo.baidu.com/install_jumbo.sh )"; source ~/.bashrc
#python
jumbo install python
# mysqldb
jumbo install python-mysql

[backup.sh]
#tcl
tar zxf tcl8.4.11-src.tar.gz && cd tcl8.4.11/unix/ && ./configure --prefix=/home/gm && make && make install && cd -
#expect
tar zxf expect5.45.tar.gz && cd expect5.45 &&  ./configure --prefix=/home/gm --with-tcl=/home/gm/lib --with-tclinclude=../tcl8.4.11/generic  && make && make install && cd -

[command example]
backup cmd:
sh load.sh backup publicdb.bce-preinternal.baidu.com 7010 bce_rdsqa_w vZ1UjN0flrAtkrcf bce_scs /home/gm/scs/data_backup
restore cmd:
sh load.sh restore publicdb.bce-preinternal.baidu.com 7010 bce_rdsqa_w vZ1UjN0flrAtkrcf bce_scs /home/gm/scs/data_backup 3711 10.81.250.205 3989 10.81.250.160 /root/agent/data/redis_7070
