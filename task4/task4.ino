#define trigPin 11
#define echoPin 12
long duration;
int distance;

int distIdx;
int sliskoscIdx;
int sliskoscVal;
bool czySlisko;
double sliskoscDelay = 1000;

double minSliskoBlisko;

double odciete[10];

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
double stop[10] =    {1.0, 0.9, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};


int predkosc[10] =  {0, 90, 110, 130, 150, 170, 190, 210, 230, 250};

int ENA = 5; // left motor
int ENB = 6;

int IN1 = A0;
int IN2 = A1;
int IN3 = A2;
int IN4 = A3;

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  Serial.begin(9600);

  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);

  sliskoscIdx = sliskoscDelay / 500;
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

  // blisko i slisko => 0
  // blisko i nslisko => 0
  // daleko i slisko => srednio
  // daleko i nslisko => szybko

  distIdx = (min(distance, 350) - 50) / 20;

  if (blisko[distIdx] > daleko[distIdx]) {
    // jest blisko
    minSliskoBlisko = min(sliskoscVal, blisko[distIdx]);
    
    odetnij(stop, minSliskoBlisko);
    mocSilnika = sumujILiczSrodekCiezkosci(odciete, srednio, szybko);
  } else {
    // jest daleko
    minSliskoBlisko = min(sliskoscVal, daleko[distIdx]);
    if (czySlisko) {
        odetnij(srednio, minSliskoBlisko);
        mocSilnika = sumujILiczSrodekCiezkosci(stop, odciete, szybko);
    } else {
        odetnij(szybko, minSliskoBlisko);
        mocSilnika = sumujILiczSrodekCiezkosci(stop, srednio, odciete);
    }
  }

  analogWrite(ENA, mocSilnika);
  analogWrite(ENB, mocSilnika);

  Serial.print("Moc silnika: ");
  Serial.println(mocSilnika);
}

void odetnij(double arr[], double threshold) {

  for (int i = 0; i< 10; i++) {
    odciete[i] = min(arr[i], threshold);
  }
}

int sumujILiczSrodekCiezkosci(double s1[], double s2[], double s3[]) {

  double licznik = 0;
  double mianownik = 0;
  for (int i = 0; i< 10; i++) {
    licznik += max(s1[i], max(s2[i], s3[i])) * predkosc[i];
    mianownik +=  max(s1[i], max(s2[i], s3[i]));
  }
  return licznik / mianownik;
}

