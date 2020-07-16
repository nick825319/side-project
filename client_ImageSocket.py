import socket

#ip = '127.0.0.1'
ip = "140.122.185.98"
port = 12000

client_socket = socket.socket()
client_socket.connect((ip, port))
print("connect to ", (ip, port))

image_name = "image%s.png"

while True:
    content = input("image name (add file Extension name, '-1' for disconnect):")
    if content is None or content == "":
        content = image_name = "image%s.png" % 1
    elif content == "-1":
        client_socket.send(bytes("BYE", encoding='utf-8'))
        break

    # sending test message to server
    # client_socket.send(bytes(content, encoding='utf-8'))
    # recvdata = client_socket.recv(1024)
    # print(str(recvdata))
    try:
        # open file
        file = open(image_name, "rb")
        file_bytes = file.read()
        size = len(file_bytes)

        # send size
        client_socket.send(bytes("SIZE %s" % size, encoding='utf-8'))
        recvdata = client_socket.recv(1024)
        print(str(recvdata))
        # send image bytes
        if recvdata == b"GOT SIZE":
            client_socket.sendall(file_bytes)
            recvdata = client_socket.recv(1024)
            print(str(recvdata))
    except Exception:
        pass

