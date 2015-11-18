#webrtc2015大会 *webrtc university课程程序* 流程分析#
---
*written by Alex Stocks on 2015/11/17

> 本篇文章分析的程序实例，是由webrtc2015大会提供的，相关文件被改名以便于闻名会意。之所以对其进行分析，是因为其提供了如下技术代码示例：
>>> 1 webrtc相关技术；
>>
> 2 服务端提供了静态页面服务以及动态请求服务能力；
>>
> 3 浏览器端通过poll（or polling）技术不断的向server端轮询其相关的最新消息。

> 其中第二项能力虽然相当原始，更好的成熟的相关框架有express，第三项能力已被更成熟的websocket+comet取代，但通过分析这个老化石，对知道这些框架或更成熟的技术的优点以及其由来都是大有裨益的。

## 客户端 ##

** 文件列表 **

<font color=blue>

- 1 index.js 启动js
- 2 http-client.js post请求处理者

</font>

### 0 页面加载完成 ###

<font color=blue>

> 当页面加载完成的时候，index.js会在window.onload中调用getMedia()。
> 
> getMedia()函数通过调用getUserMedia完成本地视频显示在localVideo视频框里。


</font>

<font color=green>
	
	function getMedia() {
	  getUserMedia({"audio":true, "video":true},
	               gotUserMedia, didntGetUserMedia);
	}
	
	function gotUserMedia(stream) {
	  myVideoStream = stream;
	  haveLocalMedia = true;
	
	  // display my local video to me
	  attachMediaStream(myVideo, myVideoStream);
	}

</font>

<font color=red>
经测试，这里调用attachMediaIfReady这个函数无效，因为此时pc还没有被创建。
</font>

### 1 connect请求 ###

<font color=blue>

 step 1:  发出connect请求 index.js:connect()->http-client.js:connect

<font color=green>
	  
	  // open XHR and send the connection request with the key
	  var client = new XMLHttpRequest();
	  client.onreadystatechange = handler;
	  client.open("GET", "/connect?key=" + key);
	  client.send();

</font>
  step 2: 当server端返回response时，调用connect::handler

<font color=green>
	
	function handler() {
		// if no error, save status and server-generated id,
		// then start asynchronouse polling for messages
		id = res.id; // 作为当前客户端的id
		status = res.status;
		poll();      // 循环从server拉取客户端的消息
		
		// run user-provided handlers for waiting and connected
		// states
		if (status === "waiting") {  
	      // 如果第一个客户端发出了connect，则server会设置status为waiting
		  waitingHandler();
		} else {                     
		  // 如果第二个客户端发出了connect，则server会设置status为connected
		  connectedHandler();
		}
	}
  
</font>

</font>

#### 1.1 waitingHandler ####

<font color=blue>

> 当客户端调用http-client.js中的相关函数的时候，首先调用index.js:createSignalingChannel(key, handlers)：

<font color=green>

	signalingChannel = createSignalingChannel(key, scHandlers);

</font>

> createSignalingChannel函数对象的首段代码即为:

<font color=green>

	handlers = handlers || {},
	initHandler = function(h) {
	return ((typeof h === 'function') && h) || doNothing;
	},
	waitingHandler = initHandler(handlers.onWaiting),
	connectedHandler = initHandler(handlers.onConnected),
	messageHandler = initHandler(handlers.onMessage);

</font>

> 从上面代码可以知道waitingHandler来自http-client.js:connect()的scHandlers，所以函数对象为connect中一下代码区：

<font color=green>
		
	// handlers for signaling channel
	scHandlers = {
		'onWaiting' : function () {
		  setStatus("Waiting");
		  // weWaited will be used later for auto-call
		  weWaited = true;
		},
		'onConnected': function () {
		  setStatus("Connected");
		  // set up the RTC Peer Connection since we're connected
		  createPC();
		},
		'onMessage': handleMsg
	};

</font>

***waitingHandler***首先通过setStatus修改页面相关控件的显示状态，然后设置weWaited为true。

</font>

#### 1.2 connectedHandler ####

<font color=blue>

