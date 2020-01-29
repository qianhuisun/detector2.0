import socket
import os

print("Connecting...")
if os.path.exists("/tmp/test_qianhui"):
    connectSocket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    connectSocket.connect("/tmp/test_qianhui")
    print("Ready.")
    print("Ctrl-C to quit.")
    print("Sending 'DONE' shuts down the server and quits.")
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

else:
    print("Couldn't Connect!")
    print("Done")
