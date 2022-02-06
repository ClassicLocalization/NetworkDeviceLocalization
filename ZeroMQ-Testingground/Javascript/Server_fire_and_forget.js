const zmq = require("zeromq");

const sock = zmq.socket("push");

var workerNumber = 0;
var timeStamps = [];

sock.bindSync("tcp://192.168.178.23:3000");
console.log("Server bound to port 3000");

setInterval(function() {
    console.log(`sending work ${workerNumber}`);
        timeStamps.push(performance.now());
        sock.send(`received work ${workerNumber} (${timeStamps[workerNumber]})`);
        workerNumber++;
}, 1000);
