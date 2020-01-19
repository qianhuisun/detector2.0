from socket import *

clientSocket = socket(AF_INET, SOCK_DGRAM)

clientSocket.sendto("Hi".encode(), ('localhost', 8090))

_bytes, addr = clientSocket.recvfrom(1024)

print(_bytes.decode())

clientSocket.sendto("DONE".encode(), ('localhost', 8090))

_bytes, addr = clientSocket.recvfrom(1024)

print(_bytes.decode())