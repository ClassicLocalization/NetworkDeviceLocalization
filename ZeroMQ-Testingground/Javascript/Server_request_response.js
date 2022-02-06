//https://codeblog.dotsandbrackets.com/inter-service-messaging-with-zeromq-node-js/
const zmq = require('zeromq');
const socket = zmq.socket('rep');

socket.bindSync(`tcp://127.0.0.1:3000`);      // listening at ..1:3000
console.log("Server bound to port 3000");
 
socket.on(`message`, function (msg) {         // on message
	console.log(`Received '${msg}'. Responding...`);
	socket.send(`Responding to ${msg}`);      // send something back
});