connectedHandler通过setStatus修改页面相关控件的显示状态后，调用函数http-client.js:createPC()。

createPC函数的主要代码如下:

<font color=green>
	
	function createPC() {
	  var config = new Array();
	
	  config.push({"url":"stun:stun.l.google.com:19302"});
	  console.log('stunuri:', stunuri, ', turnuri:', turnuri);
	  pc = new RTCPeerConnection({iceServers:config});
	  pc.onicecandidate = onIceCandidate;
	  pc.onaddstream = onRemoteStreamAdded;
	  pc.onremovestream = onRemoteStreamRemoved;
	
	  // wait for local media to be ready
	  attachMediaIfReady();
	}

	// This guard routine effectively synchronizes completion of two
	// async activities:  the creation of the Peer Connection and
	// acquisition of local media.
	function attachMediaIfReady() {
	  // If RTCPeerConnection is ready and we have local media,
	  // proceed.
	  if (pc && haveLocalMedia) {attachMedia();}
	}

	// This routine adds our local media stream to the Peer
	// Connection.  Note that this does not cause any media to flow.
	// All it does is to let the browser know to include this stream
	// in its next SDP description.
	function attachMedia() {
	  pc.addStream(myVideoStream);
	  setStatus("Ready for call");
	}
	
	// When our browser has another candidate, send it to the peer
	function onIceCandidate(e) {
	  if (e.candidate) {
	    send({type:  'candidate',
	          mlineindex:  e.candidate.sdpMLineIndex,
	          candidate:  e.candidate.candidate});
	  }
	}
	
	// When our browser detects that the other side has added the
	// media stream, show it on screen
	function onRemoteStreamAdded(e) {
	  yourVideoStream = e.stream;
	  attachMediaStream(yourVideo, yourVideoStream);
	  setStatus("On call");
	}
	
	// Yes, we do nothing if the remote side removes the stream.
	// This is a *simple* demo, after all.
	function onRemoteStreamRemoved(e) {
	  console.log('onRemoteStreamRemoved(e:', e, ')');
	}

</font>

createPC以及其相关函数的主要任务就是创建PeerConnection并设置ice以及stream回调函数，并通过 <font color=red>**addStream**</font> 加载本地视频。

</font>

### 2 发送并获取音视频请求call ###

<font color=blue>

从下面的代码来看，这个函数主要完成了 <font color=red>**PeerConnection.setLocalDescription()**</font>，其作用是发出本地的SDP(offer)，启动SDP交互流程。

<font color=green>

	// This generates the session description for an offer
	function call() {
	  pc.createOffer(gotDescription, doNothing, constraints);
	}
	
	// In either case, once we get the session description we tell
	// our browser to use it as our local description and then send
	// it to the other browser.  It is the setting of the local
	// description that allows the browser to send media and prepare
	// to receive from the other side.
	function gotDescription(localDesc) {
	  pc.setLocalDescription(localDesc);
	  send(localDesc);
	}

</font>

</font>

### 3 循环获取消息函数poll ###

<font color=blue>

step 1: 生成能调节超时时间的对象pollWaitDelay，具体的算法请参考对象的注释；

step 2: 在IIFE(Imediately-Invoked Function Express)形式的getLoop函数中，调用get函数，发出获取消息的请求；

step 3: 当server返回消息的时候，在getLoop中包含的get的的callback函数会对消息作出处理，首先如果msg不为空且status不为connected则修正status为connected并调用connectedHandler，其次分别处理每条消息，最后通过setTimeout函数循环调用getLoop。

</font>

#### 3.1 handleMessage ####

<font color=blue>

用户点击connect button的时候，页面只会显示本地视频，并且调用poll函数向server端拉取它的消息。用户点击call button的时候，开启SDP流程，poll函数从server端拉取到的消息既有文本消息，也有SDP消息。

对SDP消息的处理，既有下面的answer函数，也有 **1.2** 小节创建PeerConnection时的相关回调函数。

</font>

