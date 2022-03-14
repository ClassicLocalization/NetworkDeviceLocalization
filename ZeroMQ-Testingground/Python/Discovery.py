from socket import socket, AF_INET, SOCK_DGRAM
import time

# for closing the socket
def close(s):
    s.close()
    print("closed")


def start_session(count):
    PORT = 50000

    s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
    s.bind(('', PORT))

    some_data = "csi -e .2"
    s.sendto(some_data.encode('utf-8'), ("192.168.4.1", PORT))
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
        some_data = "Proceed with CSI"
        s.sendto(some_data.encode ('utf-8'), ("192.168.4.1", PORT))
        print("sent service announcement")
        data, addr = s.recvfrom(1024)  # wait for a packet
        if counter > count:
            recieved = True

def start_protocol():
    PORT = 50000;
    OWN_IP = 7;
    TIME_DELAY = 10
    s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
    s.bind(('', PORT))
    for i in range(2, 7):
        some_data = f"1 .{OWN_IP}"
        s.sendto(some_data.encode ('utf-8'), (f"192.168.4.{i}", PORT))
        print("sent service announcement")
        data, addr = s.recvfrom (1024)  # wait for a packet
        print("got service announcement from", data)
        print(addr)
        time.sleep(TIME_DELAY)
        if i <= 4:
            some_data = f"{i+1} .{OWN_IP}"
            s.sendto(some_data.encode('utf-8'), (f"192.168.4.{i}", PORT))
            print("sent service announcement")
            data, addr = s.recvfrom (1024)  # wait for a packet
            print("got service announcement from", data)
            print(addr)
            time.sleep(TIME_DELAY)
            some_data = f"{i+2} .{OWN_IP}"
            s.sendto(some_data.encode('utf-8'), (f"192.168.4.{i}", PORT))
            print("sent service announcement")
            data, addr = s.recvfrom (1024)  # wait for a packet
            print("got service announcement from", data)
            print(addr)
            time.sleep(TIME_DELAY)
        elif i == 5:
            some_data = f"{i+1} .{OWN_IP}"
            s.sendto(some_data.encode ('utf-8'), (f"192.168.4.{i}", PORT))
            print("sent service announcement")
            data, addr = s.recvfrom (1024)  # wait for a packet
            print("got service announcement from", data)
            print(addr)
            time.sleep(TIME_DELAY)
            some_data = f"2 .{OWN_IP}"
            s.sendto(some_data.encode ('utf-8'), (f"192.168.4.{i}", PORT))
            print("sent service announcement")
            data, addr = s.recvfrom (1024)  # wait for a packet
            print("got service announcement from", data)
            print(addr)
            time.sleep(TIME_DELAY)
        else:
            some_data = f"2 .{OWN_IP}"
            s.sendto(some_data.encode ('utf-8'), (f"192.168.4.{i}", PORT))
            print("sent service announcement")
            data, addr = s.recvfrom (1024)  # wait for a packet
            print("got service announcement from", data)
            print(addr)
            time.sleep(TIME_DELAY)
            some_data = f"3 .{OWN_IP}"
            s.sendto(some_data.encode ('utf-8'), (f"192.168.4.{i}", PORT))
            print("sent service announcement")
            data, addr = s.recvfrom (1024)  # wait for a packet
            print("got service announcement from", data)
            print(addr)
            time.sleep(TIME_DELAY)
        time.sleep(TIME_DELAY)


#start_session(5)
start_protocol()

