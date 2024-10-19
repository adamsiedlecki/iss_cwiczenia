#include "issUtils.h"

int ENA = 5; // left motor
int ENB = 6;

int IN1 = A0;
int IN2 = A1;
int IN3 = A2;
int IN4 = A3;

int powerLeft = 100; // moc silnika 0 - 255
int powerRight = 100;

// zmiany stanu sensora obrotu (42 na 1 obrót)
int leftChanges = 0;
int rightChanges = 0;

float leftDistanceCm = 0;
float rightDistanceCm = 0;

float changesToCmFactor = 42.0 * 20.41; // 42 zmiany (CHANGE) na pinie sensora obrotu przypadają na jeden obrót, który wynosi 20.41cm
float startingDistanceLeft = 0;
float startingDistanceRight = 0;
const float changeFactor = 0.5;
float differenceBetweenLeftAndRightMotorCm = 0;

const float turningRadius = 0; // TBD potrzebna odległość koła od punktu obrotu, musi być w cm
float angleInRadians = 0; // Zmienna do przechowywania kąta skrętu w radianach
const float radiansConversionFactor = 3.14/180; // Stała do przeliczania stopni na radiany
float arcDistance = 0; //Zmienna do przechowywania dystansu potrzebnego do przejechania żeby uzyskać odpowiedni kąt skrętu

String serialRequest;
int requestAngle;
int requestDistanceCm;

// Ważne:
// - nie robić definicji w petli, globalne zmienne powinny byc (definicja zmiennej kosztuje)

void setup() {
  Serial.begin(9600);
  analogWrite(ENA, powerLeft);
  analogWrite(ENB, powerRight);

  attachInterrupt(digitalPinToInterrupt(2), leftRotationIncrement, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), rightRotationIncrement, CHANGE);

  goForward(200);

  getLeftDistance(); // zapisujemy stan do globalnych
  getRightDistance();
  log("left distance cm: ", leftDistanceCm);
  log("right distance cm: ", rightDistanceCm);
  log("diff left - right cm: ", leftDistanceCm - rightDistanceCm);
}
void loop() {
  checkSerialForRequests();
}

void checkSerialForRequests() {
  if (Serial.available() > 0) {
    // Opis protokołu:
    // arduino przymuje polecenia na Serial zakończone znakiem &.
    // Polecenia można łączyć.
    // przykładowo: F10&R90-10&B5&
    //
    // F100& - jazda 100 cm do przodu
    // B100& - jazda 100 cm do tyłu
    // R90-100& - skręt o 90 stopni w prawo i przejechanie 100cm
    // L90-100& - skręt o 90 stopni w lewo i przejechanie 100cm
    serialRequest = Serial.readStringUntil('&'); // znak & kończy polecenie - jeśli go nie ma, to nie czytamy
    if (serialRequest[0] == 'F') { // forward - do przodu
      requestDistanceCm = serialRequest.substring(1).toInt();
      goForward(requestDistanceCm);
    } else if (serialRequest[0] == 'B') { // back - do tyłu
      requestDistanceCm = serialRequest.substring(1).toInt();
      goBack(requestDistanceCm);
    } else if (serialRequest[0] == 'R') { // right - w prawo
      int separatorIndex = serialRequest.indexOf('-', 1);
      requestDistanceCm = serialRequest.substring(1, separatorIndex).toInt();
      requestAngle = serialRequest.substring(separatorIndex + 1).toInt();
      goRight(requestAngle, requestDistanceCm);
    } else if (serialRequest[0] == 'L') { // left - w lewo
      int separatorIndex = serialRequest.indexOf('-', 1);
      requestDistanceCm = serialRequest.substring(1, separatorIndex).toInt();
      requestAngle = serialRequest.substring(separatorIndex + 1).toInt();
      goLeft(requestAngle, requestDistanceCm);
    }
  }
}

void leftRotationIncrement() {
  leftChanges++;
}
void rightRotationIncrement() {
  rightChanges++;
}

// 20 holes = 65mm | 42 changes = 1 obrot = 20,41cm
void getLeftDistance(){
    leftDistanceCm = leftChanges / changesToCmFactor;
}

void getRightDistance() {
  rightDistanceCm = rightChanges / changesToCmFactor;
}

void cleanPins() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void goForward(int distanceCm) {
  goStraightLine(distanceCm, true);
}

void goBack(int distanceCm) {
  goStraightLine(distanceCm, false);
}

void goStraightLine(int distanceCm, bool isForward) {
  cleanPins();
  getLeftDistance(); // zapisujemy stan do globalnych
  getRightDistance();
  startingDistanceLeft = leftDistanceCm;

  if (isForward) { // jazda w przód różni się od jazdy w tył tylko pinami do aktywacji
    digitalWrite(IN1, HIGH);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
  }


  while ((leftDistanceCm - startingDistanceLeft < distanceCm) ) { // dopóki lewy silnik nie przebył zadanego dystansu
    differenceBetweenLeftAndRightMotorCm = leftDistanceCm - rightDistanceCm;

    powerRight = powerRight + changeFactor * differenceBetweenLeftAndRightMotorCm;
    powerLeft = powerLeft - changeFactor * differenceBetweenLeftAndRightMotorCm;

    powerRight = normalize(powerRight);
    powerLeft = normalize(powerLeft);

    analogWrite(ENB, powerRight);
    analogWrite(ENA, powerLeft);

    getLeftDistance(); // zapisujemy stan do globalnych
    getRightDistance();
  }
  cleanPins();
}

int normalize(int value) {
  if (value > 255) return 255;
  else if (value < 0) return 0;
  else return value;
}

void goRight(int angle, int cm) {
  turnRight(angle);
  goForward(cm);
}

void turnRight(int angle) {
  cleanPins();
  getLeftDistance(); // zapisujemy stan do globalnych
  getRightDistance();

  digitalWrite(IN1, HIGH); 

  startingDistanceLeft = leftDistanceCm; // Przy skręcie w prawo, lewe koło będzie się obracać
  angleInRadians = angle*radiansConversionFactor; // Zmiana stopni na radiany
  arcDistance = turningRadius*angleInRadians; // Obliczanie potrzebnego dystantu po łuku

  analogWrite(ENA, 150); // Uruchomienie lewego silnika
  while ((leftDistanceCm - startingDistanceLeft < arcDistance) ) {
    getLeftDistance(); // zapisujemy stan do globalnych
  }
  cleanPins();
}

void goLeft(int angle, int cm) {
  turnLeft(angle);
  goForward(cm);
}

void turnLeft(int angle) {
  cleanPins();
  getLeftDistance(); // zapisujemy stan do globalnych
  getRightDistance();

  digitalWrite(IN4, HIGH);

  startingDistanceRight = rightDistanceCm; // Przy skręcie w lewo, prawe koło będzie się obracać
  angleInRadians = angle*radiansConversionFactor; // Zmiana stopni na radiany
  arcDistance = turningRadius*angleInRadians; // Obliczanie potrzebnego dystantu po łuku

  analogWrite(ENB, 150); // Uruchomienie prawego silnika
  while ((rightDistanceCm - startingDistanceRight < arcDistance) ) {
    getRightDistance(); // zapisujemy stan do globalnych
  }
  cleanPins();
}