<font color=green>

	//index.js

	// This is the handler for all messages received on the
	// signaling channel.
	handleMsg = function (msg) {
		// First, we clean up the message and post it on-screen
		var msgE = document.getElementById("inmessages");
		var msgString = JSON.stringify(msg).replace(/\\r\\n/g,'\n');
		msgE.value = msgString + "\n" + msgE.value;
		
		// Then, we take action based on the kind of message
		if (msg.type === "offer") {
		  pc.setRemoteDescription(new RTCSessionDescription(msg));
		  answer();
		} else if (msg.type === "answer") {
		  pc.setRemoteDescription(new RTCSessionDescription(msg));
		} else if (msg.type === "candidate") {
		  pc.addIceCandidate(
		    new RTCIceCandidate({sdpMLineIndex:msg.mlineindex,
		                         candidate:msg.candidate}));
		}
	};

	// handlers for signaling channel
	scHandlers = {
		'onWaiting' : function () {
		  setStatus("Waiting");
		  // weWaited will be used later for auto-call
		  weWaited = true;
		},
		'onConnected': function () {
		  setStatus("Connected");
		  // set up the RTC Peer Connection since we're connected
		  createPC();
		},
		'onMessage': handleMsg
	};

	signalingChannel = createSignalingChannel(key, scHandlers);

	// http-client.js

	handlers = handlers || {},
	initHandler = function(h) {
	return ((typeof h === 'function') && h) || doNothing;
	},
	messageHandler = initHandler(handlers.onMessage);

	function getLoop() {
		get(function (response) {
		  var i, msgs = (response && response.msgs) || [];
		
		  // if messages property exists, then we are connected   
		  if (response.msgs && (status !== "connected")) {
		    // switch status to connected since it is now!
		    status = "connected";
		    connectedHandler();
		  }
		  if (msgs.length > 0) {           // we got messages
		    pollWaitDelay.reset();
		    for (i=0; i<msgs.length; i+=1) {
		      handleMessage(msgs[i]);
		    }
		  } else {                         // didn't get any messages
		    pollWaitDelay.increase();
		  }
		
		  // now set timer to check again
		  setTimeout(getLoop, pollWaitDelay.value());
		});
	}

	// and this generates it for an answer.
	function answer() {
	  pc.createAnswer(gotDescription, doNothing, constraints);
	}

</font>

### 4 请求消息函数get ###

<font color=blue>

请求参数中带上请求者的id，server就会把请求者的所有消息下发下来。

<font color=green>

	var client = new XMLHttpRequest();
	client.onreadystatechange = handler;
	client.open("POST", "/get");
	client.send(JSON.stringify({"id":id}));

</font>

</font>

### 5 发送消息函数send ###

<font color=blue>

step 1: index.js:send首先获取message框中的msg内容，然后把内容放到outmessages中，调用http-client.js的send函数；

step 2: http-client.js:send:

<font color=green>

	// open XHR and send my id and message as JSON string
	var client = new XMLHttpRequest();
	client.onreadystatechange = handler;
	client.open("POST", "/send");
	var sendData = {"id":id, "message":msg};
	client.send(JSON.stringify(sendData));

</font>

</font>


## 服务端 ##

** 文件列表 **

<font color=blue>

- 1 index.js 启动js
- 2 async.js 响应动态的post请求
- 3 http-server.js 静态文本服务器
- 4 config.js 配置文件
- 5 log.js logger

</font>

### 0 静态文本服务器 ###

<font color=blue>

本程序给出了一个很好的静态服务与动态服务同时进行的example，根据browser请求路径的页面是否存在来区分请求的静态或者动态与否。下面的createFilePath函数作用是把用户请求的静态文件的相对路径转换成本地的绝对路径，我对这个函数添加了三行代码，其意是当用户请求的路径是'/'时默认用户请求的文件是index.html.

</font>

