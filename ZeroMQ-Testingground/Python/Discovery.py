from socket import socket, AF_INET, SOCK_DGRAM
import time
import csv

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

def get_single_csi(ip, iterations):
    PORT = 5000
    path = r"C:\Users\Superadmin\Desktop\Bachelorarbeit\ZeroMQ-Testingground\Python\relevantData_diffRoom.csv"

    s = socket(AF_INET, SOCK_DGRAM) #create UDP socket
    s.bind(('', PORT))

    some_data = "1 .4"
    for i in range(iterations):
        s.sendto(some_data.encode ('utf-8'), (ip, PORT))
        print ("sent service announcement")
        data, addr = s.recvfrom(1024) #wait for a packet
        print("got service announcement from", data)
        parsed_content = "2,5" + data.decode("utf-8")

        with open (path, 'a', newline='', encoding='utf-8') as f:
            # create the csv writer
            writer = csv.writer (f)
            # write a row to the csv file
            writer.writerow ([parsed_content])
        time.sleep(10)


def main():
    # start_session(5)
    # start_protocol()
    get_single_csi ("192.168.178.58", 25)


if __name__ == "__main__":
    main()



