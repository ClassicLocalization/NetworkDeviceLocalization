const zmq = require(`zeromq`);
const socket = zmq.socket(`sub`);  // PUB socket

socket.connect(`tcp://127.0.0.1:3000`);      // Connect to port 3000
 
socket.subscribe(`heartbeat`);               // Subscribe to 'heartbeat'
 
socket.on(`message`, function (topic, msg) {
	console.log(`Received: ${msg} for ${topic}`);
});