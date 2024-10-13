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
float changesToCmFactor = 42.0 * 20.41; // 42 zmiany (CHANGE) na pinie sensora obrotu przypadają na jeden obrót, który wynosi 20.41cm
float startingDistanceLeft = 0;
const float changeFactor = 0.5;
float differenceBetweenLeftAndRightMotorCm = 0;

// Ważne:
// - nie robić definicji w petli, globalne zmienne powinny byc (definicja zmiennej kosztuje)

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
void loop() {

}

void leftRotationIncrement() {
  leftChanges++;
}
void rightRotationIncrement() {
  rightChanges++;
}

// 20 holes = 65mm | 42 changes = 1 obrot = 20,41cm
float getLeftDistance(){
  return leftChanges / changesToCmFactor;
}

float getRightDistance() {
  return rightChanges / changesToCmFactor;
}

void cleanPins() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void goForward(int distanceCm) {
  cleanPins();
  startingDistanceLeft = getLeftDistance();
  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, HIGH);

  while ((getLeftDistance() - startingDistanceLeft < distanceCm) ) { // dopóki lewy silnik nie przebył zadanego dystansu
      differenceBetweenLeftAndRightMotorCm = getLeftDistance() - getRightDistance();

      powerRight = powerRight + changeFactor * differenceBetweenLeftAndRightMotorCm;
      powerLeft = powerLeft - changeFactor * differenceBetweenLeftAndRightMotorCm;

      powerRight = normalize(powerRight);
      powerLeft = normalize(powerLeft);

      analogWrite(ENB, powerRight);
      analogWrite(ENA, powerLeft);
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
