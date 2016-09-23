/******************************************************
# DESC    :
# AUTHOR  : Alex Stocks
# VERSION : 1.0
# LICENCE : LGPL V3
# EMAIL   : alexstocks@foxmail.com
# MOD     : 2016-09-23 20:22
# FILE    : a.go
******************************************************/

package main

import (
	"fmt"
)

func main() {
	var a = 1
	{
		a = 3
		fmt.Println(a)
	}
	fmt.Println(a)
}
