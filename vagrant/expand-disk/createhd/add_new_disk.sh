#!/usr/bin/env bash
# ******************************************************
# DESC    :
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : Apache License 2.0
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2017-07-27 12:08
# FILE    : add_new_disk.sh
# ******************************************************

set -e
set -x

if [ -f /etc/disk_added_date ]
then
   echo "disk already added so exiting."
   exit 0
fi


sudo fdisk -u /dev/sdb <<EOF
n
p
1


t
8e
w
EOF

pvcreate /dev/sdb1
vgextend VolGroup /dev/sdb1
lvextend /dev/VolGroup/lv_root
resize2fs /dev/VolGroup/lv_root

date > /etc/disk_added_date
