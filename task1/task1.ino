


int ENA = 5; // left motor
int ENB = 6;

int IN1 = A0;
int IN2 = A1;
int IN3 = A2;
int IN4 = A3;

int powerLeft = 100;
int powerRight = 100;

int leftChanges = 0;
int rightChanges = 0;

void setup() {
  Serial.begin(9600);
  analogWrite(ENA, powerLeft);
  analogWrite(ENB, powerRight);

  attachInterrupt(digitalPinToInterrupt(2), leftRotationIncrement, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), rightRotationIncrement, CHANGE);

  goForward(200);

  Serial.print("left distance cm: ");
  float leftDistance = getLeftDistance();
  Serial.print(leftDistance);
  
  Serial.print("right distance cm: ");
  float rightDistance = getRightDistance();
  Serial.print(rightDistance);
}

void leftRotationIncrement() {
  leftChanges++;
}
void rightRotationIncrement() {
  rightChanges++;
}

void loop() {

}
// 20 holes = 65mm | 42 changes = 1 obrot = 20,41cm
float getLeftDistance(){
  // Serial.println("");
  // Serial.print("left distance: ");
  float distanceCm = leftChanges /42.0 *20.41; // TODO nie robić definicji w petli, globalne zmienne powinny byc
  // Serial.print(distanceCm);
  // Serial.println(" cm");
  return distanceCm;
}

float getRightDistance() {
  // Serial.println("");
  // Serial.print("right distance: ");
  float distance = rightChanges /42.0 *20.41;
  // Serial.print(distance);
  // Serial.println(" cm");
  return distance;
}

void cleanPins() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void goForward(int distanceCm) {
  cleanPins();
  float changeFactor = 0.7;
  float startingDistanceLeft = getLeftDistance();
  float startingDistanceRight = getRightDistance();
  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, HIGH);

  unsigned long lastCorrection = millis();
  while ((getLeftDistance() - startingDistanceLeft <distanceCm) ) {
    if (millis() - lastCorrection>100) { // co 100ms obimy korekte
      float difference = getLeftDistance()-getRightDistance();

      powerRight = powerRight + changeFactor*(difference);
      powerLeft = powerLeft - changeFactor*(difference);
      
      powerRight = normalize(powerRight);
      powerLeft = normalize(powerLeft);

      analogWrite(ENB, powerRight);
      analogWrite(ENA, powerLeft);
      
    }

    // Serial.print("right power ");
    // Serial.println(powerRight);
    // Serial.print("left power ");
    // Serial.println(powerLeft);
    // Serial.print("difference ");
    // Serial.println(difference);
    
    //getRightDistance();
    lastCorrection = millis();
  }
  cleanPins();
}

int normalize(int value) {
  if (value > 255) return 255;
  else if (value < 0) return 0;
  else return value;
}

void goBack() {
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
}

void goRight() {
  digitalWrite(IN1, HIGH); // TODO
  digitalWrite(IN4, HIGH);
}

void goLeft() {
  digitalWrite(IN1, HIGH); // TODO
  digitalWrite(IN4, HIGH);
}



