package main

import (
	"fmt"
	"github.com/golang/protobuf/proto"
	"io/ioutil"
	"proto3_proto"
)

func main() {
	var ma proto3_proto.MessageArray
	ma.MsgMap = make(map[string]*proto3_proto.Message)
	for idx := 0; idx < 10; idx++ {
		var msg proto3_proto.Message
		msg.Name = fmt.Sprintf("%s%d", "hello", idx)
		msg.Hilarity = proto3_proto.Message_PUNS
		msg.HeightInCm = 123456
		msg.Data = []byte("world")
		msg.ResultCount = 123456
		msg.TrueScotsman = true
		msg.Score = 123.456
		msg.Key = make([]uint64, 100)
		for index := 0; index < 96; index++ {
			msg.Key[index] = uint64(index)
		}

		ma.MsgMap[msg.Name] = &msg
	}

	data, err := proto.Marshal(&ma)
	if err != nil {
		fmt.Println("marshaling error: ", err)
	}
	//fmt.Println("data:", string(data))

	ioutil.WriteFile("../../go_pb_bin_v3", data, 0666)
}
