# Face Fan: the webcam finds your face, the fan on the servo follows you
# pip install "opencv-python<5" pyserial   (OpenCV 5 dropped the face finder used here)
#
# Mega version: works with the Elegoo MEGA 2560 R3 + L293D (face_fan_mega.ino).
# Set PORT to your Mega's COM port (Arduino IDE > Tools > Port).
# Close the Arduino Serial Monitor before running this!
# PORT = None runs without the Arduino and just prints what it would send.
#
# Sends "position,speed": far away = full speed, close = gentle breeze.
import time
import cv2

__version__ = "1.0.0"   # keep in step with the VERSION file

PORT = "COM3"   # <-- change to YOUR port number

ser = None
if PORT:
    import serial
    try:
        ser = serial.Serial(PORT, 115200, timeout=0)
    except serial.SerialException:
        from serial.tools import list_ports
        print("Can't open", PORT, "- close the Serial Monitor and check the number.")
        print("Ports I can see:", [p.device for p in list_ports.comports()])
        raise SystemExit
    time.sleep(2)   # the Mega restarts when the port opens

# face finder that comes built into OpenCV (no download needed)
faces_model = cv2.CascadeClassifier(
    cv2.data.haarcascades + "haarcascade_frontalface_default.xml")
cap = cv2.VideoCapture(0)

smooth = 50.0
FAR, CLOSE = 10, 40   # face width in % of the picture (tune these)
GENTLE = 40           # fan speed when you're really close
last_sent = None
last_send_time = 0

while True:
    ok, frame = cap.read()
    if not ok:
        break
    frame = cv2.flip(frame, 1)    # mirror view, feels natural
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = faces_model.detectMultiScale(gray, scaleFactor=1.2,
                                         minNeighbors=5, minSize=(60, 60))
    w_frame = frame.shape[1]

    if len(faces) > 0:
        # pick the biggest face (the closest person)
        x, y, w, h = max(faces, key=lambda f: f[2] * f[3])
        center = x + w / 2
        pos = center / w_frame * 100            # 0 = left edge, 100 = right edge
        smooth = 0.7 * smooth + 0.3 * pos       # removes jitter
        # face size tells how close you are: small face = far = blow harder
        size = w / w_frame * 100                # about 10 (far) to 40 (close)
        speed = int(100 - (size - FAR) / (CLOSE - FAR) * (100 - GENTLE))
        speed = max(GENTLE, min(100, speed)) // 10 * 10
        msg = f"{int(smooth) // 2 * 2},{speed}"  # steps so it doesn't spam
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
    else:
        msg = "-1"                              # no face

    # send when it changes (max 20 times a second), and repeat every
    # half second anyway so the board knows you're still there
    now = time.time()
    if (msg != last_sent and now - last_send_time > 0.05) or now - last_send_time > 0.5:
        if ser:
            ser.write((msg + "\n").encode())
        else:
            print("would send:", msg)
        last_sent = msg
        last_send_time = now

    cv2.putText(frame, f"send {msg}", (10, 30),
                cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
    cv2.imshow("Face Fan (press q to quit)", frame)
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
if ser:
    ser.close()
cv2.destroyAllWindows()
