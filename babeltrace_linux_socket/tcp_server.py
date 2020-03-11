from socket import * 
from select import *
import os

def createServer():

    if os.path.exists("/tmp/test_qianhui"):
        os.remove("/tmp/test_qianhui")

    print("Opening socket...")
    serverSocket = socket(AF_UNIX, SOCK_STREAM)
    serverSocket.bind("/tmp/test_qianhui")
    
    serverSocket.listen(5)
    print("Server is listening ...")
    serverSocket.settimeout(20)

    while True:
        print("Listening to next client...")

        connectSocket, addr = serverSocket.accept()

        try:
            _bytes = connectSocket.recv(1024)
            _data = _bytes.decode()
            print("-" * 20)
            print(_data)
            _bytes = connectSocket.recv(1024)
            _data = _bytes.decode()
            print("-" * 20)
            print(_data)
            connectSocket.close()
        except Exception as e:
            print(e)
            connectSocket.close()

    print("-" * 20)
    print("Server is shutting dowen ...")
    serverSocket.close()

createServer()