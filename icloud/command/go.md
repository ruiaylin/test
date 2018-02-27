# go command list #
---
* Author Alex Stocks(2016-09-25 18:13) *
---

* [go command list](#go-command-list)
  * [help](#help)
  * [build](#build)
  * [doc](#doc)
  * [get](#get)
  * [list](#list)
  * [run](#run)
  * [test](#test)
  * [tool](#tool)
  * [refers](#refers)

## help ##
---
- 1 go help build
- 2 go help testflag

## build ##
---
- 1 go build -gcflags "-N -l -m" -o test

     -N 禁止优化, -l(小写的L) 禁用内联, -m 参数用于显示EA优化后的信息, -u 禁用unsafe, -S 输出汇编代码 # go tool compile -help查看gcflags的参数
- 2 go build -x # -x 展示编译详细过程
- 3 go build -gcflags "-B"

	go默认是有数组边界检查,可以关掉.
	ref: http://www.zhihu.com/question/24492033?sort=created
- 4 go build -ldflags "-s -w"

    -s: 去掉符号信息 -w: 去掉DWARF调试信息 # ref: http://studygolang.com/articles/1362
    一个只带有一个"fmt.Println("hello")"语句的程序用各种符号编译后结果:

	2016/09/28  11:32         1,713,664 a.exe   # go build

    2016/09/28  11:36         1,714,688 nl.exe  # go build -gcflags "-N -l"

    2016/09/28  11:36         1,714,688 nlm.exe # go build -gcflags "-N -l -m"

    2016/09/28  11:34         1,075,200 r.exe   # go build -ldflags "-s -w"
    
    可以使用[upx](https://github.com/upx/upx/releases)通过去除符号表进一步压缩bin文件。
    
- 4 GOOS=linux go build -a # 去除编译缓存，从头重新编译
    
## doc ##
---
- 1 go doc fmt
- 2 godoc -http a.go
- 3 godoc -http :6060

    然后在浏览器中访问localhost:6060/pkg，就可以查看本地package的doc。
- 4 go doc net/http.Response.Status # 查看包里面结构体的定义

## fmt ##
---
- 1 gofmt -w -l -r "panic(err) -> log.Error(err)" test.go # fmt的时候进行替换
- 2 diff -u <(echo -n) <(gofmt -d test.go) # 格式化的时候输出其中的错误 

## get ##
---
- 1 go get -u # 强制更新一个包，加上-v可以输出更新过程
- 2 go get -d golang.org/x/oauth2/... # -d = download，仅仅下载包代码，既不编译也不安装
- 3 go get -t # 下载包后对包执行单元测试
- 4 go get ./... # 获取所有的依赖
- 5 go get .  # 获取当前包的最新版

## linter ##
---

golint作用是检查代码风格/错误。

	golint -set_exit_status $(go list ./... | grep -v /vendor/)

注意-set\_exit\_status选项。 默认情况下，golint仅输出样式问题，并带有返回值（带有0返回码），所以CI不认为是出错。 如果指定了-set_exit_status，则在遇到任何样式问题时，golint的返回码将不为0。

## list ##
---
- 1 go list -f # 列出相关package，如
    * go list -f '{{.Deps}}' runtime # 列出package runtime的依赖包
    * go list -f {{.Dir}} crypto/tls # 列出package crypto/tls的绝对路径
    * go list -f {{.Dir}} github.com/AlexStocks/getty
    * go run `go list -f {{.Dir}} crypto/tls`/generate_cert.go  --rsa-bits 1024 --host 127.0.0.1,::1,localhost --ca --start-date "Jan 1 00:00:00 1970" --duration=1000000h
- 2 查找含有某个字符串的包名
    go list all | grep http # 列出GOPATH内所有包
    go list std | grep http # 列出go/src/内所有包
- 3 查找golang命令所在的包
    go list cmd
- 4 go list -f '{{.GoFiles}}' os/exec # 这个命令会给出在当前平台上使用这个包进行编译时将会被编译的文件
- 5 go list ./...  # 列出包的方法

## run ##
---
- 1 查看函数那些变量逃逸到了heap上
    go run -gcflags '-m -l' escape.go
    ./escape.go:6: moved to heap: x
    ./escape.go:7: &x escape to heap
    ./escape.go:11: bar new(int) does not escape
    
- 2 查看逃逸的具体过程
   
   		// ref: https://segment.com/blog/allocation-efficiency-in-high-performance-go-services/
   		// go build -gcflags '-m -m' ./main.go
   	 	// main.go
	    package main

	 	import "fmt"

		func main() {
    		x := 42
    		fmt.Println(x) // Print系列函数会导致其使用的参数逃逸
    		fmt.Println(43)
		}



	

## test ##
---
- 1 go test -v -file wechat_test.go wechat.go # 测试单个文件，一定要带上被测试的原文件, "-file"参数不是必须的，可以省略

    注意go1.9之后-test.file这个参数已经没有了，所以是一定得省略。

- 2 go test -v -test.run TestRefreshAccessToken # 测试单个方法

    如果多个函数都以TestRefreshAccessToken作为前缀，则所有这些单测都会被执行，可以通过这个方法 "TestRefreshAccessToken$" 只执行TestRefreshAccessToken测试。
    
    go test -v -run Gops # 测试ExampleGops

- 3 -test.bench pattern # Run benchmarks matching the regular expression.  By default, no benchmarks run.

    -test.run pattern   # Run only those tests and examples matching the regular expression. For convenience, each of these -test.X flags of the test binary is also available as the flag -X in 'go test' itself. 也就是说-test.bench = -bench, -test.run = -run.

    go test -test.bench MapTravel -test.run XXX  # go test -bench MapTravel -run XXX

- 4 go test -bench=".*" -cpuprofile=cpu.prof -c 
	* cpuprofile是表示生成的cpu profile文件
	* -c是生成可执行的二进制文件， 这个是生成状态图必须的， 它会在本目录下生成可执行文件mysql.test,

- 5 go tool pprof mysql.test cpu.prof 
   * 然后使用go tool pprof工具, 调用web（需要安装graphviz）来生成svg文件,生成后使用浏览器查看svg文件
- 6 go test -race # 显示竞争条件
- 7 go test -coverprofile=c.out && go tool cover -html=c.out 
   *  把测试覆盖率输入进一个文件，cover工具会把结果显示在浏览器中
- 8 go test ./... # 运行所有的单测
- 9 go test -bench=Fib40 -benchtime=20s
- 10 go test -bench=. -run=^a  
     > To run only benchmarks，如果没有run后面的相关参数则go test会把Test和Benchmark相关函数都执行一遍*
     * go test -v -bench BenchmarkAsyncProducer -run=^a 
       > just run BenchmarkAsyncProducer
     * go test -v -bench AsyncProducer$ -run=^a 
     
       > just run BenchmarkAsyncProducer
       
       > 上面参数中如果不添加-run，则所有的Test单测也会被执行，但是指定的-run=^a指定的Test单测函数不存在，所以只有bench相关函数被执行。
- 11 如果单测文件中有flag变量，则可以在go test命令上加上相关变量的值，如httptest有如下用法:
		
		// go/src/net/http/httptest/server.go
		// go test -run=BrokenTest -httptest.serve=127.0.0.1:8000
		var serve = flag.String("httptest.serve", "", "if non-empty, httptest.NewServer serves on this address and blocks")
- 12 go test -v hello -run ^TestHello_World$   
      * 执行测试不必在测试代码所在的目录，在任何地方都可以通过上面的方式执行某个package(Hello)的单测函数(TestHello_World)。
- 13 cover

		PKG_LIST=$(go list ./… | grep -v /vendor/)
		for package in ${PKG_LIST}; do
           go test -covermode=count -coverprofile "cover/${package##*/}.cov" "$package" ;
		done
		tail -q -n +2 cover/*.cov >> cover/coverage.cov
		go tool cover -func=cover/coverage.cov
		go tool cover -html=cover/coverage.cov -o coverage.html
		
    上面脚本最后一步是获得HTML格式的覆盖率报告。
- 14 内存检查

	clang有一个未初始化的内存检测器，称为MemorySanitizer。go测试工具能很好地与Clang模块进行交互(只要在linux / amd64主机上，并使用最新版本的Clang / LLVM(> = 3.8.0)。
	
		go test -msan -short $(go list . /…| grep - v /vendor/)  

- 15 一步执行所有的单测

     每个.go文件需要一个能支持单元测试的_test.go文件。可以使用以下命令运行某个工程下所有包的测试:   
     
		go test -short $(go list ./… | grep -v /vendor/) 
   
## tool ##
---
- 1 go tool compile -help # 显示"go build -gcflags"可用的flags
- 2 go tool objdump -s "main\.main" ./test # 输出符号表
- 3 go tool vet -shadow # 查看是否有同名变量冲突
- 4 go tool dist list   # 列出go所支持的系统平台
- 5 go tool pprof http://161.202.204.177:36379/debug/pprof/profile # 列出pprof
- 6 go tool pprof -inuse_space http://127.0.0.1:8080/debug/pprof/heap # 可以得到一个交互式的命令行，查看正在使用(inuse_space)的一些内存相关信息
    * top10来查看正在使用的对象较多的10个函数入口，top后可以跟任意数字
    * top -cum来查找调用链，-cum的意思就是，将函数调用关系中的数据进行累积，比如A函数调用的B函数，则B函数中的内存分配量也会累积到A上面，这样就可以很容易的找出调用链。cum一列值是累积值，上下两个调用函数的差值就是调用者自己占用的内存
    * web，我们的浏览器弹出一个.svg图片，显示了这些累积关系画成一个拓扑图，途中两个百分比的意义为：(自身分配的内存 of 该函数累积分配的内存)
- 7 go tool pprof -inuse_space -cum -svg http://127.0.0.1:8888/debug/pprof/heap >heap.svg # 获取内存使用图和调用链
    * 用--inuse_space来分析程序常驻内存的占用情况，另有一个alloc_space则是分析内存的临时分配情况
    * 用--alloc_objects来分析内存的临时分配情况，可以提高程序的运行速度
    * --inuse/alloc_space --inuse/alloc_objects
- 8 go-torch -inuse_space http://127.0.0.1:8080/debug/pprof/heap --colors=mem

    go-torch -alloc_space http://127.0.0.1:8080/debug/pprof/heap --colors=mem
     
    go-torch -t 5 -u localhost:9808  # 对占用9808端口的服务端进程制作cpu火焰图
    
    * 火焰图就像一个山脉的截面图，从下而上是每个函数的调用栈，因此山的高度跟函数调用的深度正相关，而山的宽度跟使用/分配内存的数量成正比。我们只需要留意那些宽而平的山顶，这些部分通常是我们需要优化的地方
    
- 9 go1.10之后的pprof  
    go test -run=^$ -bench=. -cpuprofile=profile.out  
    go tool pprof -http=:8080 profile.out  
    pprof -http=:8080 profile.out  # pprof来自go get -u github.com/google/pprof

    go1.10的pprof增加了Web UI，可以直接看火焰图和调用关系图。

## refers ##
---
- 1 [go fix与go tool fix](http://wiki.jikexueyuan.com/project/go-command-tutorial/0.10.html)
- 2 [Go 1.10中值得关注的几个变化](https://tonybai.com/2018/02/17/some-changes-in-go-1-10/)
