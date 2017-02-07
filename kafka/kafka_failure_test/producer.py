#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ******************************************************
# DESC    : calculate kafka producer throughout by kafka-python
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : Apache License 2.0
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2016-12-20 21:02
# FILE    : producer.py
# ******************************************************

import time
from kafka import KafkaProducer

from calculator import *

def python_kafka_producer_performance():
    brokers = bootstrap_servers.split(',')
    producer = KafkaProducer(
        bootstrap_servers=brokers,
        max_block_ms=5000)
        # api_version = (0, 10))

    producer_start = time.time()
    for i in range(msg_count):
        msg_payload = 'hello' + str(i)
        print msg_payload
        # import pdb; pdb.set_trace()
        try:
            time.sleep(3)
            producer.send(topic, key=msg_payload, value=bytes(msg_payload))
        except Exception as e:
            print 'exception:', e

    producer.flush() # clear all local buffers and produce pending messages

    return time.time() - producer_start

producer_timings['python_kafka_producer'] = python_kafka_producer_performance()
calculate_thoughput(producer_timings['python_kafka_producer'])

# test results:
# Processed 1000000 messsages in 68.10 seconds
# 1.40 MB/s
# 14685.32 Msgs/s
