#!/usr/bin/env python
# name public.py
import os
import socket

def IsOpen(ip, port):
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    try:
        s.connect((ip,int(port)))
        s.close()
        #print ("valid redes master: %s:%s" % (ip, port))
        return True
    except:
        #print ("invalid redis master: %s:%s" % (ip, port))
        return False
