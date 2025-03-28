#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD

const int trigPin = 9, echoPin = 10;
const int buttonPin = 7, buzzerPin = 11;
const int rgbPins[] = {3, 5, 6};

float tempLimit = 25.0;
float distLimit = 20.0; // cm

void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // Leitura do DHT11
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature();

  // Leitura do HC-SR04
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.0343 / 2;

  // Controle do LED RGB e Buzzer
  if(temp > tempLimit) {
    analogWrite(rgbPins[0], 255); // Vermelho
    tone(buzzerPin, 1000);
  } else if(distance < distLimit) {
    analogWrite(rgbPins[2], 255); // Azul
    tone(buzzerPin, 500);
  } else {
    analogWrite(rgbPins[1], 255); // Verde
    noTone(buzzerPin);
  }

  // Atualização do LCD
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print("C H:");
  lcd.print(humidity);
  lcd.print("%");
 
  lcd.setCursor(0,1);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print("cm   ");

  delay(1000); // Atualização a cada 1 segundo
}
