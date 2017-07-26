#!/usr/bin/env bash

# The output of all these installation steps is noisy. With this utility
# the progress report is nice and concise.
function install {
    echo Installing $1
    shift
    yum -y install "$@" >/dev/null 2>&1
}

# 设置hosts文件
echo "Update /etc/hosts"
cat > /etc/hosts <<EOF
127.0.0.1       localhost
::1 localhost localhost.localdomain localhost6 localhost6.localdomain6
192.168.100.100 node0
192.168.100.101 node1
192.168.100.102 node2
EOF

echo "Remove unused logs"
sudo rm -rf /etc/udev/rules.d/70-persistent-net.rules /root/anaconda-ks.cfg /root/install.log /root/install.log.syslog /root/install-post.log

# 关掉防火墙
echo "Disable iptables"
setenforce 0 >/dev/null 2>&1 && iptables -F

### Set env ###
echo "export LC_ALL=en_US.UTF-8"  >>  /etc/profile
cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

# 配置yum源并
echo "Setup yum repos"
# rm -rf /etc/yum.repos.d/*
# cp /vagrant/*.repo /etc/yum.repos.d/
yum clean all >/dev/null 2>&1

# 修改root帐号密码为redhat
echo "Setup root account"
# Setup sudo to allow no-password sudo for "admin". Additionally,
# make "admin" an exempt group so that the PATH is inherited.
cp /etc/sudoers /etc/sudoers.orig
echo "root            ALL=(ALL)               NOPASSWD: ALL" >> /etc/sudoers
echo 'redhat'|passwd root --stdin >/dev/null 2>&1

# 禁止透明大页及修改 swappiness
echo never > /sys/kernel/mm/transparent_hugepage/defrag
echo never > /sys/kernel/mm/transparent_hugepage/enabled
echo 0 > /proc/sys/vm/swappiness
echo "echo never > /sys/kernel/mm/transparent_hugepage/defrag" | tee -a /etc/rc.local
echo "echo never > /sys/kernel/mm/transparent_hugepage/enabled" | tee -a /etc/rc.local

# 设置公网网络下的命名服务解析
echo "Setup nameservers"
# http://ithelpblog.com/os/linux/redhat/centos-redhat/howto-fix-couldnt-resolve-host-on-centos-redhat-rhel-fedora/
# http://stackoverflow.com/a/850731/1486325
echo "nameserver 8.8.8.8" | tee -a /etc/resolv.conf
echo "nameserver 8.8.4.4" | tee -a /etc/resolv.conf

# 生成ssh公要文件
echo "Setup ssh"
[ ! -d /root/.ssh ] && ( mkdir /root/.ssh ) && ( chmod 600 /root/.ssh  ) && yes|ssh-keygen -f ~/.ssh/id_rsa -t rsa -N ""

# 安装一些常用软件
install Git git
install "Base tools" vim wget curl
install "Hadoop dependencies" expect rsync pssh

echo 'All set, rock on!'
