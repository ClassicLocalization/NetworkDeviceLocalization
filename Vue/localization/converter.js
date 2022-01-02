// Get your libraries
var io = require('socket.io')(4000);
var zmq = require('zeromq')

// Connect to your ZeroMQ Socket
var sock = zmq.socket('sub')
sock.connect('tcp://127.0.0.1:4001');
sock.subscribe('rand');
console.log('ZMQ sub connected to port 4000');

//Connect to SocketIO 
io.on('connection', function() {
  console.log('a user connected');
})

//Create a function that will get triggered by ZeroMQ. Data is the binary stream that is recieved by ZeroMQ.
function trigger(data) {
  //Throw away the Topic of your recieved String by cutting off the first 4 bytes ("rand") 
  data = data.toString().slice(4)
  //Parse the remaining string and send the object to your WebUi via SocketIO
  data = JSON.parse(data)
  io.emit("news", data)
}

//Connect your triggerfunction and zeromq.
sock.on('message', trigger)