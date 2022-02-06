from socket import socket, AF_INET, SOCK_DGRAM
import time
import zmq


def createServer():
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind ("tcp://*:5555")

    while True:
        #  Wait for next request from client
        message = socket.recv ()
        print("Received request: %s" % message)

        #  Do some 'work'
        time.sleep(1)

        #  Send reply back to client
        socket.send(b"World")



PORT = 50000
MAGIC = "fna349fn" #to make sure we don't confuse or get confused by other programs

s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
s.bind(('', PORT))

while 1:
    data, addr = s.recvfrom(1024) #wait for a packet
    print("got service announcement from", data[len(MAGIC):])
    createServer()


