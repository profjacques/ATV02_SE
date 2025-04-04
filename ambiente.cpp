#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

// Configuração dos pinos
const int TMP36_PIN = A0;
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BUTTON_PIN = 7;    // Botão com resistor externo
const int RED_PIN = 3;
const int GREEN_PIN = 5;
const int BLUE_PIN = 6;
const int LED_PIN = 13;      // LED de confirmação

// Variáveis de estado
enum Mode { NORMAL, ADJUST_TEMP, ADJUST_DIST };
Mode currentMode = NORMAL;

// Variáveis de temperatura
float currentTemperature = 0;
float tempLimit = 30.0;
float liveLimit = 30.0;      // Limite temporário
bool adjustingTemp = false;

// Variáveis de distância
int currentDistance = 0;
int distLimit = 20;
int liveDistLimit = 20;      // Limite temporário
bool adjustingDist = false;

// Controle do botão
unsigned long lastButtonPress = 0;
const int debounceDelay = 200;  // Debounce de 200ms

void setup() {
    Serial.begin(9600);
    
    pinMode(BUTTON_PIN, INPUT);  // SEM PULLUP INTERNO
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    lcd.init();
    lcd.backlight();
    
    lcd.setCursor(0, 0);
    lcd.print("Inicializando...");
    delay(1000);
    lcd.clear();
}

void loop() {
    // Leitura dos sensores
    currentTemperature = readPreciseTemperature();
    currentDistance = readDistance();

    // Verificação do botão com debounce
    if (digitalRead(BUTTON_PIN) == LOW && (millis() - lastButtonPress) > debounceDelay) {
        lastButtonPress = millis();
        
        switch(currentMode) {
            case NORMAL:
                currentMode = ADJUST_TEMP;
                adjustingTemp = true;
                liveLimit = currentTemperature;
                break;
                
            case ADJUST_TEMP:
                // Sai do modo de ajuste e salva
                tempLimit = liveLimit;
                adjustingTemp = false;
                currentMode = ADJUST_DIST;
                adjustingDist = true;
                liveDistLimit = currentDistance;
                digitalWrite(LED_PIN, HIGH);
                delay(200);
                digitalWrite(LED_PIN, LOW);
                break;
                
            case ADJUST_DIST:
                // Sai do modo de ajuste e salva
                distLimit = liveDistLimit;
                adjustingDist = false;
                currentMode = NORMAL;
                digitalWrite(LED_PIN, HIGH);
                delay(200);
                digitalWrite(LED_PIN, LOW);
                break;
        }
        lcd.clear();
    }

    // Atualização do display e LEDs
    switch(currentMode) {
        case NORMAL:
            displayNormalMode();
            break;
            
        case ADJUST_TEMP:
            if(adjustingTemp) {
                liveLimit = currentTemperature; // Acompanha em tempo real
            }
            displayAdjustTempMode();
            break;
            
        case ADJUST_DIST:
            if(adjustingDist) {
                liveDistLimit = currentDistance; // Acompanha em tempo real
            }
            displayAdjustDistMode();
            break;
    }

    // Controle dos LEDs
    updateLEDs();
    
    delay(100);
}

void displayNormalMode() {
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(currentTemperature, 1);
    lcd.print("C D:");
    lcd.print(currentDistance);
    lcd.print("cm");
    
    lcd.setCursor(0, 1);
    lcd.print("L:");
    lcd.print(tempLimit, 1);
    lcd.print("C ");
    lcd.print(distLimit);
    lcd.print("cm");
}

void displayAdjustTempMode() {
    lcd.setCursor(0, 0);
    lcd.print("AJUSTE TEMP");
    
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(currentTemperature, 1);
    lcd.print("C Lim:");
    lcd.print(liveLimit, 1);
    lcd.print("C");
}

void displayAdjustDistMode() {
    lcd.setCursor(0, 0);
    lcd.print("AJUSTE DIST");
    
    lcd.setCursor(0, 1);
    lcd.print("D:");
    lcd.print(currentDistance);
    lcd.print("cm Lim:");
    lcd.print(liveDistLimit);
    lcd.print("cm");
}

void updateLEDs() {
    if(currentMode == NORMAL) {
        digitalWrite(GREEN_PIN, currentTemperature <= tempLimit && currentDistance >= distLimit);
        digitalWrite(RED_PIN, currentTemperature > tempLimit);
        digitalWrite(BLUE_PIN, currentDistance < distLimit);
    }
    else if(currentMode == ADJUST_TEMP) {
        digitalWrite(RED_PIN, (millis()/500) % 2);
        digitalWrite(GREEN_PIN, LOW);
        digitalWrite(BLUE_PIN, LOW);
    }
    else if(currentMode == ADJUST_DIST) {
        digitalWrite(BLUE_PIN, (millis()/500) % 2);
        digitalWrite(RED_PIN, LOW);
        digitalWrite(GREEN_PIN, LOW);
    }
}

float readPreciseTemperature() {
    // Média móvel para maior precisão
    static float readings[5];
    static byte index = 0;
    
    int raw = analogRead(TMP36_PIN);
    readings[index] = (raw * (5.0 / 1023.0) - 0.5) * 100.0;
    index = (index + 1) % 5;
    
    float sum = 0;
    for (byte i = 0; i < 5; i++) {
        sum += readings[i];
    }
    return sum / 5.0;
}

int readDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.034 / 2;
}
