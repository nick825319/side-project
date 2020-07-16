import io
import socket
import struct
import time
from PIL import Image
import matplotlib.pyplot as pl


server_socket = socket.socket()
server_socket.bind(('140.122.184.103', 15002))  # ADD IP HERE
server_socket.listen(5)

# Accept a single connection and make a file-like object out of it
connection = server_socket.accept()[0].makefile('rb')
print("connecting")
f = pl.figure()
ax = f.add_subplot()

try:
    img = None
    while True:
        # Read the length of the image as a 32-bit unsigned int. If the
        # length is zero, quit the loop
        image_len = struct.unpack('<L', connection.read(struct.calcsize('<L')))[0]
        if not image_len:
            break
        # Construct a stream to hold the image data and read the image
        # data from the connection
        image_stream = io.BytesIO()

        start_time = time.time()
        image_stream.write(connection.read(image_len))
        network_time = time.time()
        # Rewind the stream, open it as an image with PIL and do some
        # processing on it
        image_stream.seek(0)
        image = Image.open(image_stream)
        image_read_time = time.time()

        if img is None:
            img = ax.imshow(image)
        else:
             img.set_data(image)

        pl.pause(0.01)
        f.canvas.draw()
        #pl.draw()
        end_time = time.time()

        print('all response time = %0.5f' % (end_time-start_time))
        print('network time = %0.5f' % (network_time-start_time))
        print('open image time = %0.5f' % (image_read_time-network_time))
        print('show image time = %0.5f' % (end_time-image_read_time))

        print('Image is %dx%d' % image.size)
        #image.verify()
        #print('Image is verified')
finally:
    connection.close()
    server_socket.close()
