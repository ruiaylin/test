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

