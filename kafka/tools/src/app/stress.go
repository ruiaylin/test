package main

import (
	"flag"
	"fmt"
	"log"
	"math/rand"
	"os"
	"os/signal"
	"strings"
	"sync"
	"syscall"
	"time"

	"errors"
	"math"
)

import (
	kafka "github.com/AlexStocks/goext/database/kafka"
	alg "github.com/AlexStocks/goext/math/rand"
	"github.com/Shopify/sarama"
)

var (
	zookeeper  = flag.String("zookeeper", "", "A comma-separated Zookeeper connection string (e.g. `zookeeper1.local:2181,zookeeper2.local:2181,zookeeper3.local:2181`)")
	brokerList = flag.String("brokers", "", "The comma separated list of brokers in the Kafka cluster")
	topic      = flag.String("topic", "", "The topic to produce messages to")
	// messageBodySize = flag.Int("message-body-size", 100, "The size of the message payload")
	// waitForAll = flag.Bool("wait-for-all", false, "Whether to wait for all ISR to Ack the message")
	statFrequency = flag.Int("freq", 1000, "How many messages to send in one minute")
	uinNumber     = flag.Int("uin-num", 100, "UIN number")
	totalDuration = flag.Int("run-time", 3, "Total run time in minute")
	printMessage  = flag.Bool("print-msg", false, "Whether print message or not")
	printPacket   = flag.Bool("print-packet", true, "Whether print message or not")
	verbose       = flag.Bool("verbose", false, "Whether to enable Sarama logging")
)

type MessageMetadata struct {
	EnqueuedAt time.Time
}

func (mm *MessageMetadata) Latency() time.Duration {
	return time.Since(mm.EnqueuedAt)
}

func producerConfiguration() *sarama.Config {
	config := sarama.NewConfig()
	config.Producer.Return.Errors = true
	config.Producer.Return.Successes = true

	// if *waitForAll {
	// 	config.Producer.RequiredAcks = sarama.WaitForAll
	// } else {
	// 	config.Producer.RequiredAcks = sarama.WaitForLocal
	// }
	config.Producer.RequiredAcks = sarama.WaitForAll

	return config
}

func genUINArray(min int64, max int64, size int) []int64 {
	var (
		array []int64 = make([]int64, size)
	)

	rand.Seed(time.Now().Unix())
	for i := 0; i < size; i++ {
		array[i] = int64(rand.Intn(int(max-min))) + min
	}

	return array
}

func composeUinPacket(uin []int64, packet []int32) ([]string, error) {
	var (
		uinArrayLen    = len(uin)
		packetArrayLen = len(packet)
		arrayLen       = int(math.Min(float64(uinArrayLen), float64(packetArrayLen)))
	)

	if arrayLen <= 0 {
		return nil, errors.New(fmt.Sprintf("@uin array length:%v, @packet array length:%v",
			uinArrayLen, packetArrayLen))
	}

	var (
		uinPacket = make([]string, arrayLen, arrayLen)
	)
	for idx := 0; idx < arrayLen; idx++ {
		uinPacket[idx] = fmt.Sprintf("%d:%d", uin[idx], packet[idx])
	}

	return uinPacket, nil
}