<font color=green>
	
	// Creates a handler to collect POSTed data and to route the
	// request based on the path name
	// handle其实是一个路由器句柄
	function start(handle, port) {
	  function onRequest(req, res) {
	    var urldata = url.parse(req.url, true),
	        pathname = urldata.pathname,
	        info = {
	          "res": res,
	          "query": urldata.query,
	          "postData": ""
	        };
	    
	    log("Request for " + pathname + " received");
	    req.setEncoding("utf8");
	    // post请求有接入中和接入完成两种状态， 两种状态对应着两种回调方法
	    req.addListener("data", function (postDataChunk) {
	      info.postData += postDataChunk;
	      // log('info content:', info);
	      log("Received POST data chunk '" + postDataChunk + "'.");
	    });
	    req.addListener("end", function () {
	      route(handle, pathname, info);
		  // log('info content:', info);
	    });
	  }
	  
	  var server = http.createServer(onRequest).listen(port, config.httpServer.host);
	}
	
	// This function adds the serveFilePath to the beginning of the
	// given pathname after removing .., ~, and other such
	// problematic syntax bits from a security perspective.
	// ** There is no claim that this is now secure **
	function createFilePath(pathname) {
	  var filtered = new Array(),
	    temp;
	  
	  if (pathname == '/') {
	    pathname += 'index.html';
	  }
	  var components = pathname.substr(1).split('/');
	  for (var i = 0, len = components.length; i < len; i++) {
	    temp = components[i];
	    if (temp == "..") continue;  // no updir
	    if (temp == "") continue;   // no root
	    temp = temp.replace(/~/g, '');  // no userdir
	    filtered.push(temp);
	  }
	  
	  return (serveFilePath + "/" + filtered.join("/"));
	}

	// Determines whether requested path is a static file or a custom
	// path with its own handler
	function route(handle, pathname, info) {
	  log("About to route a request for " + pathname);
	  // Check if path after leading slash is an existing file that
	  // can be served
	  var filepath = createFilePath(pathname);
	  log("Attempting to locate " + filepath);
	  fs.stat(filepath, function(err, stats) {
	    if (!err && stats.isFile()) {  // serve file
	      // 文件存在，即浏览器请求静态文件
	      serveFile(filepath, info);
	    } else {  // must be custom path
	      // 文件不存在，则进行路由
	      handleCustom(handle, pathname, info);
	    }
	  });
	}
	
	// Opens, reads, and sends to the client the contents of the
	// named file
	// 静态文本服务器
	function serveFile(filepath, info) {
	  var res = info.res,
	      query = info.query;
	
	  log("Serving file " + filepath);
	  fs.open(filepath, 'r', function(err, fd) {
	    if (err) {log(err.message);
	              noHandlerErr(filepath, res);
	              return;}
	    var readBuffer = new Buffer(40480);  // increased buffer size to avoid clipping
	    fs.read(fd, readBuffer, 0, 40480, 0,
	      function(err, readBytes) {
	        if (err) {log(err.message);
	                  fs.close(fd);
	                  noHandlerErr(filepath, res);
	                  return;}
	        log('just read ' + readBytes + ' bytes');
	        if (readBytes > 0) {
	          res.writeHead(200,
	                        {"Content-Type": contentType(filepath)});
	          res.write(
	            addQuery(readBuffer.toString('utf8', 0, readBytes),
	                     query));
	        }
	        res.end();
	      });
	    });
	}

</font>

### 1 响应connect请求 ###

<font color=blue>

async.js有三个比较重要的变量，几乎其所有的逻辑都是围绕这三个变量展开的，下面罗列下三个变量的意义：

 1. connections保存了所有的连接对，它本身是一个二位数组，第一维的index就是浏览器端发起请求时的参数key，第二维则是连接对的双方用户，每个用户的id是服务端收到浏览器的connect请求时其响应函数connect调用newID()分配的； 
 2. partner数组保存了每个用户的信息；
 3. messagesFor数组保存了每个用户应收到的消息，相当于一个message center，当用户通过poll机制取走消息后，messagesFor[receiverID]会被情空。
 

</font>

<font color=green>

	var connections = {}, // key
	    partner = {},     // c2c
	    messagesFor = {};

</font>

<font color=blue>

下面的代码块即为browser的connect请求的server端的响应函数。收到请求后，connect()函数首先给请求者随机地分配一个id，然后根据connections[req.key]的状态给browser分别返回不同的响应。

</font>

