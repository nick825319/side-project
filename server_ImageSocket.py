import socket
#hostname = '127.0.0.1'
#hostname = "140.122.185.98"
hostname = '140.122.184.103'
port = 12000
image_name = "image%s.png"
imgcounter = 1
buffer_size = 4096
WAIT_CONNECT_FLAG = 1
server = socket.socket()
server.bind((hostname, port))
server.listen(5)
remaining = 0
while True:
    buffer_size = 4096
    if WAIT_CONNECT_FLAG == 1:
        print("waiting connect... listening port:", port)
        connect_socket, client_address = server.accept()
        print(client_address, "connecting")
        WAIT_CONNECT_FLAG = 0

    print("waiting for new event")
    receive_content = connect_socket.recv(buffer_size)
   # b = bytearray(receive_content)
   # test  = binascii.hexlify(b)
   # print(test)
    try:
        txt = receive_content.decode("utf-8")
        if txt.startswith('SIZE'):
            print(txt)
            tmp = txt.split()
            size = int(tmp[1])

            print('size is %s' % size)
            connect_socket.send(bytes("GOT SIZE", encoding="utf-8"))
            # Set the buffer size for the image
            print('got size')
            buffer_size = 40960000

            # write file
            my_file = open(image_name % imgcounter, 'ab')
            receive_content = connect_socket.recv(buffer_size)
            print("receive data size: ", receive_content.__len__())
            remaining = size - receive_content.__len__()
            my_file.write(receive_content)
            print("writing to file finished")
            connect_socket.send(bytes("SERVER GOT IMAGE PART", encoding="utf-8"))
            my_file.close()
        elif txt.startswith('BYE'):
            WAIT_CONNECT_FLAG = 1
            print("client disconnected...")
            connect_socket.close()
        else:
            # write file
            if receive_content.__len__() != 0:
                my_file = open(image_name % imgcounter, 'ab')
                print("receive data size: ", receive_content.__len__())
                remaining -= receive_content.__len__()
                print("remain %s" % remaining)
                my_file.write(receive_content)
                print("writing to file finished")
                connect_socket.send(bytes("SERVER GOT IMAGE PART", encoding="utf-8"))
                my_file.close()
                if remaining <= 0:
                    connect_socket.send(bytes("FINISH", encoding="utf-8"))
                    connect_socket.close()
                    WAIT_CONNECT_FLAG = 1
                    imgcounter += 1
            else:
                print("receive 0 size packet ,close socket")
                connect_socket.send(bytes("FINISH", encoding="utf-8"))
                WAIT_CONNECT_FLAG = 1
    except UnicodeDecodeError:
        print("UnicodeDecodeError")
        # write file
        my_file = open(image_name % imgcounter, 'ab')
        print("receive data size: ", receive_content.__len__())
        remaining -= receive_content.__len__()
        print("remain %s" % remaining)
        my_file.write(receive_content)
        print("writing to file finished")
        connect_socket.send(bytes("SERVER GOT IMAGE PART", encoding="utf-8"))
        my_file.close()
        if remaining <= 0:
            connect_socket.send(bytes("FINISH", encoding="utf-8"))
            WAIT_CONNECT_FLAG = 1
            imgcounter += 1



