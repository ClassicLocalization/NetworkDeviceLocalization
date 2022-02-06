import socket
UDP_IP = '192.168.178.69'
UDP_PORT = 50000
BUFFER_SIZE = 1024
MESSAGE = "Message from PC"
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((UDP_IP, UDP_PORT))
s.send(MESSAGE)
data = s.recv(BUFFER_SIZE)
s.close()
print("received data:", data)