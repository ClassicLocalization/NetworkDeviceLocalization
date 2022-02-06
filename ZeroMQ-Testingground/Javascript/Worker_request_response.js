//https://codeblog.dotsandbrackets.com/inter-service-messaging-with-zeromq-node-js/
const zmq = require('zeromq');
const socket = zmq.socket('req');

socket.connect(`tcp://127.0.0.1:3000`);             // Same address
console.log("Server bound to port 3000");
 
var counter = 0;                                 // Message counter
 
socket.on(`message`, function (msg) {            // When receive message
	console.log(`Response received: "${msg}"`);
	setTimeout(sendMessage, 2000);               // Schedule next request
});
 
sendMessage();
 
function sendMessage () {
	const msg = `MSG #${counter++}`;
	console.log(`Sending ${msg}`);
	socket.send(msg);                            // Send request
}