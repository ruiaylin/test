#!/usr/bin/env python

import os
import sys
import threading
import traceback
import time
import signal
#import datetime
from datetime import timedelta,datetime
from conf import log_file_conf 

g_terminated = False

try:
    log_fd = open("log_genlog.log","a")
except:
    log_fd = sys.stderr

def getExecInfo():
    type, value, tb = sys.exc_info()
    return str(traceback.format_exception(type, value, tb))

def log(info, level = "WARNING", t = "0"):
    mes = "[%s][ %s ][ %s ]" % (time.asctime(), level, info)
    if t == 1:
        print mes
    log_fd.write(mes + "\n")

def write_file(file_conf):
    try:
        f = open(file_conf['file_name'], 'a+')
        try:
            count = 0
            while not g_terminated:
                t1 = datetime.now()
                for i in range(file_conf['write_line']):
                    f.writelines(str(t1) + "\t\t")
                    f.writelines(file_conf['log_content'] + "\n")
                    f.flush()
                t2 = datetime.now()
                delta = t2 -t1
                print delta.seconds, delta.microseconds
                if delta.seconds < 0 and delta.microseconds > 0:
                    time.sleep(float(1 - delta.microseconds/1000000))
                count = count + 1
                print('the %d time' % count)
        finally:
            f.close()
    except:
        log(getExecInfo)

def sig_handle(signum =0, e=0):
    print('catch the signal %d' % signum)
    g_terminated = True
    sys.exit()

def main():
    thread_pool = []
    signal.signal(signal.SIGTERM, sig_handle)
    signal.signal(signal.SIGALRM, sig_handle)
    signal.signal(signal.SIGINT, sig_handle)
    for i in range(len(log_file_conf)):
        th = threading.Thread(target = write_file, args=(log_file_conf[i],))
        thread_pool.append(th)
    for i in range(len(log_file_conf)):
        thread_pool[i].start()
    signal.alarm(15)

if __name__ == "__main__":
    main()

