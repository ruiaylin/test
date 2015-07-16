"""
#!/bin/python
"""
# restore script, written by AlexStocks on 2015/04/27
import sys
import os
import MySQLdb 

def GetPassword(_host, _port, _user, _password, _db, _cluster_id, _ip):
    print "cluster id:%s, ip:%s" % (_cluster_id, _ip)
    conn = MySQLdb.connect(host=_host, port=int(_port), user=_user, passwd=_password, db=_db)
    cursor = conn.cursor()
    sql="select password from cache_instance where cache_instance_type=3 and cluster_id=%s and floating_ip=\"%s\";" % (_cluster_id, _ip)
    result = cursor.execute(sql)
    record=cursor.fetchall()
    password=record[0][0]

    return password


"""print help"""
def printHelp():
    """ print help prompt
    """
    print 'usage:'
    print '  example: ./backup.py mysql-host mysql-port user password db local_dir 1 127.0.0.1'
    #mysql -h publicdb.bce-preinternal.baidu.com -P7010 -ubce_rdsqa_w -pvZ1UjN0flrAtkrcf bce_scs"
    

if __name__ == '__main__':
    if len(sys.argv) < 9:
        printHelp()
        sys.exit(1)

    host=sys.argv[1] 
    port=sys.argv[2] 
    user=sys.argv[3] 
    password=sys.argv[4] 
    db=sys.argv[5] 
    local_dir=sys.argv[6]
    src_cluster_id=sys.argv[7]
    src_ip=sys.argv[8]
    dst_cluster_id=sys.argv[9]
    dst_ip=sys.argv[10]
    dst_dir=sys.argv[11]
    local_dir+="/"
    local_dir+=src_cluster_id
    local_dir+="/"
    local_dir+=src_ip
    local_dir+="/data/"
    dst_user="root"
    password=GetPassword(host, port, user, password, db, dst_cluster_id, dst_ip)
    cmd = "sh restore.sh %s %s %s %s %s" % (dst_ip, dst_user, password, dst_dir, local_dir)
    os.system(cmd)
