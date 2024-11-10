#include "TRSensors.h"

#define NUM_SENSORS 5
// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
TRSensors trs =   TRSensors();
unsigned int sensorValues[NUM_SENSORS];

//jak robot skreca w prawo od lini to positio dodatnie

int position = 0;
int prevPosition = 0;
int previousPositionSum = 0;

int ENA = 5; // right motor
int ENB = 6;

int IN1 = A0;
int IN2 = A1;
int IN3 = A2;
int IN4 = A3;

int powerLeft = 80;
int powerRight = 80;

int pid;

double kp = 0.33;
double ki = 0;
double kd = -0.02;

int lostCounter = 0;

void setup() {
  for (int i = 0; i < 400; i++)  // make the calibration take about 10 seconds
  {
    trs.calibrate();       // reads all sensors 10 times
  }
  position = trs.readLine(sensorValues)-2000;

  Serial.begin(9600);
  analogWrite(ENA, powerLeft);
  analogWrite(ENB, powerRight);
  cleanPins();
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
}

void loop() {

  position = trs.readLine(sensorValues)-2000;
  //Serial.println(position);
  previousPositionSum += position;
  
  if ((position == 2001 || position == -2001) && (prevPosition == 2001 || prevPosition == -2001)) {
    lostCounter++;
    if (lostCounter > 10) {
      findLine();
      lostCounter = 0;
    }
  } else {
    lostCounter = 0;
  }

  pid = kp * position + ki * previousPositionSum + kd * (position - prevPosition);
  prevPosition = position;

  analogWrite(ENB, normalize(powerRight + pid));
  analogWrite(ENA, normalize(powerLeft - pid));
  delay(1);

}

void findLine() {
  // powerLeft = 80;
  // powerRight = 80;
  if (position == -2001) {
    //analogWrite(ENA, powerRight);
    analogWrite(ENB, 0);
    while (position == -2001) {
      position = trs.readLine(sensorValues)-2000;
    }
  }
  else {
    analogWrite(ENA, 0);
    //analogWrite(ENB, powerLeft);
    while (position == 2001) {
      position = trs.readLine(sensorValues)-2000;
    }
  }
}

int normalize(int value) {
  if (value > 255) return 255;
  else if (value < 0) return 0;
  else return value;
}

void cleanPins() {
  analogWrite(ENB, 0);
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
