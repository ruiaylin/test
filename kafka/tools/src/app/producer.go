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
)

import (
	kafka "github.com/AlexStocks/goext/database/kafka"
	"github.com/Shopify/sarama"
)

var (
	pf          = flag.String("platform", "i", "os platform (e.g. `i` = iOS, `a` = Android)")
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
		"token": "fZAFJfdiqjw:APA91bGIpdPyt6oUmTR2dcgx7dwGeN212Bc8VSJdtheeXkfI8oQmh6OtEKr9y7Yo0pRzI--ydShJYesemOTjrh4omN3dPsGS7Ir1mjiA2oONhBDpjyvdiAInhVb5w-d9_oUaIj3D1UjW",
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

	partition, offset, err := producer.SendMessage(*topic, *key, *value)
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
		"os": 1,
		"app_id": 1,
		"token": "3e26927c4fa4b10099483deb86182bdcaa42da0f104dc68f1d00b65e776d29c4",
		"payload": "{\"aps\": {\"sound\": \"\", \"badge\": 1, \"alert\": \"apns-alert\"}, \"open_url\": \"local://live?room_id=u4850311483077144030001222&show_id=7814842859\", \"extra\": {\"type\": \"uid\", \"id\": \"a4d82062-d952-11e6-801b-002590f3b4ae\"}}"
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

	partition, offset, err := producer.SendMessage(*topic, *key, *value)
	if err != nil {
		logger.Println("FAILED to produce message:", err)
	} else {
		fmt.Printf("topic=%s\tpartition=%d\toffset=%d\n", *topic, partition, offset)
	}
}

func main() {
	flag.Parse()

	switch *pf {
	case "i":
		apns_producer()
	case "a":
		gcm_producer()
	default:
		tool()
	}
}
