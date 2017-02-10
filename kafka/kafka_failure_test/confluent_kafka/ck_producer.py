#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# ******************************************************
# DESC    : calculate kafka producer throughout by confluent-kafka-python
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : Apache License 2.0
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2016-12-20 21:03
# FILE    : confluent_producer.py
# ******************************************************

import time

import confluent_kafka

from calculator import *

def confluent_kafka_producer_performance():
    conf = {'bootstrap.servers': bootstrap_servers}
    producer = confluent_kafka.Producer(**conf)
    messages_to_retry = 0

    producer_start = time.time()
    for i in range(msg_count):
        try:
            time.sleep(3)
            msg_payload=bytes(str(i))
            producer.produce(topic, value=msg_payload)
            print i
        except BufferError as e:
            messages_to_retry += 1

    # hacky retry messages that over filled the local buffer
    for i in range(messages_to_retry):
        producer.poll(0)
        try:
            time.sleep(3)
            producer.produce(topic, key=str(i), value=msg_payload)
            # producer.produce(topic, value=msg_payload)
            print i
        except BufferError as e:
            # producer.poll(0)
            producer.flush()
            producer.produce(topic, value=msg_payload)

    producer.flush()

    return time.time() - producer_start

producer_timings['confluent_kafka_producer'] = confluent_kafka_producer_performance()
calculate_thoughput(producer_timings['confluent_kafka_producer'])

# output:
# Processed 1000000 messsages in 3.67 seconds
# 25.98 MB/s
# 272425.81 Msgs/s
