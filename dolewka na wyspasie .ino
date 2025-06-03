#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/wdt.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // adres I2C 0x27, 16 znaków, 2 linie

const unsigned long SECOND = 1000;
const byte  sensor1 = 2; // czujnik 1
const byte  sensor2 = 4; // czujnik 2
const byte  relay = 8;   // przekaźnik

const unsigned long eventTime  = SECOND; // czas przerwania: 1 sekunda
unsigned long previousTime = 0;
const int maxDolewka  = 20; // max licznik dolewania

int odliczanie  = 0;
bool dolewaSie = false;
bool bezpiecznikBool = false;

// Zmienna do animacji
byte dots = 0;

// Zapamiętane ostatnie stany dla LCD (do eliminacji migania)
int lastS1 = -1, lastS2 = -1, lastOdliczanie = -1;
bool lastDolewaSie = false, lastBezpiecznik = false;
byte lastDots = 255;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  pinMode(relay, OUTPUT);
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  digitalWrite(relay, HIGH);

  wdt_enable(WDTO_2S);   // Watchdog: resetuje Arduino po 2 sekundach bez "karmienia"
}

void loop() {
  wdt_reset();

  monitorCzujnikow();
  dolewka();
  updateLCD();
  delay(300); // odświeżaj LCD co 0,3s
}

void monitorCzujnikow() {
  if (digitalRead(sensor1) == 0 && digitalRead(sensor2) == 0) {
    if (!bezpiecznikBool) {
      dolewaSie = true;
    }
  }
  if (digitalRead(sensor2) == 1) {
    dolewaSie = false;
  }
}

void dolewka() {
  if (dolewaSie) {
    if (odliczanie <= maxDolewka) {
      unsigned long currentTime = millis();
      if (currentTime - previousTime >= eventTime) {
        digitalWrite(relay, LOW); // załącz przekaźnik
        previousTime = currentTime;
        odliczanie++;
      }
    } else {
      digitalWrite(relay, HIGH); // wyłącz przekaźnik
      bezpiecznikBool = true;
      dolewaSie = false;
    }
  } else {
    digitalWrite(relay, HIGH);
    odliczanie = 0;
    dolewaSie = false;
  }
}

// -------- WYŚWIETLANIE --------

void updateLCD() {
  int s1 = digitalRead(sensor1);
  int s2 = digitalRead(sensor2);

  // Sprawdź, czy trzeba odświeżyć LCD (gdy zmieni się stan)
  bool czyZmiana =
    (s1 != lastS1) ||
    (s2 != lastS2) ||
    (dolewaSie != lastDolewaSie) ||
    (bezpiecznikBool != lastBezpiecznik) ||
    (odliczanie != lastOdliczanie) ||
    (dots != lastDots);

  if (czyZmiana) {
    lcd.clear();
    lastS1 = s1;
    lastS2 = s2;
    lastDolewaSie = dolewaSie;
    lastBezpiecznik = bezpiecznikBool;
    lastOdliczanie = odliczanie;
    lastDots = dots;

    // Linia 1: status i sensory
    lcd.setCursor(0, 0);
    lcd.print("S1:");
    lcd.print(s1);
    lcd.print(" S2:");
    lcd.print(s2);
    lcd.print(" ");

    if (bezpiecznikBool) {
      lcd.print("BLAD");
    } else if (dolewaSie) {
      lcd.print("DOL");
      for (int i=0; i<dots; i++) lcd.print(".");
    } else {
      lcd.print("CZEKAJ");
    }

    // Linia 2: licznik i pasek postępu
    lcd.setCursor(0, 1);
    if (dolewaSie) {
      lcd.print("Odl:");
      if (odliczanie < 10) lcd.print(" ");
      lcd.print(odliczanie);
      lcd.print(" [");
      int barLen = map(odliczanie, 0, maxDolewka, 0, 6); // pasek 6 znaków
      for (int i = 0; i < 6; i++) {
        if (i < barLen) lcd.print("#");
        else lcd.print(" ");
      }
      lcd.print("]");
    } else {
      lcd.print("Bzp:");
      lcd.print(bezpiecznikBool ? "ON " : "OFF");
      lcd.print(" Odl:");
      if (odliczanie < 10) lcd.print(" ");
      lcd.print(odliczanie);
      lcd.print("   ");
    }
  }
  if (dolewaSie) {
    dots = (dots + 1) % 4;
  }
}
