from time import sleep
from socket import socket, AF_INET, SOCK_DGRAM, SOL_SOCKET, SO_BROADCAST, gethostbyname, gethostname


PORT = 50000
MAGIC = "fna349fn" #to make sure we don't confuse or get confused by other programs

s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
s.bind(('', 0))
s.setsockopt(SOL_SOCKET, SO_BROADCAST, 1) #this is a broadcast socket
my_ip= gethostbyname(gethostname()) #get our IP. Be careful if you have multiple network interfaces or IPs

while 1:
    #data = MAGIC+my_ip
    data = "Message from PC"
    s.sendto(bytes(data, 'utf-8'), ('<broadcast>', PORT))
    print(bytes(data, 'utf-8'))
    print("sent service announcement")
    sleep(5)