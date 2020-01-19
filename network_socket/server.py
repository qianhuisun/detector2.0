from socket import * 
from select import *

def createServer():

    serverSocket = socket(AF_INET6, SOCK_STREAM)
        
    serverSocket.bind(('::1', 8089))
    serverSocket.listen(5)
    print("Server is listening ...")
    serverSocket.settimeout(10)

    inputs = [serverSocket]

    while inputs:
        
        input("Enter to continue > ")

        readable, writable, exceptional = select(inputs, [], inputs)

        for s in exceptional:
            inputs.remove(s)
            s.close()
        
        for s in readable:
            if s is serverSocket:
                connectSocket, addr = s.accept()
                print("Got client addr: " + str(addr))
                connectSocket.setblocking(0)
                inputs.append(connectSocket)
            else:
                inputs.remove(s)
                s.setblocking(1)
                listening = True
                try:
                    while listening:
                        _bytes = s.recv(1024)
                        _data = _bytes.decode()
                        print("-" * 20)
                        print(_data)
                        if _data == "DONE":
                            listening = False
                    s.close()
                except Exception as e:
                    print(e)
                    s.close()

    print("-" * 20)
    print("Server is shutting dowen ...")
    serverSocket.close()

print("Access http://localhost:8089")
createServer()