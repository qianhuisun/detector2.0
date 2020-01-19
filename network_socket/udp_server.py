from socket import *

serverSocket = socket(AF_INET, SOCK_DGRAM)

serverSocket.bind(('localhost', 8090))

print("UDP server is listening...")

while True:

    input("Enter to continue > ")

    _bytes, addr = serverSocket.recvfrom(1024)

    print(_bytes.decode())
    print(addr)

    serverSocket.sendto("Server received: ".encode() + _bytes, addr)
