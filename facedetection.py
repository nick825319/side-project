from __future__ import print_function
import cv2
import numpy as np

HAAR_CASCADE_XML_FILE_FACE = "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml"

gst_str = ('nvarguscamerasrc ! '
                   'video/x-raw(memory:NVMM), '
                   'width=(int)800, height=(int)600, '
                   'format=(string)NV12, framerate=(fraction)30/1 ! '
                   'nvvidconv flip-method=2 ! '
                   'video/x-raw, width=(int){}, height=(int){}, '
                   'format=(string)BGRx ! '
                   'videoconvert ! appsink').format(800, 600)

def faceDetect():
    face_cascade = cv2.CascadeClassifier(HAAR_CASCADE_XML_FILE_FACE)

    video_capture = cv2.VideoCapture(gst_str, cv2.CAP_GSTREAMER)
    if video_capture.isOpened():
        cv2.namedWindow("faceDetect", cv2.WINDOW_NORMAL)
        cv2.resizeWindow("faceDetect", 800, 600)
        while True:
            return_key, image = video_capture.read()
            if not return_key:
                break

            grayscale_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            detected_faces = face_cascade.detectMultiScale(grayscale_image, 1.3, 5)
            
            #create rectangle around face
            for(x_pos, y_pos, width, height) in detected_faces:
                cv2.rectangle(image, (x_pos, y_pos), (x_pos + width, y_pos + height), (0, 0, 0), 2)
            cv2.imshow("faceDetect", image)

            # key esc as quit
            key = cv2.waitKey(30) & 0xff
            if key == 27:
                break

        video_capture.release()
        cv2.destoryAllWindows()
    else:
        print("open camera fail")

if __name__ == "__main__":
    faceDetect()


