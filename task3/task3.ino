#include <Servo.h>

int distance = 0;
float a = 8412.5893;
float b = -80.1786;
float distance_cm = 0;

Servo myservo;

int desiredAngle;

int pid;

double kp = 0.16;
double ki = 0.00005;
double kd = 7;

float prevDistance = 0;
float previousDistanceSum = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(9);
}

void loop() {
  // 215 na srodku
  // 155 - daleko od sensora
  // 500 - blisko sensora
  distance = analogRead(A0);
  prevDistance = distance_cm;
  // przeliczanie na cm i centrowanie
  distance_cm = (a / (distance - b)) - 28.5;
  previousDistanceSum += distance_cm;

  pid = kp * distance_cm + ki * previousDistanceSum + kd * (distance_cm - prevDistance);

  desiredAngle = map(pid, -14.5, 14.5, 0, 210);
  myservo.write(desiredAngle);
  Serial.println(desiredAngle);
}
