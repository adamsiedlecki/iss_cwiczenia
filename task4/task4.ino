#define trigPin 11
#define echoPin 12
long duration;
int distance;

int distIdx;
int sliskoscIdx;
int sliskoscVal;
bool czySlisko;
double sliskoscDelay = 0;

double minBlisko;
double minDaleko;

double odciete1[10];
double odciete2[10];

int mocSilnika;

// odleglosc = indeks * 20 + 50
//                1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
double blisko[18] = {1.0, 1.0, 0.9, 0.9, 0.9, 0.8, 0.8, 0.7, 0.7, 0.6, 0.6, 0.5, 0.2, 0.0, 0.0, 0.0, 0.0, 0.0};
double daleko[18] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.2, 0.5, 0.6, 0.6, 0.7, 0.7, 0.8, 0.8, 0.9, 0.9, 0.9, 1.0, 1.0};

// indeks = 500ms
double nslisko[10]= {1.0, 0.9, 0.7, 0.5, 0.3, 0.2, 0.1, 0.0, 0.0, 0.0};
double slisko[10] = {0.0, 0.0, 0.0, 0.1, 0.2, 0.3, 0.5, 0.7, 0.9, 1.0};

// min = 90
double srednio[10] = {0.0, 0.0, 0.0, 0.3, 0.5, 0.7, 0.9, 1.0, 0.0, 0.0};
double szybko[10] =  {0.0, 0.0, 0.0, 0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 1.0};
double stop[10] =    {1.0, 0.9, 0.7, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

int predkosc[10] =  {0, 90, 110, 130, 150, 170, 190, 210, 230, 250};

int ENA = 5; // left motor
int ENB = 6;

int IN1 = A0;
int IN2 = A1;
int IN3 = A2;
int IN4 = A3;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  Serial.begin(9600);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN4, HIGH);

  sliskoscIdx = min(9, sliskoscDelay / 500);
  sliskoscVal = max(nslisko[sliskoscIdx], slisko[sliskoscIdx]);
  czySlisko = slisko[sliskoscIdx] > nslisko[sliskoscIdx];
}

void loop() {
  // Trigger the sensor by setting the trigPin high for 10 microseconds:
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance:
  distance = duration * 0.034 / 2; // distance in cm
  Serial.print(distance);
  Serial.println(" cm");

  distIdx = max(0, min(17, (distance - 50) / 20)); // Ograniczenie indeksu od 0 do 17

  minBlisko = min(sliskoscVal, blisko[distIdx]);
  minDaleko = min(sliskoscVal, daleko[distIdx]);

  if (blisko[distIdx] > daleko[distIdx]) {  // Blisko
    if (czySlisko) {
      odetnij(stop, srednio, minBlisko);
      mocSilnika = sumujILiczSrodekCiezkosci(odciete1, odciete2, srednio);
      Serial.println("Blisko i slisko: bardzo wolno");
    } else {
      odetnij(srednio, szybko, minBlisko);
      mocSilnika = sumujILiczSrodekCiezkosci(stop, odciete1, odciete2);
      Serial.println("Blisko i nieslisko: wolno");
    }
  } else {  // Daleko
    if (czySlisko) {
      odetnij(stop, szybko, minDaleko);
      mocSilnika = sumujILiczSrodekCiezkosci(odciete1, srednio, odciete2);
      Serial.println("Daleko i slisko: średnio");
    } else {
      odetnij(stop, srednio, minDaleko);
      mocSilnika = sumujILiczSrodekCiezkosci(odciete1, odciete2, szybko);
      Serial.println("Daleko i nieslisko: szybko");
    }
  }

  analogWrite(ENA, mocSilnika);
  analogWrite(ENB, mocSilnika);

  Serial.print("Moc silnika: ");
  Serial.println(mocSilnika);
}

int sumujILiczSrodekCiezkosci(double s1[], double s2[], double s3[]) {
  double licznik = 0;
  double mianownik = 0;
  for (int i = 0; i < 10; i++) {
    double maxVal = max(s1[i], max(s2[i], s3[i]));
    licznik += maxVal * predkosc[i];
    mianownik += maxVal;
  }
  if (mianownik == 0) return 0;  // Zabezpieczenie przed dzieleniem przez 0
  return licznik / mianownik;
}

void odetnij(double arr1[], double arr2[], double threshold) {

  for (int i = 0; i< 10; i++) {
    odciete1[i] = min(arr1[i], threshold);
    odciete2[i] = min(arr2[i], threshold);
  }
}


