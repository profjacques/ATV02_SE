#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD

const int trigPin = 9, echoPin = 10;
const int tempPin = A0; // Pino analógico para o TMP36

float tempLimit = 25.0;
float distLimit = 20.0; // cm

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Leitura do TMP36
  int tempReading = analogRead(tempPin);
  float voltage = tempReading * (5.0 / 1023.0);
  float temp = (voltage - 0.5) * 100.0;

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
    analogWrite(3, 255); // Vermelho
    tone(11, 1000);
  } else if(distance < distLimit) {
    analogWrite(6, 255); // Azul
    tone(11, 500);
  } else {
    analogWrite(5, 255); // Verde
    noTone(11);
  }

  // Atualização do LCD
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print("C");

  lcd.setCursor(0,1);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print("cm   ");

  delay(1000); // Atualização a cada 1 segundo
}
