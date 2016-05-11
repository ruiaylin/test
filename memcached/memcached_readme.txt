[memcache & memadmin by srouce]
[libevent]
unzip Libevent-release-2.0.22-stable.zip
cd Libevent-release-2.0.22-stable/
sh autogen.sh 
./configure  
make  
make install  

[memcached]
unzip memcached-1.4.25.zip
cd memcached-1.4.25/
sh autogen.sh
./configure
make
make install

[php]
sudo apt-get install libxml2-dev  
tar -zxvf php-5.3.8.tar.gz  
cd php-5.3.8  
./configure  
make  
make install  

[libmemcached]
http://launchpad.net/libmemcached/1.0/0.52/+download/libmemcached-0.52.tar.gz 
tar zxf libmemcached-0.52.tar.gz 
cd libmemcached-0.52/
./configure 
make
meke install

[PHP Memcache扩展]
yum install php-devel
phpize  
wget http://pecl.php.net/get/memcache-3.0.6.tgz 
tar zxf memcache-3.0.6.tgz
cd memcache-3.0.6
./configure --enable-memcache --with-php-config=/usr/bin/php-config  --with-zlib-dir  
make  
make install  
    [修改php.ini]
    php -i | grep ini # 查找php.ini的位置
    在php.ini中添加：extension=/usr/local/lib64/memcache.so
    
[error]
1 httpd/logs/error_log: [Mon May 09 20:17:42 2016] [error] [client 61.129.119.185] PHP Warning:  Unknown: open(/var/lib/php/session/sess_idj0shsj27c649mgfjrkqfi6b0, O_RDWR) failed: Permission denied (13) in Unknown on line 0
vim /etc/php.ini
添加 session.save_path = "/tmp/lib/php/session"

[test]
# test.php
<?php  
        $mem = new Memcache;  
        $mem->connect("127.0.0.1", 11211);  
        $mem->set('key', 'This is a test!', 0, 60);  
        $val = $mem->get('key');  
        echo $val;  
?> 

php -c /etc/php5/cgi/php.ini test.php  

[安装memadmin的第二种方法]
1 安装 httpd & phpize
yum install -y php # install php & httpd
yum install -y php-devel

2 安装memadmin包
tar jxf memadmin-pkg.bz2
cd memadmin-pkg
cp memcache.so /usr/local/lib64/

3 修改php.ini

mkdir -p /tmp/lib/php/session
vim /etc/php.ini
extension=/usr/local/lib64/memcache.so
session.save_path = "/tmp/lib/php/session"



