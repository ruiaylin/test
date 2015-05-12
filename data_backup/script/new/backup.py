"""
#!/bin/python
"""
# backup script, written by zhaoxin08 on 2015/04/09
import sys
import os
sys.path.insert(2, "/home/work/opbin/tools/data-backup-tools/script/pexpect-2.3")
import MySQLdb
import pexpect
#import sys
import paramiko

from public import *

def Backup(host, user, passwd, peer_dir, local_dir):
#def SshConnect(host, user, passwd):
    """ SshExcuteCmd """
    ssh = paramiko.SSHClient()
    ssh.load_system_host_keys()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    flag = ""
    psd = passwd[0]
    try:
        print ("host:%s, user:%s, passwd:%s" % (host, user, password))
        ssh.connect(hostname=host, username=user, password=passwd[0])
    except paramiko.AuthenticationException:
        try:
            #print ("try second passwd:%s" % passwd[1])
            ssh.connect(hostname=host, username=user, password=passwd[1])
            psd = passwd[1]
        except paramiko.AuthenticationException:
            print "Auth Failed!"
        else:
            flag = "ok"
    except socket.error:
        print "Server is unreachable!"
    except Exception as e:
        print "exception:", e
    else:
        flag = "ok"

    if flag != "ok":
        return

    ssh.exec_command("yum install -y openssh-clients.x86_64")
    ssh.close()

    print("begin scp host %s:%s" % (host, psd))
    command = ("scp -l 8000 -r -P %s %s@%s:%s/ %s/" % (22, user, host, peer_dir, local_dir))
    scp = pexpect.spawn(command)
    res = scp.expect(['password:', 'continue connecting (yes/no)?', pexpect.EOF, pexpect.TIMEOUT], timeout=100)
    if res == 0:
        scp.sendline(psd)
    elif res == 1:
        scp.sendline('yes\n')
        scp.expect('password: ')
        scp.sendline(psd)
        print ('scp %s OK', host)
    else:
        print('scp %s failed:timeout or eof', host)
        scp.close()
        print('pexpect.EOF or pexpect.TIMEOUT')
        return None
    print ("scp result:%s" % scp.read())
    scp.close()
    return None

def GetRedisMasters(_host, _port, _user, _password, _db):
    conn = MySQLdb.connect(host=_host, port=int(_port), user=_user, passwd=_password, db=_db)
    cursor = conn.cursor()
    #sql="select cluster_id,floating_ip,password,hash_name,port from cache_instance where cache_instance_type=3 and status=4;"
    #sql="select cluster_id,floating_ip,password,hash_name,port from cache_instance where cache_instance_type=3 and cluster_id in (select id from cache_cluster where status=5);"
    #sql="select cache_instance.status,cache_instance.cluster_id,cache_instance.floating_ip,cache_instance.password,cache_instance.hash_name,cache_instance.port from cache_instance,cache_cluster where cache_instance_type=3 and cache_instance.cluster_id=cache_cluster.id and cache_cluster.status=5 and cache_instance.status=0;"
    sql="select cache_instance.cluster_id,cache_instance.floating_ip,cache_instance.password,cache_instance.hash_name,cache_instance.port from cache_instance,cache_cluster where cache_instance_type=3 and cache_instance.cluster_id=cache_cluster.id and cache_cluster.status=5 and cache_instance.status=0;"
    result = cursor.execute(sql)
    redis_masters = []
    for row in cursor.fetchall():
        #if (row[1] != "10.81.250.178") and (row[1] != "10.81.250.197") and (row[1] != "10.81.250.23"):
        #    redis_masters.append([row[0], row[1], row[2]])
        #redis_masters.append([row[0], row[1], row[2], row[3]])
        if IsOpen(row[1], row[4]):
            redis_masters.append([row[0], row[1], row[2], row[3]])
    return redis_masters


"""print help"""
def printHelp():
    """ print help prompt
    """
    print 'usage:'
    print '  example: ./backup.py mysql-host mysql-port user password db local_dir'
    #mysql -h publicdb.bce-preinternal.baidu.com -P7010 -ubce_rdsqa_w -pvZ1UjN0flrAtkrcf bce_scs"

if __name__ == '__main__':
    if len(sys.argv) < 7:
        printHelp()
        sys.exit(1)
	
#10.58.144.51 5100 bce_r 29nCkiadfsa bce_scs /home/disk1/nfs_data
    host=sys.argv[1]
    port=sys.argv[2]
    user=sys.argv[3]
    password=sys.argv[4]
    db=sys.argv[5]
    remote_user="root"
    remote_dir="/root/agent/data/redis_8080"
    redis_masters=GetRedisMasters(host, port, user, password, db)
    for redis_master in redis_masters:
        #print ("%s-%s-%s" % (redis_master[0], redis_master[1], redis_master[2]))
        #cmd = "sh backup.sh %s %s %s %s %s %s %s" % (redis_master[1], remote_user, redis_master[2], remote_dir, redis_master[0], redis_master[3], local_dir)
        cluster_id=redis_master[0]
        remote_host=redis_master[1]
        remote_password = []
        remote_password.append(redis_master[2])
        remote_password.append("admin@123")
        hash_name=redis_master[3]
        local_dir=sys.argv[6]
        local_dir=("%s/%s/%s" % (local_dir, cluster_id, hash_name))
        cmd = ("mkdir -p %s && cd %s && rm -rf ./data_old && mv data data_old && mkdir data" % (local_dir, local_dir))
        #print cmd
        os.system(cmd)
        local_dir=("%s/data" % local_dir)
        cmd = ("mkdir -p %s" % local_dir)
        #print cmd
        os.system(cmd)
        Backup(remote_host, remote_user, remote_password, remote_dir, local_dir)

