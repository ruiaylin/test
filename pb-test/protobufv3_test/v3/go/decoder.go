package main

import (
	"fmt"
	"github.com/golang/protobuf/proto"
	"io/ioutil"
	"os"
	"proto3_proto"
)

func main() {
	ma := &proto3_proto.MessageArray{}
	var fileName string = "../../pb_bin_v3"
	if 2 == len(os.Args) {
		fileName = os.Args[1]
	}

	data, _ := ioutil.ReadFile(fileName)
	proto.Unmarshal(data, ma)

	msg_map := ma.GetMsgMap()
	fmt.Println("MessageArray.msg_map size:", len(msg_map))
	for k, v := range msg_map {
		fmt.Println("key:", k)
		fmt.Println("  msg.name:", v.Name)
		fmt.Println("  msg.hilarity:", v.Hilarity)
		fmt.Println("  msg.height_in_cm:", v.HeightInCm)
		fmt.Println("  msg.data:", v.Data)
		fmt.Println("  msg.result_data:", v.ResultCount)
		fmt.Println("  msg.true_scotsman:", v.TrueScotsman)
		fmt.Println("  msg.score:", v.Score)
		fmt.Println("  msg.score num:", len(v.Key))
		for index, value := range v.Key {
			fmt.Println("  msg.score[", index, "]:", value)
		}
	}
	fmt.Println("primary data:", ma.String())
}
