#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ******************************************************
# DESC    : calculate kafka consumer throughout by confluent-kafka-python
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : Apache License 2.0
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2016-12-20 21:21
# FILE    : confluent_consumer.py
# ******************************************************

import time
import uuid

import confluent_kafka

from calculator import *

def confluent_kafka_consumer_performance():
    msg_consumed_count = 0
    conf = {'bootstrap.servers': bootstrap_servers,
            'group.id': uuid.uuid1(),
            'session.timeout.ms': 6000,
            'default.topic.config': {
                'auto.offset.reset': 'latest' # 'earliest'
            }
    }

    consumer = confluent_kafka.Consumer(**conf)

    consumer_start = time.time()
    # This is the same as pykafka, subscribing to a topic will start a background thread
    consumer.subscribe([topic])

    while True:
        msg = consumer.poll(1)
        if msg:
            print 'msg(topic:%s, partition:%d, key:%s, value:%s):' % (msg.topic(), msg.partition(), msg.key(), msg.value())
            msg_consumed_count += 1

        if msg_consumed_count >= msg_count:
            break

    consumer_timing = time.time() - consumer_start
    consumer.close()
    return consumer_timing

consumer_timings['confluent_kafka_consumer'] = confluent_kafka_consumer_performance()
calculate_thoughput(consumer_timings['confluent_kafka_consumer'])

# output:
# Processed 1000000 messsages in 4.42 seconds
# 21.60 MB/s
# 226446.46 Msgs/s
