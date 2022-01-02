<template>
  <div class="hello">
    <h1>{{ msg }}</h1>
    <button @click="startConnection()">start connection</button>
  </div>
</template>

<script>
import Zmq from 'zeromq';

export default {
  name: 'HelloWorld',
  props: {
    msg: String
  },
  methods: {
    startConnection(){
      const zmq = Zmq.require("zeromq");
      const sock = zmq.socket("pull");

      sock.connect("tcp://192.168.178.26:3000");
      console.log("Worker connected to port 3000");

      sock.on("message", function(msg) {
        console.log("work: %s", msg.toString());
      });
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h3 {
  margin: 40px 0 0;
}
ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}
</style>
