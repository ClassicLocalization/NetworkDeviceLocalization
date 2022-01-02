const zmq = require("zeromq");

const sock = zmq.socket("pull");

sock.bindSync("tcp://192.168.178.26:3001");
console.log("Server bound to port 3001");

sock.on("message", function(msg) {
    console.log("work: %s", msg.toString());
  });