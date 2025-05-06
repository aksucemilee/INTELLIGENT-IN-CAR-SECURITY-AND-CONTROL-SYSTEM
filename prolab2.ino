#include <LiquidCrystal.h>

// lcd pin yerlesimi : RS, E, D4, D5, D6, D7
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

// girisler
const int btnMotor = 3;
const int btnKemer = 2;
const int swKapi   = 4;
const int potYakit = A2;
const int ldrPin   = A1;
const int sicaklik = A0;

// LED’ler
const int ledKirmizi = 30;
const int ledMavi    = 31;
const int ledSari    = 32;
const int ledRGB_R   = 33;
const int ledRGB_G   = 34;
const int ledRGB_B   = 35;

const int buzzer = 28;

// motor cikislari
const int in1 = 9;
const int in2 = 10;
const int ena = 8;
const int in3 = 11;
const int in4 = 12;
const int enb = 13;

const int FAR_ESIGI = 921;
const unsigned long DISPLAY_DELAY = 150;

// blink kontrol 
unsigned long yakitBlink = 0;
unsigned long newLCD = 0;
bool yakitBlinkDurum = false;
bool motorDurumu = false;

void setup() {
  lcd.begin(16, 2);

  pinMode(btnMotor, INPUT_PULLUP);
  pinMode(btnKemer, INPUT_PULLUP);
  pinMode(swKapi,  INPUT_PULLUP);

  pinMode(ledKirmizi, OUTPUT);
  pinMode(ledMavi,    OUTPUT);
  pinMode(ledSari,    OUTPUT);
  pinMode(ledRGB_R,   OUTPUT);
  pinMode(ledRGB_G,   OUTPUT);
  pinMode(ledRGB_B,   OUTPUT);
  pinMode(buzzer,     OUTPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  int yakitRaw = analogRead(potYakit);
  int yakitYuzde = map(yakitRaw, 0, 1023, 0, 100);
  int rawTemp = analogRead(sicaklik);
  float derece = (rawTemp * 5.0 / 1023.0) * 100.0;
  int isik = analogRead(ldrPin);

  bool kapiKapali   = digitalRead(swKapi) == LOW;
  bool kemerTakili  = digitalRead(btnKemer) == LOW;
  bool motorButonu  = digitalRead(btnMotor) == LOW;

  //yakit bitme kontrolu
  if (yakitRaw <= 10) {
    motorDurumu = false;
    digitalWrite(in1, LOW); digitalWrite(in2, LOW); digitalWrite(ena, LOW);
    digitalWrite(in3, LOW); digitalWrite(in4, LOW); digitalWrite(enb, LOW);
    digitalWrite(ledSari, LOW);
    digitalWrite(ledKirmizi, LOW);
    digitalWrite(ledMavi, LOW);
    digitalWrite(ledRGB_R, LOW);
    digitalWrite(ledRGB_G, LOW);
    digitalWrite(ledRGB_B, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Yakit Bitti!");
    lcd.setCursor(0, 1);
    lcd.print("Motor Durdu");

    delay(DISPLAY_DELAY);
    return;
  }

  //blink sari led bagımsız
  if (yakitYuzde <= 5) {
    if (millis() - yakitBlink >= 50) {
      yakitBlinkDurum = !yakitBlinkDurum;
      digitalWrite(ledSari, yakitBlinkDurum ? HIGH : LOW);
      yakitBlink = millis();
    }
  } else if (yakitYuzde <= 10) {
    digitalWrite(ledSari, HIGH);
    yakitBlinkDurum = false;
  } else {
    digitalWrite(ledSari, LOW);
    yakitBlinkDurum = false;
  }

  //motor-kemer
  if (kapiKapali) {
    if (motorButonu) {
      if (!kemerTakili) {
        motorDurumu = false;
        digitalWrite(ledKirmizi, HIGH);
        digitalWrite(buzzer, HIGH);
        delay(500);
        digitalWrite(buzzer, LOW);
      } else {
        motorDurumu = true;
        digitalWrite(ledKirmizi, LOW);
        digitalWrite(buzzer, LOW);
      }
    } else {
      motorDurumu = false;
    }
  } else {
    motorDurumu = false;
  }

  // sicaklik-klima
  if (derece > 25.0) {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    digitalWrite(enb, HIGH);
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    digitalWrite(enb, LOW);
  }

  //far
  if (isik <= FAR_ESIGI) {
    digitalWrite(ledMavi, HIGH);
  } else {
    digitalWrite(ledMavi, LOW);
  }

  //motor calisma
  if (motorDurumu) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(ena, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(ena, LOW);
  }

  //kapi acik-pembe led
  if (!kapiKapali) {
    digitalWrite(ledRGB_R, HIGH);
    digitalWrite(ledRGB_G, LOW);
    digitalWrite(ledRGB_B, HIGH);
  } else {
    digitalWrite(ledRGB_R, LOW);
    digitalWrite(ledRGB_G, LOW);
    digitalWrite(ledRGB_B, LOW);
  }

  // lcd ekran guncellemeleri
  if (now - newLCD >= DISPLAY_DELAY) {
    lcd.clear();
    if (!kapiKapali) {
      lcd.setCursor(0, 0);
      lcd.print("UYARI: Kapi Acik!");
      lcd.setCursor(0, 1);
      lcd.print("Motor Calismaz");
    } else if (yakitYuzde <= 5) {
      lcd.setCursor(0, 0);
      lcd.print("Kritik: Yakit %");
      lcd.setCursor(0, 1);
      lcd.print(yakitYuzde);
    } else if (yakitYuzde <= 10) {
      lcd.setCursor(0, 0);
      lcd.print("Uyari: Yakit %");
      lcd.setCursor(0, 1);
      lcd.print(yakitYuzde);
    } else if (derece > 25.0) {
      lcd.setCursor(0, 0);
      lcd.print("Sicaklik:");
      lcd.print(derece, 1);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Klima Acildi");
    } else if (isik <= FAR_ESIGI) {
      lcd.setCursor(0, 0);
      lcd.print("Farlar Acik");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Farlar Kapandi");
    }
    newLCD = now;
  }
}
