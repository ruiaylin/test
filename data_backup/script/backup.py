"""
#!/bin/python
"""
import sys
import os
import MySQLdb 

def GetRedisMasters(_host, _port, _user, _password, _db):
    conn = MySQLdb.connect(host=_host, port=int(_port), user=_user, passwd=_password, db=_db)
    cursor = conn.cursor()
    sql="select cluster_id,floating_ip,password from cache_instance where cache_instance_type=3;"
    result = cursor.execute(sql)
    redis_masters = []
    for row in cursor.fetchall():
        redis_masters.append([row[0], row[1], row[2]])
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

    host=sys.argv[1] 
    port=sys.argv[2] 
    user=sys.argv[3] 
    password=sys.argv[4] 
    db=sys.argv[5] 
    local_dir=sys.argv[6]
    remote_user="root"
    remote_dir="/root/agent/data/redis_8080"
    redis_masters=GetRedisMasters(host, port, user, password, db)
    for redis_master in redis_masters:
        cmd = "sh backup.sh %s %s %s %s %s %s" % (redis_master[1], remote_user, redis_master[2], remote_dir, redis_master[0], local_dir)
        os.system(cmd)
