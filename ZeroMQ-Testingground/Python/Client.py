#
#   Hello World client in Python
#   Connects REQ socket to tcp://localhost:5555
#   Sends "Hello" to server, expects "World" back
#

import zmq

context = zmq.Context()

#  Socket to talk to server
print("Connecting to server…")
socket = context.socket(zmq.REQ)
socket.connect("udp://192.168.178.68:50000")

#  Do infinite requests, waiting each time for a response
request = 0
while True:
    print("Sending request %s …" % request)
    socket.send(b"%d" % request)

    #  Get the reply.
    message = socket.recv()
    print("Received reply %s [ %s ]" % (request, message))
    request+=1