"""
#!/bin/python
"""
# backup script, written by AlexStocks on 2015/04/09
import sys
import os
import httplib
import json
#sys.path.insert(2, "/home/work/opbin/tools/data-backup-tools/script/pexpect-2.3")
import MySQLdb
import pexpect
import paramiko
import logging
import log
import socket
import commands

from conf import *
from public import *

def GetLocalIp():
    return socket.gethostbyname(socket.gethostname())

def Curl(host, port, httpType, url, headers):
    """ Curl """
    connection = httplib.HTTPConnection(host, port)
    connection.request(httpType, url, '', headers)
    response = connection.getresponse()
    result = response.read().strip()
    status = response.status
    if not str(status).startswith('2'):
        logging.warn(result)
        logging.warn(status)
        sys.exit(-1)
    return json.loads(result)

def GetRsInfo(host, port, vip, vport):
    uri = "/?r=interface/api&handler=getRsInfo&vip=" + vip + "&port=" + str(vport)
    logging.debug("host:" + host + " port:" + str(port) +" uri: " + uri)
    return Curl(host, port, 'GET', uri, {})

def GetValidRs(response):
    """ GetValidRs """
    logging.debug(json.dumps(response))
    if response['status'] == 0:
        if 'message' in response.keys():
            if 'port_info_list' in response['message'].keys():
                if len(response['message']['port_info_list']) == 1:
                    if 'vs_port' in response['message']['port_info_list'][0].keys():
                        response['message']['port_info_list'][0]['vs_port'] = int(
                        response['message']['port_info_list'][0]['vs_port'])
                    else:
                        ErrorQuit("vs_port not exit :" +
                        json.dumps(response['message']['port_info_list'][0]))
                    if 'vs_port_end' in response['message']['port_info_list'][0].keys():
                        response['message']['port_info_list'][0]['vs_port_end'] = int(
                        response['message']['port_info_list'][0]['vs_port_end'])
                    else:
                        ErrorQuit("vs_port_end not exit :" +
                        json.dumps(response['message']['port_info_list'][0]))
                    if 'rs_info_list' in response['message']['port_info_list'][0].keys():
                        if len(response['message']['port_info_list'][0]['rs_info_list']) == 2:
                            for rs in response['message']['port_info_list'][0]['rs_info_list']:
                                if rs['weight'] == "1":
                                    return rs['rs_ip']
                        else:
                            ErrorQuit("Wrong rs_info_list len: " +
                            json.dumps(response['message']['port_info_list'][0]['rs_info_list']))
                    else:
                        ErrorQuit("rs_info_list not exit: " +
                        json.dumps(response['message']['port_info_list'][0]))
                else:
                    ErrorQuit("Wrong port_info_list len: " +
                    json.dumps(response['message']['port_info_list']))
            else:
                ErrorQuit("message not exist: " + json.dumps(response['message']))
        else:
            ErrorQuit("Wrong response: " + json.dumps(response))
    else:
        ErrorQuit("Wrong status: " + json.dumps(response))

    return None

def GetProcessNum(process):
    command = ('ps aux | grep %s | grep -vE \'noah|grep\' | wc -l' % process)
    num = os.popen(command, 'r').readlines()[0]

    return num

def IsMaster():
    response = GetRsInfo(bgw['host'], (int)(bgw['port']), bgw['vip'], bgw['vport'])
    valid_rs = GetValidRs(response)
    logging.debug("valid_rs :%s" % valid_rs)
    local_host = GetLocalIp()
    logging.debug("local host:%s" % local_host)
    if valid_rs == local_host:
        return True
    else:
        logging.warn('Error: local host %s is not the csmaster master!' % local_host)

    return False

def CheckAof(name):
    suffix = ".aof"
    suffix_len = len(suffix)
    flag = False
    for _name in name:
        _name_len = len(_name)
        #logging.critical("%s %s %s %s" % (_name, suffix_len < len(_name), _name[_name_len-4 :_name_len], _name[_name_len-4 :_name_len] == suffix))
        if suffix_len < len(_name) and _name[_name_len-4 :_name_len] == suffix:
            flag = True
            break

    return flag

def Backup(host, user, passwd, peer_dir, local_dir):
    """ SshExcuteCmd """
    print('host:%s, passwd:%s' % (host, passwd))
    ssh = paramiko.SSHClient()
    #ssh.load_system_host_keys()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    flag = ""
    psd = passwd[0]
    try:
        logging.info("host:%s, user:%s, passwd:%s" % (host, user, psd))
        ssh.connect(hostname=host, username=user, password=passwd[0])
    except paramiko.AuthenticationException:
        try:
            #print ("try second passwd:%s" % passwd[1])
            psd = passwd[1]
            logging.info("try again:host:%s, user:%s, passwd:%s" % (host, user, psd))
            ssh.connect(hostname=host, username=user, password=passwd[1])
        except paramiko.AuthenticationException:
            logging.critical("Auth Failed!")
        except socket.error:
            logging.critical("Server %s is unreachable!" % host)
        except Exception as e:
            logging.critical("connect host %s, exception: %s" % (host, e))
        else:
            flag = "ok"
    except socket.error:
        logging.critical("Server %s is unreachable!" % host)
    except Exception as e:
        #print "exception:", e
        logging.critical("connect host %s, exception: %s" % (host, e))
    else:
        flag = "ok"

    if flag != "ok":
        return

    ssh.exec_command("yum install -y openssh-clients.x86_64")
    ssh.exec_command("/root/agent/bin/redis-cli -h 127.0.0.1 -p 8080 config set appendonly yes")

    logging.info("begin scp host %s" % (host))
    command = ("scp -l 8000 -r -P %s %s@%s:%s/%s %s/" % (22, user, host, peer_dir, 'appendonly.aof', local_dir))
    scp = pexpect.spawn(command)
    res = scp.expect(['password:', 'continue connecting (yes/no)?', pexpect.EOF, pexpect.TIMEOUT], timeout=None)
    if res == 0:
        scp.sendline(psd)
    elif res == 1:
        scp.sendline('yes\n')
        scp.expect('password: ')
        scp.sendline(psd)
        print ('scp %s OK' % host)
    else:
        logging.critical('scp %s failed:timeout or eof' % host)
        scp.close()
        return None
    # scp.expect(['dump.rdb', 'appendonly.aof', pexpect.EOF, pexpect.TIMEOUT], timeout=None)
    scp.expect(pexpect.EOF, timeout=None) # pexpect.TIMEOUT: Timeout exceeded in read_nonblocking().
    # logging.debug("scp result:%s" % scp.read())
    logging.debug("scp %s finished" % host)
    scp.close()

    stdin, stdout, stderr = ssh.exec_command("find %s -type f" % peer_dir)
    files = stdout.read()
    ssh.close()
    file_str = ("").join(files)
    file_list = file_str.split('\n')
    logging.debug("host %s files:%s\n" % (host, files))
    if CheckAof(file_list) == False:
        logging.critical("host %s aof disable, its files:\n%s" % (host, files))

    return None

