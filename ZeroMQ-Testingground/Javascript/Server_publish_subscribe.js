const zmq = require(`zeromq`);
const socket = zmq.socket(`pub`);  // PUB socket
 
socket.bindSync(`tcp://127.0.0.1:3000`);
 
const topic = `heartbeat`;
 
setInterval(function () {
	const timestamp = Date.now().toString();
	socket.send([topic, timestamp]);          // Publish timestamp
}, 2000);                                     // every two seconds
                                              // in 'heartbeat' topic