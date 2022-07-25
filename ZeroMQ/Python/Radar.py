from socket import socket, AF_INET, SOCK_DGRAM

def get_single_csi(ip, iterations):
    PORT = 50000

    s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
    s.bind(('', PORT))

    some_data = "1 .4"
    s.sendto (some_data.encode ('utf-8'), (ip, PORT))
    print ("sent service announcement")
    for i in range(iterations):
        data, addr = s.recvfrom(1024) #wait for a packet
        print("got service announcement from", data)


def main():
    get_single_csi("192.168.178.58", 500)

if __name__ == "__main__":
    main()