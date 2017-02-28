#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ******************************************************
# DESC    : calculator kafka throughout
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : Apache License 2.0
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2016-12-20 20:51
# FILE    : calculator.py
# ******************************************************

# bootstrap_servers = 'localhost:9092,localhost:19092,localhost:29092'
bootstrap_servers = 'localhost:9092'
topic = 'test0'
msg_count = 1000000
# msg_count = 1000
msg_size = 100
# msg_payload = ('kafkatest' * 20).encode()[:msg_size]
producer_timings = {}
consumer_timings = {}

def calculate_thoughput(timing, n_messages=1000000, msg_size=100):
    print("Processed {0} messsages in {1:.2f} seconds".format(n_messages, timing))
    print("{0:.2f} MB/s".format((msg_size * n_messages) / timing / (1024*1024)))
    print("{0:.2f} Msgs/s".format(n_messages / timing))

