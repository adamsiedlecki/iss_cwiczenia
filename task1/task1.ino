


int ENA = 5; // left motor
int ENB = 6;

int IN1 = A0;
int IN2 = A1;
int IN3 = A2;
int IN4 = A3;

int powerLeft = 150;
int powerRight = 130;

int leftChanges = 0;
int rightChanges = 0;

const float changesToCmFactor = 857.22; // 42.0 *20.41;

double changesCmRatio = 20.41 / 42.0;

double  leftDistanceCm = 0;
double  rightDistanceCm = 0;

float difference = 0;

const float turningRadius = 11.7; // TBD potrzebna odległość koła od punktu obrotu, musi być w cm
float angleInRadians = 0; // Zmienna do przechowywania kąta skrętu w radianach
const float radiansConversionFactor = 3.14/180; // Stała do przeliczania stopni na radiany
float arcDistance = 0; //Zmienna do przechowywania dystansu potrzebnego do przejechania żeby uzyskać odpowiedni kąt skrętu

float startingDistanceLeft = 0;
float startingDistanceRight = 0;

int turningPower = 110;


String serialRequest;
int requestAngle;
int requestDistanceCm;

void setup() {
  Serial.begin(9600);
  analogWrite(ENA, powerLeft);
  analogWrite(ENB, powerRight);

  attachInterrupt(digitalPinToInterrupt(2), leftRotationIncrement, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), rightRotationIncrement, CHANGE);

  //goForward(200);
  //turnRight(90);

  // Serial.print("left distance cm: ");
  // Serial.print(leftDistanceCm);
  
  // Serial.print("right distance cm: ");
  // Serial.print(rightDistanceCm);

  //delay(12000); // na odpiecie kabla
}

void leftRotationIncrement() {
  leftChanges++;
}
void rightRotationIncrement() {
  rightChanges++;
}

void loop() {
  checkSerialForRequests();
}

void checkSerialForRequests() {
  if (Serial.available() > 0) { // Serial.available() > 0
    // Opis protokołu:
    // arduino przymuje polecenia na Serial zakończone znakiem &.
    // Polecenia można łączyć.
    // przykładowo: F10&R90-10&B5&
    //
    // F100& - jazda 100 cm do przodu
    // B100& - jazda 100 cm do tyłu
    // R90-100& - skręt o 90 stopni w prawo i przejechanie 100cm
    // L90-100& - skręt o 90 stopni w lewo i przejechanie 100cm

    // F190&R90-195&L90-200&& - wyjazd z sali
        // F190&R80-195&L90-200&& - wyjazd z sali z korekta
    serialRequest = Serial.readStringUntil("&&"); // znak & kończy polecenie - jeśli go nie ma, to nie czytamy
    Serial.println(serialRequest);
    delay(10000); //na odpiecie kabla

    while(serialRequest.length() > 1) {
      int singleCommandEndPos = serialRequest.indexOf("&") +1;
      Serial.println(serialRequest.substring(0, singleCommandEndPos));
        if (serialRequest[0] == 'F') { // forward - do przodu
      
          requestDistanceCm = serialRequest.substring(1, singleCommandEndPos).toInt();
          goForward(requestDistanceCm);
        } else if (serialRequest[0] == 'B') { // back - do tyłu
          // requestDistanceCm = serialRequest.substring(1).toInt();
          // goBack(requestDistanceCm);
        } else if (serialRequest[0] == 'R') { // right - w prawo
          int separatorIndex = serialRequest.indexOf('-', 1);
          requestAngle = serialRequest.substring(1, separatorIndex).toInt();

          requestDistanceCm = serialRequest.substring(separatorIndex + 1, singleCommandEndPos).toInt();
          Serial.print("requestDistanceCm: ");
          Serial.println(requestDistanceCm);
          goRight(requestAngle, requestDistanceCm);
        } else if (serialRequest[0] == 'L') { // left - w lewo
          int separatorIndex = serialRequest.indexOf('-', 1);
          requestAngle = serialRequest.substring(1, separatorIndex).toInt();
          requestDistanceCm = serialRequest.substring(separatorIndex + 1, singleCommandEndPos).toInt();
          goLeft(requestAngle, requestDistanceCm);
        }
        serialRequest = serialRequest.substring(singleCommandEndPos);

        powerLeft = 100;
        powerRight = 100;
      }
    }
}

// 20 holes = 65mm | 42 changes = 1 obrot = 20,41cm
void getLeftDistance(){
  leftDistanceCm = leftChanges * changesCmRatio;
}

void getRightDistance() {
  rightDistanceCm = rightChanges * changesCmRatio;
}


void cleanPins() {
  rightChanges = 0;
  leftChanges = 0;
  leftDistanceCm = 0;
  rightDistanceCm = 0;
  analogWrite(ENB, 0);
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void goForward(int distanceCm) {
  cleanPins();
  float changeFactor = 2;
  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, HIGH);
  

  while ((leftDistanceCm <distanceCm) ) {
    leftDistanceCm = leftChanges * changesCmRatio;
    rightDistanceCm = rightChanges * changesCmRatio;
    difference = leftDistanceCm - rightDistanceCm;

    // powerRight = powerRight + changeFactor*(difference);
    // powerLeft = powerLeft - changeFactor*(difference);
    
    // powerRight = normalize(powerRight);
    // powerLeft = normalize(powerLeft);

    analogWrite(ENB, normalize(powerRight + changeFactor*(difference)));
    analogWrite(ENA, normalize(powerLeft - changeFactor*(difference)));

    // Serial.print("left distance cm: ");
    // Serial.print(leftDistanceCm);
  
    // Serial.print("right distance cm: ");
    // Serial.print(rightDistanceCm);
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

void goRight(int angle, int cm) {
  turnRight(angle);
  goForward(cm);
}

void turnRight(int angle) {
  cleanPins();
  getLeftDistance(); // zapisujemy stan do globalnych
  getRightDistance();

  digitalWrite(IN1, HIGH); 

  
  angleInRadians = angle*radiansConversionFactor; // Zmiana stopni na radiany
  arcDistance = turningRadius*angleInRadians; // Obliczanie potrzebnego dystantu po łuku

  analogWrite(ENA, turningPower); // Uruchomienie lewego silnika
  while (leftDistanceCm < arcDistance) {
    leftDistanceCm = leftChanges * changesCmRatio;
    Serial.println(leftDistanceCm);
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

  angleInRadians = angle*radiansConversionFactor; // Zmiana stopni na radiany
  arcDistance = turningRadius*angleInRadians; // Obliczanie potrzebnego dystantu po łuku

  analogWrite(ENB, turningPower); // Uruchomienie prawego silnika
  while (rightDistanceCm < arcDistance ) {
    rightDistanceCm = rightChanges * changesCmRatio;
    Serial.println(rightDistanceCm);
  }
  cleanPins();
}