def GetRedisMasters(_host, _port, _user, _password, _db):
    conn = MySQLdb.connect(host=_host, port=int(_port), user=_user, passwd=_password, db=_db)
    cursor = conn.cursor()
    #sql="select cluster_id,floating_ip,password,hash_name,port from cache_instance where cache_instance_type=3 and status=4;"
    #sql="select cluster_id,floating_ip,password,hash_name,port from cache_instance where cache_instance_type=3 and cluster_id in (select id from cache_cluster where status=5);"
    #sql="select cache_instance.status,cache_instance.cluster_id,cache_instance.floating_ip,cache_instance.password,cache_instance.hash_name,cache_instance.port from cache_instance,cache_cluster where cache_instance_type=3 and cache_instance.cluster_id=cache_cluster.id and cache_cluster.status=5 and cache_instance.status=0;"
    sql="select cache_instance.cluster_id,cache_instance.floating_ip,cache_instance.password," \
	    "cache_instance.hash_name,cache_instance.port from cache_instance,cache_cluster " \
		"where cache_instance_type=3 and cache_instance.cluster_id=cache_cluster.id " \
		"and engine_type=2 and (cache_cluster.status=5 or cache_cluster.status=16);"
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
    print '  example: ./backup.py'
    #mysql -h publicdb.bce-preinternal.baidu.com -P7010 -ubce_rdsqa_w -pvZ1UjN0flrAtkrcf bce_scs"

def __clearKnownHosts():
    known_hosts = os.path.expanduser('~')
    known_hosts += "/.ssh/known_hosts" 
    cmd = ("rm -rf %s" % known_hosts)
    os.system(cmd)

if __name__ == '__main__':
    cwd = os.path.dirname(os.path.realpath(__file__))
    log.init_log(cwd + '/log/backup.log')
    logging.info('backup starting...')

    # offline
    if online == 0 and False == IsMaster():
        print "error: not real csmaster master!"
        sys.exit(1)

    # online
    num = GetProcessNum('csmaster')
    logging.debug("master process number:%s" % num)
    if online == 1 and int(num) <= 0:
        sys.exit(1)

    # clear known_hosts
    __clearKnownHosts()

    remote_user="root"
    remote_dir="/root/agent/data/redis_8080"
    redis_masters=GetRedisMasters(mysql['host'], mysql['port'], mysql['user'], mysql['password'], mysql['db'])
    for redis_master in redis_masters:
        #print ("%s-%s-%s" % (redis_master[0], redis_master[1], redis_master[2]))
        cluster_id=redis_master[0]
        remote_host=redis_master[1]
        remote_password = []
        remote_password.append(redis_master[2])
        remote_password.append("admin@123")
        hash_name=redis_master[3]
        local_dir = ("%s/%s/%s" % (local_backup_dir, cluster_id, hash_name))
        #cmd = ("mkdir -p %s && cd %s && rm -rf ./data_old && mv data data_old && mkdir data" % (local_dir, local_dir))
        cmd = ("mkdir -p %s && cd %s && mkdir data_new" % (local_dir, local_dir))
        #print cmd
        os.system(cmd)
        local_dir=("%s/data_new" % local_dir)
        #cmd = ("mkdir -p %s" % local_dir)
        #print cmd
        #os.system(cmd)
        #print('peer{host:%s, user:%s, password:%s, dir:%s}' % (remote_host, remote_user, remote_password, remote_dir))
        Backup(remote_host, remote_user, remote_password, remote_dir, local_dir)
        cmd = ("find %s/ -type f | wc -l" % local_dir)
        file_num = commands.getoutput(cmd)
        # the data_new is empty, delete it
        if int(file_num) == 0:
            cmd = ("cd %s && cd .. && rm -rf ./data_new" % local_dir)
            os.system(cmd)
            continue

        data_dir = ("%s/../data" % local_dir)
        cmd = ("find %s/../data -type f | wc -l" % local_dir)
        file_num = 0
        if os.path.isdir(data_dir):
            file_num = commands.getoutput(cmd)
        cmd = ("cd %s && cd .. && mv data_new data" % local_dir)
        if int(file_num) != 0:
            # the data dir is not empty, delete data_old and move data to data_old
            if os.path.isdir(data_dir):
                cmd = ("cd %s && cd .. && rm -rf ./data_old" \
                        " && mv data data_old && mv data_new data" % local_dir)

        os.system(cmd)
