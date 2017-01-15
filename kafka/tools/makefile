.PHONY: all dep dir bin test clean show
.SUFFIXES: .go

SRC_DIR		:= src
OUTPUT_DIR	:= release

DEBUG		:= true
BUILD		:= go build
CLEAN		:= go clean
RUN			:= go run
INSTALL 	:= go install

ifeq ($(DEBUG), true)
	#GFLAGS  := -gcflags "-N -l" -race -x -v # -x会把go build的详细过程输出
	GFLAGS  := -gcflags "-N -l" -race -v
	OUTPUT_DIR := debug
else
	GFLAGS  := -ldflags "-s"
	OUTPUT_DIR := release
endif

BIN			:= stress consumer producer
all: clean dep dir $(BIN)

dep:
	export GOPATH=${PWD}:${GOPATH}

dir:
	@-mkdir -p $(OUTPUT_DIR)

consumer:
	export GOPATH=${PWD}:${GOPATH} && cd $(SRC_DIR) && $(BUILD) $(GFLAGS) -o ../$(OUTPUT_DIR)/$@ app/consumer.go

producer:
	export GOPATH=${PWD}:${GOPATH} && cd $(SRC_DIR) && $(BUILD) $(GFLAGS) -o ../$(OUTPUT_DIR)/$@ app/producer.go

stress:
	export GOPATH=${PWD}:${GOPATH} && cd $(SRC_DIR) && $(BUILD) $(GFLAGS) -o ../$(OUTPUT_DIR)/$@ app/stress.go

clean:
	-$(CLEAN)
	-rm -rf $(OUTPUT_DIR)

install:
	-$(INSTALL)

show:
	@-echo $(OUTPUT_DIR)

testc:
	$(OUTPUT_DIR)/consumer -group=test -zookeeper=127.0.0.1:2181/kafka -topics=test1

# 默认情况下，Kafka根据传递消息的key来进行分区的分配，即hash(key) % numPartitions,默认情况下，
# Kafka根据传递消息的key来进行分区的分配，即hash(key) % numPartitions,Kafka几乎就是随机找一个分区发送无key的消息，
# 然后把这个分区号加入到缓存中以备后面直接使用
testp:
	$(OUTPUT_DIR)/producer -brokers=116.211.15.191:9091,116.211.15.191:9092,116.211.15.191:9093,116.211.15.191:9094 --topic=freak-topic --verbose=true --partitioner=hash --key='nihao' --value='fuck'

testpa:
	$(OUTPUT_DIR)/producer -zookeeper=127.0.0.1:2181/kafka --topic=test1 --verbose=true --partitioner=hash --key='nihao' --value=‘value0’

tests:
	$(OUTPUT_DIR)/stress -zookeeper=116.211.15.190:2181 -topic=freak-topic -freq=100  -uin-num=10 -run-time=2 -verbose=true

