// Copyright 2013-2014 Digital Codex LLC
// You may use this code for your own education.  If you use it
// largely intact, or develop something from it, don't claim
// that your code came first.  You are using this code completely
// at your own risk.  If you rely on it to work in any particular
// way, you're an idiot and we won't be held responsible.

var server = require('./http-server');
var async = require('./async');
var log = require('./log').log;
var config = require('./config');
var port = process.argv[2] || config.httpServer.port;

// returns 404
function fourohfour(info) {
  var res = info.res;
  log('Request handler fourohfour was called.');
  res.writeHead(404, {'Content-Type': 'text/plain'});
  res.write('404 Page Not Found');
  res.end();
}

var handle = {};
handle['/'] = fourohfour;
handle['/connect'] = async.connect;
handle['/send'] = async.send;
handle['/get'] = async.get;

server.serveFilePath('static');
server.start(handle, port);