<font color=green>
	
	// handle XML HTTP Request to connect using a given key
	function connect(info) {
	  var res = info.res,
	      query = info.query,
	      thisconnection,
	      newID = function() {
	        // create large random number unlikely to be repeated
	        // soon in server's lifetime
	        return Math.floor(Math.random()*1000000000);
	        },
	      // 第一个，返回{'id':random id, 'status':'waiting'}
	      // 第二个，返回{'id':random id, 'status':'connected'}
	      connectFirstParty = function() {
	        if (thisconnection.status == "connected") {
	          // delete pairing and any stored messages
	          delete partner[thisconnection.ids[0]];
	          delete partner[thisconnection.ids[1]];
	          delete messagesFor[thisconnection.ids[0]];
	          delete messagesFor[thisconnection.ids[1]];
	        }
	        connections[query.key] = {};
	        thisconnection = connections[query.key];
	        thisconnection.status = "waiting";
	        thisconnection.ids = [newID()];
	        webrtcResponse({"id":thisconnection.ids[0],
	                        "status":thisconnection.status}, res);
	      },
	      connectSecondParty = function() {
	        thisconnection.ids[1] = newID();
	        partner[thisconnection.ids[0]] = thisconnection.ids[1];
	        partner[thisconnection.ids[1]] = thisconnection.ids[0];
	        messagesFor[thisconnection.ids[0]] = [];
	        messagesFor[thisconnection.ids[1]] = [];
	        thisconnection.status = "connected";
	        webrtcResponse({"id":thisconnection.ids[1],
	                        "status":thisconnection.status}, res);
	      };
	
	  log("Request handler 'connect' was called.");
	  if (query && query.key) {
	    var thisconnection = connections[query.key] ||
	                         {status:"new"};
	    if (thisconnection.status == "waiting") { // first half ready
	      connectSecondParty(); return;
	    } else { // must be new or status of "connected"
	      connectFirstParty(); return;
	    }
	  } else {
	    webrtcError("No recognizable query key", res);
	  }
	}

</font>

#### 1.1 连接对的第一个人的响应函数 connectFirstParty ####

<font color=blue>

 1. 如果connections[req.key].status不为waiting，说明请求者是连接的第一个请求者；
 2. 如果连接的status为connected，则清空connection&partner&messagesFor相应地元素的值；
 3. 设置connections[req.key]的值为{'ids[0]':newID(), 'status':waiting'}；
 4. 给客户端返回{'id':new id, 'status':'waiting'}。

</font>

#### 1.2 连接对的第二个人的响应函数 connectSecondParty ####

<font color=blue>

 1. 如果connections[req.key].status为waiting，说明请求者是连接的第二个请求者；
 2. 设置connections[req.key]的值为{'ids[1]':newID(), 'status':connected'}；
 3. 设置partner[connection[key].ids[0]]的值为connection[key].ids[1]，同时设置partner[connection[key].ids[1]]的值为connection[key].ids[0]，其意是设置每个partner的peer；
 4. 给客户端返回{'id':new id, 'status':'connected'}。

</font>

### 2 响应send请求 ###

<font color=blue>

这个函数就是把message的内容push到messagesFor[peerID]中，然后给发送者返回发送成功消息。

</font>

<font color=green>

	// Queues message in info.postData.message for sending to the
	// partner of the id in info.postData.id
	function sendMessage(info) {
	  var postData = JSON.parse(info.postData),
	      res = info.res;
	  messagesFor[partner[postData.id]].push(postData.message);
	  log("Saving message ***" + postData.message +
	      "*** for delivery to id " + partner[postData.id]);
	  webrtcResponse("Saving message ***" + postData.message +
	                 "*** for delivery to id " +
	                 partner[postData.id], res);
	}

</font>

### 3 响应get请求 ###

<font color=blue>

这个函数就是响应请求者的pull请求，把他的message打包下发下去，同时把messagesFor[req.id]清空。

</font>

<font color=green>
	
	// Returns all messages queued for info.postData.id
	function getMessages(info) {
	  var postData = JSON.parse(info.postData),
	      res = info.res;

	  webrtcResponse({'msgs':messagesFor[postData.id]}, res);
	  messagesFor[postData.id] = [];
	}

</font>










