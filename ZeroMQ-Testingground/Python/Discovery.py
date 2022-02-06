from socket import socket, AF_INET, SOCK_DGRAM
from time import sleep

# for closing the socket
def close(s):
    s.close()
    print("closed")

PORT = 50000
MAGIC = "fna349fn" #to make sure we don't confuse or get confused by other programs

s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
s.bind(('', PORT))

while 1:
    data, addr = s.recvfrom(1024) #wait for a packet
    print("got service announcement from", data)
    print(addr)
    s.sendto(data, (addr[0], addr[1]))
    print(data)
    print("sent service announcement")
    sleep(5)

