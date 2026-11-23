// Face Fan - Elegoo MEGA 2560 R3 version
// Parts: Mega 2560, L293D chip on a breadboard, SG90 servo, DC motor + fan blade,
//        breadboard power supply module (set to 5V) with a 9V battery.
// Wiring (all jumper wires, no screw terminals):
//   Servo signal (orange) -> pin 3,  servo red -> +5V rail,  servo brown -> GND rail
//   L293D pin 1 (EN1) -> pin 10  pin 2 (IN1) -> pin 9   pin 7 (IN2) -> pin 8
//   L293D pin 3 + pin 6 -> the 2 motor wires
//   L293D pin 4, 5, 12, 13 -> GND rail     pin 16 + pin 8 -> +5V rail
//   Mega GND -> GND rail (all grounds must meet)
// The Mega's own "L" LED (pin 13) lights while it sees a face.
// Laptop sends "position,speed" + newline (e.g. 42,80), or -1 when no face.
#include <Servo.h>

Servo head;
const int SERVO_PIN = 3;
const int FAN_EN  = 10;   // PWM pin -> fan speed
const int FAN_IN1 = 9;
const int FAN_IN2 = 8;

const int LEFT_ANGLE  = 150;  // swap these two if the fan turns AWAY from you
const int RIGHT_ANGLE = 30;
const int MIN_PWM = 130;      // raise (150, 180) if the motor hums but won't start
const unsigned long FAN_OFF_AFTER = 2000;   // ms without a face -> fan stops
const unsigned long CENTER_AFTER  = 5000;   // ms without a face -> fan looks forward

float angle = 90;
int target = 90;
int fanSpeed = 0;
bool fanRunning = false;
unsigned long lastFace = 0;

void setup() {
  head.attach(SERVO_PIN);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FAN_EN, OUTPUT);
  pinMode(FAN_IN1, OUTPUT);
  pinMode(FAN_IN2, OUTPUT);
  digitalWrite(FAN_IN1, HIGH);   // swap HIGH/LOW (or the 2 motor wires) to reverse airflow
  digitalWrite(FAN_IN2, LOW);
  analogWrite(FAN_EN, 0);
  head.write(90);
  Serial.begin(115200);
  Serial.println("Face Fan (Mega) ready. Send position,speed (like 50,80) or -1.");
}

void loop() {
  // 1. read the latest message from the laptop
  if (Serial.available()) {
    int pos = Serial.parseInt();
    int spd = 100;
    if (Serial.peek() == ',') {
      Serial.read();
      spd = Serial.parseInt();
    }
    while (Serial.available()) Serial.read();
    if (pos >= 0) {
      target = map(constrain(pos, 0, 100), 0, 100, LEFT_ANGLE, RIGHT_ANGLE);
      fanSpeed = constrain(spd, 0, 100);
      lastFace = millis();
    }
  }

  unsigned long sinceFace = millis() - lastFace;
  bool faceSeen = sinceFace < FAN_OFF_AFTER;
  if (sinceFace > CENTER_AFTER) target = 90;   // nobody around: look forward

  // 2. fan motor through the L293D
  digitalWrite(LED_BUILTIN, faceSeen);
  if (faceSeen && fanSpeed > 0) {
    if (!fanRunning) {               // kick-start: full power for a moment
      analogWrite(FAN_EN, 255);
      delay(100);
      fanRunning = true;
    }
    analogWrite(FAN_EN, map(fanSpeed, 1, 100, MIN_PWM, 255));
  } else {
    analogWrite(FAN_EN, 0);
    fanRunning = false;
  }

  // 3. servo: move a little closer each time = smooth turning
  angle += (target - angle) * 0.15;
  head.write((int)angle);
  delay(20);
}
