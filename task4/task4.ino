#define trigPin 11
#define echoPin 12
long duration;
int distance;
double sliskoscDelay = 1000;
int mocSilnika;
int IN1 = A0;
int IN2 = A1;
int IN3 = A2;
int IN4 = A3;
float blisko;
float daleko;
float slisko;
float nieSlisko;
// Reguły sterowania
float zaplonBliskoSlisko;
float zaplonBliskoNieSlisko;
float zaplonDalekoSlisko;
float zaplonDalekoNieSlisko;
// Wyjścia dla każdej reguły
int predkoscBliskoSlisko = 0;    // Stop
int predkoscBliskoNieSlisko = 50; // Wolno
int predkoscDalekoSlisko = 150;  // Średnio
int predkoscDalekoNieSlisko = 255; // Szybko
float sumaZaplonow;



void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    Serial.begin(9600);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN4, HIGH);
}

void loop() {
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    // Calculate the distance:
    distance = duration * 0.034 / 2; // distance in cm
    blisko = funkcjaBlisko(distance);
    daleko = funkcjaDaleko(distance);
    slisko = funkcjaSlisko(sliskoscDelay);
    nieSlisko = funkcjaNieSlisko(sliskoscDelay);
    // Reguły sterowania
    zaplonBliskoSlisko = min(blisko, slisko); // blisko i slisko to stop
    zaplonBliskoNieSlisko = min(blisko, nieSlisko); // blisko i nieslisko to wolno
    zaplonDalekoSlisko = min(daleko, slisko); // daleko i slisko to srednio
    zaplonDalekoNieSlisko = min(daleko, nieSlisko); // daleko i nieslisko to szybko
    // tutaj jest miejsce w którym nam czegoś brakuje - zaprezentowano na ćwiczeniach
    sumaZaplonow = zaplonBliskoSlisko + zaplonBliskoNieSlisko + zaplonDalekoSlisko + zaplonDalekoNieSlisko;
    if (sumaZaplonow == 0) {
        mocSilnika = 0;
    } else {
        mocSilnika = (
            zaplonBliskoSlisko * predkoscBliskoSlisko +
            zaplonBliskoNieSlisko * predkoscBliskoNieSlisko +
            zaplonDalekoSlisko * predkoscDalekoSlisko +
            zaplonDalekoNieSlisko * predkoscDalekoNieSlisko
        ) / sumaZaplonow;
    }


    analogWrite(5, mocSilnika * 0.85); // lewy silnik jest mocniejszy
    analogWrite(6, mocSilnika);
    Serial.print("Odległość: ");
    Serial.print(distance);
    Serial.print(" cm, Prędkość: ");
    Serial.println(mocSilnika);
    //delay(100);
}
float funkcjaBlisko(int dystans) {
    if (dystans <= 50) return 1.0;
    if (dystans >= 120) return 0.0;
    return (120.0 - dystans) / 70.0;
}
float funkcjaDaleko(int dystans) {
    if (dystans <= 50) return 0.0;
    if (dystans >= 120) return 1.0;
    return (dystans - 50.0) / 70.0;
}
float funkcjaSlisko(double opoznienie) {
    if (opoznienie <= 0) return 0.0;
    if (opoznienie >= 5000) return 1.0;
    return opoznienie / 500.0;
}
float funkcjaNieSlisko(double opoznienie) {
    if (opoznienie <= 0) return 1.0;
    if (opoznienie >= 5000) return 0.0;
    return (5000.0 - opoznienie) / 500.0;
}