
  const zmq = require("zeromq");
  const sock = zmq.socket("pull");

  sock.connect("tcp://192.168.178.23:80");
  console.log("Worker connected to port 3000");

  sock.on("message", function(msg) {
    console.log("work: %s", msg.toString());
  });
