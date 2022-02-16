from socket import socket, AF_INET, SOCK_DGRAM
from time import sleep

# for closing the socket
def close(s):
    s.close()
    print("closed")


def start_session(count):
    PORT = 50000
    PORT = 50000

    s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
    s.bind(('', PORT))

    someData = "Proceed with CSI"
    s.sendto(someData.encode('utf-8'), ("192.168.4.2", PORT))
    print("sent service announcement")
    data, addr = s.recvfrom(1024) #wait for a packet
    print("got service announcement from", data)
    print(addr)
    recieved = False;
    counter = 0
    while not recieved:
        counter += 1
        data, addr = s.recvfrom(1024)  # wait for a packet
        print("got service announcement from", data)
        print(addr)
        someData = "Proceed with CSI"
        s.sendto (someData.encode ('utf-8'), ("192.168.4.2", PORT))
        print ("sent service announcement")
        data, addr = s.recvfrom (1024)  # wait for a packet
        if counter > count:
         recieved = True

start_session(15)

