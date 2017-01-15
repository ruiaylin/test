/******************************************************
# DESC    : kafka consumer
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : LGPL V3
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2016-05-31 22:06
# FILE    : consumer.go
******************************************************/

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
)

import (
	kafka "github.com/AlexStocks/goext/database/kafka"
	"github.com/Shopify/sarama"
)

const (
	DefaultKafkaTopics   = "test_topic"
	DefaultConsumerGroup = "consumer_example.go"
)

var (
	consumerGroup  = flag.String("group", DefaultConsumerGroup, "The name of the consumer group, used for coordination and load balancing")
	kafkaTopicsCSV = flag.String("topics", DefaultKafkaTopics, "The comma-separated list of topics to consume")
	zookeeper      = flag.String("zookeeper", "", "A comma-separated Zookeeper connection string (e.g. `zookeeper1.local:2181,zookeeper2.local:2181,zookeeper3.local:2181`)")

	zookeeperNodes []string
)

func init() {
	sarama.Logger = log.New(os.Stdout, "[Sarama] ", log.LstdFlags)
}

func main() {
	flag.Parse()

	if *zookeeper == "" {
		flag.PrintDefaults()
		os.Exit(1)
	}

	var consumer kafka.Consumer
	consumer, err := kafka.NewConsumer(*consumerGroup, *kafkaTopicsCSV, *zookeeper)
	if err != nil {
		panic(fmt.Sprintf("Failed to initialize Kafka consumer: %v", err))
	}

	err = consumer.Start()
	if err != nil {
		panic(fmt.Sprintf("Failed to start Kafka consumer: %v", err))
	}
}
