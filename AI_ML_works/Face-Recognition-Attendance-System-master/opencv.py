import cv2
import pandas as pd
from datetime import datetime

cap= cv2.VideoCapture(0)
face_cascade = cv2.CascadeClassifier("haarcascade_frontalface_alt.xml")

while True:
    ret,frame= cap.read()
    gray_frame= cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    if ret== False:
        continue

    faces= face_cascade.detectMultiScale(frame,1.3,5)

    for (x,y,w,h) in faces:
        cv2.rectangle(frame, (x,y), (x+w, y+h), (255,0,0), 2)

    


    # current date and time
    now = datetime.now()
    timestamp = datetime.timestamp(now)
    dt_object = datetime.fromtimestamp(timestamp)
    print(dt_object)
 
    cv2.imshow("Video Frame", frame) 
    cv2.imshow("Gray Frame", gray_frame)   

    #wait for user imput - q, then you will stop the loop
    key_pressed= cv2.waitKey(1) & 0xFF

    if key_pressed == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()    