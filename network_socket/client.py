import socket

connectSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
connectSocket.connect(('127.0.0.1', 8089))

while True:
    try:
        _data = input("> ")
        if _data != "":
            print("Sending:", _data)
            connectSocket.send(_data.encode())
            if _data == "DONE":
                print("Shutting down client ...")
                connectSocket.close()
                break
    except KeyboardInterrupt as exc:
        print("Error:\n")
        print(exc)
        print("Shutting down client ...")
        connectSocket.close()
        break
    
connectSocket.close()