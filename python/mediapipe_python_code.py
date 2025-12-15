# mediapipe_serial_trigger.py
import mediapipe as mp
import cv2 as cv
import time
import serial
import serial.tools.list_ports

# ---------------- Serial setup ----------------
SERIAL_PORT = 'COM4'
BAUDRATE = 115200
SERIAL_TIMEOUT = 0.1

def auto_find_port():
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if ('Arduino' in p.description) or ('CH340' in p.description) or ('USB Serial' in p.description) or ('CP210' in p.description):
            return p.device
    if ports:
        return ports[0].device
    return None

if SERIAL_PORT == 'AUTO':
    SERIAL_PORT = auto_find_port()

ser = None
if SERIAL_PORT:
    try:
        ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=SERIAL_TIMEOUT)
        print(f"Opened serial on {SERIAL_PORT}")
        time.sleep(1.2)
        ser.reset_input_buffer()
    except Exception as e:
        print("Serial open failed:", e)
        ser = None

# ---------------- Mediapipe setup ----------------
mp_draw = mp.solutions.drawing_utils
mp_hands = mp.solutions.hands

vid = cv.VideoCapture(0)

history = []
hist_size = 15

robot_busy = False
last_triggered_count = -1

with mp_hands.Hands(min_detection_confidence=0.7,
                    min_tracking_confidence=0.7) as hands:

    while vid.isOpened():
        succeed, img = vid.read()
        if not succeed:
            break

        img = cv.flip(img, 1)
        image_rgb = cv.cvtColor(img, cv.COLOR_BGR2RGB)
        image_rgb.flags.writeable = False
        results = hands.process(image_rgb)
        image_rgb.flags.writeable = True

        ans = 0
        if results.multi_hand_landmarks:
            for i in results.multi_hand_landmarks:
                lm = i.landmark
                v1 = [8, 12, 16, 20]
                v2 = [6, 10, 14, 18]
                v = []

                for j in range(4):
                    v.append(1 if lm[v1[j]].y < lm[v2[j]].y else 0)

                if lm[4].x < lm[20].x:
                    v.append(1 if lm[4].x < lm[3].x else 0)
                else:
                    v.append(1 if lm[4].x > lm[3].x else 0)

                ans = sum(v)
                mp_draw.draw_landmarks(img, i, mp_hands.HAND_CONNECTIONS)

        # -------- stable finger count --------
        history.append(ans)
        if len(history) > hist_size:
            history.pop(0)

        try:
            stable_ans = max(set(history), key=history.count)
        except ValueError:
            stable_ans = 0

        # -------- read Arduino feedback --------
        if ser:
            while ser.in_waiting:
                line = ser.readline().decode(errors='ignore').strip()
                if line:
                    print("<< Arduino:", line)
                    if "Task completed" in line:
                        robot_busy = False

        # ================= TRIGGER LOGIC =================
        if stable_ans != 0:
            parity = stable_ans % 2   # 0 = even, 1 = odd
        else:
            parity = None

        if (parity is not None) and (not robot_busy) and (parity != last_triggered_count):
            if ser:
                if parity == 1:
                    ser.write(b'1\n')
                    print(">> Sent ODD (1)")
                else:
                    ser.write(b'0\n')
                    print(">> Sent EVEN (0)")
                ser.flush()

            robot_busy = True
            last_triggered_count = parity

        if stable_ans == 0:
            last_triggered_count = -1

        # ---------------- UI ----------------
        cv.putText(img, f"Finger Count: {stable_ans}", (20, 50),
                   cv.FONT_HERSHEY_SIMPLEX, 0.8, (255, 0, 0), 2)

        status = "BUSY" if robot_busy else "IDLE"
        cv.putText(img, f"robot: {status}", (20, 90),
                   cv.FONT_HERSHEY_SIMPLEX, 0.9,
                   (0, 0, 255) if robot_busy else (0, 255, 0), 2)

        cv.imshow("Finger Counter", cv.resize(img, (960, 540)))

        if cv.waitKey(10) & 0xFF == ord('q'):
            break

vid.release()
cv.destroyAllWindows()
if ser:
    ser.close()