func main() {
	flag.Parse()

	if *topic == "" {
		panic(fmt.Sprintf("kafka topic is nil"))
	}

	if *verbose {
		sarama.Logger = log.New(os.Stdout, "[sarama] ", log.LstdFlags)
	}

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

	var (
		wg                            sync.WaitGroup
		enqueued, successes, failures int
		totalLatency                  time.Duration
		messageFormat                 string
	)

	messageFormat = `{"bizType":"packet", "bizAction":"InFromClient", "from":"%d", "request_time":"%d"}`
	// messageFormat = `
	//                 {
	//                     "bizType":"packet",
	//                     "bizAction":"InFromClient",
	//                     "from":"%d",
	//                     "request_time":"%d"
	//                 }
	//                 `
	producer, err := sarama.NewAsyncProducer(strings.Split(*brokerList, ","), producerConfiguration())
	if err != nil {
		log.Fatalln("Failed to start producer:", err)
	}

	wg.Add(1)
	go func() {
		defer wg.Done()
		var (
			latency, batchDuration time.Duration
			batchStartedAt         time.Time
			rate                   float64
		)

		batchStartedAt = time.Now()
		for message := range producer.Successes() {
			totalLatency += message.Metadata.(*MessageMetadata).Latency()
			successes++
			// fmt.Println("successes:", successes)

			if successes%*statFrequency == 0 {

				batchDuration = time.Since(batchStartedAt)
				rate = float64(*statFrequency) / (float64(batchDuration) / float64(time.Second))
				latency = totalLatency / time.Duration(*statFrequency)

				log.Printf("Rate: %0.2f/s; latency: %0.2fms\n", rate, float64(latency)/float64(time.Millisecond))

				totalLatency = 0
				batchStartedAt = time.Now()
			}
		}
	}()

	wg.Add(1)
	go func() {
		defer wg.Done()
		for err := range producer.Errors() {
			log.Println("FAILURE:", err)
			failures++
		}
	}()

	// messageBody := sarama.ByteEncoder(make([]byte, *messageBodySize))
	var (
		messageStart     time.Time
		messageDuration  time.Duration
		sleep            int
		secondsOneMinute = 60
		signals          = make(chan os.Signal, 1)

		uinArray    []int64
		packetArray []int32
		uinPacket   []string
	)
	signal.Notify(signals, os.Interrupt, os.Kill, syscall.SIGHUP, syscall.SIGTERM)

	uinArray = genUINArray(1, 1000000, *uinNumber)
	// fmt.Println("uin array:", uinArray)
	log.Println("uin array:", uinArray)
	// ProducerLoop:
	for t := 0; t < *totalDuration; t++ {
		// fmt.Println("t:", t)
		messageStart = time.Now()
		packetArray, _ = alg.CalRedPacket(int32(*statFrequency), int32(*uinNumber))
		// idx := 0
		// uin loop
		// fmt.Println("red packet:", packetArray)
		// log.Println("red packet:", packetArray)
		uinPacket, _ = composeUinPacket(uinArray, packetArray)
		if *printPacket {
			log.Println("red packet:", uinPacket)
		}
		for i := 0; i < *uinNumber; i++ {
			// fmt.Println("i:", i)
			// red packet loop
			for j := 0; j < int(packetArray[i]); j++ {
				// idx++
				// fmt.Println("j:", j)
				message := &sarama.ProducerMessage{
					Topic:    *topic,
					Key:      sarama.StringEncoder(fmt.Sprintf("%d", uinArray[i])),
					Value:    sarama.StringEncoder(fmt.Sprintf(messageFormat, uinArray[i], enqueued)), // idx)), // enqueued)),
					Metadata: &MessageMetadata{EnqueuedAt: time.Now()},
				}
				if *printMessage {
					log.Println("msg:", message.Value)
				}
				// fmt.Println("enqueued:", enqueued, ", i:", i, ", uin:", uinArray[i])
				select {
				case <-signals:
					producer.AsyncClose()
					// break ProducerLoop
					goto END
				case producer.Input() <- message:
					enqueued++
				}
				time.Sleep(time.Duration(10 * time.Millisecond))
			}
		}

		messageDuration = time.Since(messageStart)

		sleep = secondsOneMinute - int(float64(messageDuration)/float64(time.Second))
		// fmt.Println("messageDuration", messageDuration, "-", float64(messageDuration)/float64(time.Second), "sleep secondsOneMinute:", sleep)
		if 0 < sleep && sleep <= secondsOneMinute {
			// log.Println("start to sleep ", sleep, " seconds ......")
			// time.Sleep(time.Duration(time.Duration(sleep) * time.Second))
			// fmt.Println("sleep end")
		LoopSleep:
			for {
				log.Println("start to sleep ", sleep, " seconds ......")
				select {
				case sig := <-signals:
					log.Println("get signal:", sig)
					producer.AsyncClose()
					goto END
				case <-time.After(time.Second * time.Duration(sleep)):
					break LoopSleep
				}
			}
		}
	}

WaitSignalLoop:
	for {
		select {
		case sig := <-signals:
			log.Println("get signal:", sig)
			producer.AsyncClose()
			break WaitSignalLoop
		case <-time.After(time.Second * time.Duration(secondsOneMinute)):
			log.Println("timeout 60s, wait for signal to exit ......")
		}
	}

END:
	log.Println("Waiting for in flight messages to be processed...")
	wg.Wait()

	log.Println()
	log.Printf("Enqueued: %d; Produced: %d; Failed: %d.\n", enqueued, successes, failures)
}
