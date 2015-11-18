var signalingChannel, key, id,
    haveLocalMedia = false,
    weWaited = false,
    myVideoStream, myVideo, 
    yourVideoStream, yourVideo,
    doNothing = function() {},
    pc,
    constraints = {mandatory: {
                    OfferToReceiveAudio: true,
                    OfferToReceiveVideo: true}};

////////////////////////////
// This is the main routine.
////////////////////////////

// This kicks off acquisition of local media.  Also, it can
// automatically start the signaling channel.
window.onload = function () {
  // auto-connect signaling channel if key provided in URI
  if (queryparams && queryparams['key']) {
    document.getElementById("key").value = queryparams['key'];
    connect();
  }

  myVideo = document.getElementById("myVideo");
  yourVideo = document.getElementById("yourVideo");
  
  getMedia();

  //  connect() calls createPC() when connected.
  //  attachMedia() is called when both createPC() and getMedia()
  //  have succeeded.
};


/////////////////////
// This next section is for setting up the signaling channel.
/////////////////////

// This routine connects to the web server and sets up the
// signaling channel.  It is called either automatically on doc
// load or when the user clicks on the "Connect" button.
function connect() {
  var errorCB, scHandlers, handleMsg;

  // First, get the key used to connect
  key = document.getElementById("key").value;

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

  // Finally, create signaling channel
  signalingChannel = createSignalingChannel(key, scHandlers);
  errorCB = function (msg) {
    document.getElementById("response").innerHTML = msg;
  };

  // and connect.
  signalingChannel.connect(errorCB);
}


// This routine sends a message on the signaling channel, either
// by explicit call or by the user clicking on the Send button.
function send(msg) {
  var handler = function (res) {
    document.getElementById("response").innerHTML = res;
    return;
  },

  // Get message if not passed in
  msg = msg || document.getElementById("message").value;

  // Clean it up and post it on-screen
  msgE = document.getElementById("outmessages");
  var msgString = JSON.stringify(msg).replace(/\\r\\n/g,'\n');
  msgE.value = msgString + "\n" + msgE.value;

  // and send on signaling channel
  signalingChannel.send(msg, handler);
}


/////////////////////////////
// This next section is for getting local media
/////////////////////////////

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

function didntGetUserMedia() {
  console.log("couldn't get video");
}

//////////////////////////////
// This next section is for setting up the RTC Peer Connection
//////////////////////////////

function createPC() {
  var stunuri = true,
      turnuri = false,
      myfalse = function(v) {
                  return ((v==="0")||(v==="false")||(!v)); },
      config = new Array();

  // adjust config string based on any query params
  if (queryparams) {
    if ('stunuri' in queryparams) {
      stunuri = !myfalse(queryparams['stunuri']);
    }
    if ('turnuri' in queryparams) {
      turnuri = !myfalse(queryparams['turnuri']);
    };
  };

  if (stunuri) {
    // this is one of Google's public STUN servers
    config.push({"url":"stun:stun.l.google.com:19302"});
  }
  console.log('stunuri:', stunuri, ', turnuri:', turnuri);

  if (turnuri) {
    if (stunuri) {
      // can't use TURN-only TURN server in this case because of
      // bug in Chrome that causes STUN server responses to be
      // ignored, so we use TURN server that also does STUN
        config.push({"urls":"turn:47.88.1.81", "username": "u1",
                     "credential":"u1"});                   
    } else {
      // this is our TURN-only TURN server
      config.push({"urls":"turn:47.88.1.81", "username": "u1",
                   "credential":"u1"});
    }
  }
  console.log("config = " + JSON.stringify(config));

  pc = new RTCPeerConnection({iceServers:config});
  pc.onicecandidate = onIceCandidate;
  pc.onaddstream = onRemoteStreamAdded;
  pc.onremovestream = onRemoteStreamRemoved;

  // wait for local media to be ready
  attachMediaIfReady();
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


///////////////////////////////////
// This next section is for attaching local media to the Peer
// Connection.
///////////////////////////////////

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


////////////////////////////
// This next section is for calling and answering
////////////////////////////

// This generates the session description for an offer
function call() {
  console.log('call()');
  pc.createOffer(gotDescription, doNothing, constraints);
}

// and this generates it for an answer.
function answer() {
  pc.createAnswer(gotDescription, doNothing, constraints);
}

// In either case, once we get the session description we tell
// our browser to use it as our local description and then send
// it to the other browser.  It is the setting of the local
// description that allows the browser to send media and prepare
// to receive from the other side.
function gotDescription(localDesc) {
  pc.setLocalDescription(localDesc);
  console.log('gotDescrition send localDesc');
  send(localDesc);
}


////////////////////////////////////
// This section is for changing the UI based on application
// progress.
////////////////////////////////////

// This function hides, displays, and fills various UI elements
// to give the user some idea of how the browser is progressing
// at setting up the signaling channel, getting local media,
// creating the peer connection, and actually connecting
// media (calling).
function setStatus(str) {
  var statuslineE = document.getElementById("statusline"),
      statusE = document.getElementById("status"),
      sendE = document.getElementById("send"),
      connectE = document.getElementById("connect"),
      callE = document.getElementById("call"),
      scMessageE = document.getElementById("scMessage");

  switch (str) {
    case 'Waiting':
      statuslineE.style.display = "inline";
      statusE.innerHTML =
        "Waiting for peer signaling connection";
      sendE.style.display = "none";
      connectE.style.display = "none";
      break;
    case 'Connected':
      statuslineE.style.display = "inline";
      statusE.innerHTML =
        "Peer signaling connected, waiting for local media";
      sendE.style.display = "inline";
      connectE.style.display = "none";
      scMessageE.style.display = "inline-block";
      break;
    case 'Ready for call':
      statusE.innerHTML = "Ready for call";
      callE.style.display = "inline";
      break;
    case 'On call':
      statusE.innerHTML = "On call";
      callE.style.display = "none";
      break;
    default:
  }
}

