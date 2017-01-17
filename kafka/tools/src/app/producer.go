/******************************************************
# DESC    : kafka message producer
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
	"strings"
	// "encoding/json"
)

import (
	kafka "github.com/AlexStocks/goext/database/kafka"
	"github.com/Shopify/sarama"
)

var (
	platform    = flag.String("platform", "i", "os platform (e.g. `i` = iOS, `a` = Android)")
	zookeeper   = flag.String("zookeeper", "", "A comma-separated Zookeeper connection string (e.g. `zookeeper1.local:2181,zookeeper2.local:2181,zookeeper3.local:2181`)")
	brokerList  = flag.String("brokers", "", "The comma separated list of brokers in the Kafka cluster")
	topic       = flag.String("topic", "", "The topic to produce to")
	partitioner = flag.String("partitioner", "hash", "The partitioning scheme to use. Can be `hash`, or `random`")
	waitForAll  = flag.Bool("wait-for-all", false, "Whether to wait for all ISR to Ack the message")
	key         = flag.String("key", "", "The key of the message to produce")
	value       = flag.String("value", "", "The value of the message to produce")
	verbose     = flag.Bool("verbose", false, "Whether to turn on sarama logging")

	logger = log.New(os.Stderr, "", log.LstdFlags)
)

func tool() {
	if *verbose {
		sarama.Logger = logger
	}

	// fmt.Println(*zookeeper, *brokerList)
	if *zookeeper != "" && *brokerList == "" {
		brokerArray, err := kafka.GetBrokerList(*zookeeper)
		if err != nil {
			panic(fmt.Sprintf("kafka.GetBrokerList(zookeeper{%v}) = error{%v}", *zookeeper, err))
		}
		*brokerList = strings.Join(brokerArray, ",")
		fmt.Println("zookeeper host:", *zookeeper, ", kafka broker list:", *brokerList)
	}

	if *brokerList == "" {
		panic(fmt.Sprintf("Failed to initialize Kafka producer, brokerList is nil"))
	}

	producer, err := kafka.NewProducer(*brokerList, *partitioner, *waitForAll)
	if err != nil {
		panic(fmt.Sprintf("Failed to initialize Kafka producer: %v", err))
	}
	defer producer.Close()

	partition, offset, err := producer.SendMessage(*topic, *key, *value)
	if err != nil {
		logger.Println("FAILED to produce message:", err)
	} else {
		fmt.Printf("topic=%s\tpartition=%d\toffset=%d\n", *topic, partition, offset)
	}
}

func gcm_producer() {
	if *verbose {
		sarama.Logger = logger
	}

	*zookeeper = "127.0.0.1:2181/kafka"
	*topic = "test1"
	*verbose = true
	*partitioner = "hash"
	*key = "nihao"
	*value = `{
		"os":2,
		"app_id":1,
		"token": "cJAlmwiffkA:APA91bFiYUle7ETJLnSuPyDTnPgIs3kZg8-QCBuh1DV6WIlDSWJhAXd47LD9GMkuicxhqCNQSwsUqUbDhsvJmqHA_aODD8dD91akDioJ3PJLxKBaYX703z46mB-sge-gBT-A04AxHdoM",
		"payload": "{\"message\": {\"content\": \"gcm-content\", \"title\": \"gcm-title\"}, \"open_url\": \"local://live?room_id=u4850311483077144030001222&show_id=7814842859\", \"extra\": { \"type\": \"uid\", \"id\": \"a4d82062-d952-11e6-801b-002590f3b4ae\" }}"
	}`

	// fmt.Println(*zookeeper, *brokerList)
	if *zookeeper != "" && *brokerList == "" {
		brokerArray, err := kafka.GetBrokerList(*zookeeper)
		if err != nil {
			panic(fmt.Sprintf("kafka.GetBrokerList(zookeeper{%v}) = error{%v}", *zookeeper, err))
		}
		*brokerList = strings.Join(brokerArray, ",")
		fmt.Println("zookeeper host:", *zookeeper, ", kafka broker list:", *brokerList)
	}
	if *brokerList == "" {
		panic(fmt.Sprintf("Failed to initialize Kafka producer, brokerList is nil"))
	}
	producer, err := kafka.NewProducer(*brokerList, *partitioner, *waitForAll)
	if err != nil {
		panic(fmt.Sprintf("Failed to initialize Kafka producer: %v", err))
	}
	defer producer.Close()

	partition, offset, err := producer.SendBytes(*topic, []byte(*key), []byte(*value))
	if err != nil {
		logger.Println("FAILED to produce message:", err)
	} else {
		fmt.Printf("topic=%s\tpartition=%d\toffset=%d\n", *topic, partition, offset)
	}
}

func apns_producer() {
	if *verbose {
		sarama.Logger = logger
	}

	*zookeeper = "127.0.0.1:2181/kafka"
	*topic = "test1"
	*verbose = true
	*partitioner = "hash"
	*key = "nihao"
	*value = `{
		"plan_id": 17,
		"os": 1,
		"app_id": 1,
		"token": "ffe3cd80cfc44fed3faaeef11a21593e5e9b0bbb3f05a6fdaaf9aa1794f2e782",
		"payload": "{\"aps\": {\"sound\": \"\", \"badge\": 1, \"alert\": \"apns-alert\"}, \"open_url\": \"local://live?room_id=u4850311483077144030001222&show_id=7814842859\", \"extra\": {\"type\": \"uid\", \"id\": \"a4d82062-d952-11e6-801b-002590f3b4ae\"}}"
	}`

	// *value = `{
	// 	"plan_id": 17,
	// 	"os": 1,
	// 	"app_id": 1,
	// 	"token": "ffe3cd80cfc44fed3faaeef11a21593e5e9b0bbb3f05a6fdaaf9aa1794f2e782",
	// 	"payload": "payload"
	// }`

	// fmt.Println(*zookeeper, *brokerList)
	if *zookeeper != "" && *brokerList == "" {
		brokerArray, err := kafka.GetBrokerList(*zookeeper)
		if err != nil {
			panic(fmt.Sprintf("kafka.GetBrokerList(zookeeper{%v}) = error{%v}", *zookeeper, err))
		}
		*brokerList = strings.Join(brokerArray, ",")
		fmt.Println("zookeeper host:", *zookeeper, ", kafka broker list:", *brokerList)
	}

	if *brokerList == "" {
		panic(fmt.Sprintf("Failed to initialize Kafka producer, brokerList is nil"))
	}

	producer, err := kafka.NewProducer(*brokerList, *partitioner, *waitForAll)
	if err != nil {
		panic(fmt.Sprintf("Failed to initialize Kafka producer: %v", err))
	}
	defer producer.Close()

	partition, offset, err := producer.SendBytes(*topic, []byte(*key), []byte(*value))
	if err != nil {
		logger.Println("FAILED to produce message:", err)
	} else {
		fmt.Printf("topic=%s\tpartition=%d\toffset=%d\n", *topic, partition, offset)
	}
}

func main() {
	flag.StringVar(platform, "p", "i", "os platform (e.g. `i` = iOS, `a` = Android)")
	flag.Parse()

	switch *platform {
	case "i":
		apns_producer()
	case "a":
		gcm_producer()
	default:
		tool()
	}
}
