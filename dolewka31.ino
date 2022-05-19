
const unsigned long SECOND = 1000;
const unsigned long HOUR = 3600 * SECOND;

const byte  sensor1 = 5; //sensor 1
const byte  sensor2 =  4; // sensor 2
const byte  relay =  6; // przekaznik

const unsigned long eventTime  = SECOND; //1 sek
unsigned long previousTime = 0;

const unsigned long eventTime_1  = SECOND; //1 sek
unsigned long previousTime_1 = 0;

const unsigned long eventTime_2  = SECOND; //1 sek
unsigned long previousTime_2 = 0;

const unsigned long eventTime_3  = SECOND; //1 sek
unsigned long previousTime_3 = 0;


int odliczanie  = 0;
int odliczanie2  = 0;
int bespiecznikOdliczanie=0;
int absolutneMax=40;
int odliczanieDopelnianiaKrotkie  = 0;
int odliczanieDopelnianiaDlugie  = 0;

int maxDlugie = 15;
int maxKrotkie = 10;

bool dolewaSie = false;
bool dopelnianieKrotkie = false;
bool dopelnianieDlugie = false;
 bool bezpiecznikBool =false;
const int maxDolewka  = 20; //czas razy 2





///////////////// wyswietlacz
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(115200);

  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);

  digitalWrite(relay, HIGH);
  pinMode(relay, OUTPUT);

  //led
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);


  //////rysowanie
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }



}

void loop() {
  monitorCzujnikow();
  dolewka();
  bezpiecznik();
  displayONE();
  
}
void monitorCzujnikow() {
  if (digitalRead(sensor1) == 0 && digitalRead(sensor2) == 0) {
    dolewaSie = true;
    bezpiecznikBool=true;

  }
  if (digitalRead(sensor2) == 1) {
        dolewaSie=false;
   
  }
}



void dolewka() {
  if (dolewaSie) {
 
    if (odliczanie <= maxDolewka) {
      unsigned long currentTime = millis();
  
      if (currentTime - previousTime >= eventTime)
      {
      
        digitalWrite(9, LOW); // led
        digitalWrite(10, HIGH);
        digitalWrite(relay, LOW);
        previousTime = currentTime;
        odliczanie++;
      }
    } else {

      digitalWrite(9, LOW); // led
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(relay, HIGH);
      dolewaSie = false;
        odliczanie = 0;
    }
  } else {

      digitalWrite(9, LOW); // led
      digitalWrite(10, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(relay, HIGH);
       dolewaSie = false;
         odliczanie = 0;
    }

}






void displayONE() {             // temperature comes in as a float with 2 decimal places

  // set up OLED text size and print the temperature data
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(" Sensor ");
  display.print(digitalRead(sensor1));
  display.print(" Sensor ");
  display.println(digitalRead(sensor2));
  display.print(" Odliczanie ");
  display.println(odliczanie);
  display.print(" Dolewa sie ");
    display.println(dolewaSie);
    display.print(" Krot  ");
      display.println(odliczanieDopelnianiaKrotkie);
      display.print(" dlugie ");
        display.println(odliczanieDopelnianiaDlugie);
      display.print(" bezpieczne ");
        display.println(bespiecznikOdliczanie);


  display.display();                    // update the OLED display with all the new text
}

void bezpiecznik(){
  
  if( bezpiecznikBool){
      unsigned long currentTime_3= millis();
      
    if (currentTime_3 - previousTime_3 >= eventTime_3) {
      bespiecznikOdliczanie++;
              previousTime_3 = currentTime_3;
            if (bespiecznikOdliczanie >= absolutneMax)
      {



        
        digitalWrite(9, HIGH); // led
        digitalWrite(10, HIGH);
        digitalWrite(11, HIGH);
        digitalWrite(relay, HIGH);
        odliczanieDopelnianiaDlugie = 0;
        dolewaSie = false;
        odliczanie = 0;
        dopelnianieDlugie = false;
        
      }
      
      }
    
    
    }
  
  
  
  
  }
