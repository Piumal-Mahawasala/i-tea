//Viral Science www.viralsciencecreativity.com www.youtube.com/c/viralscience
//Arduino Digital Weight Scale HX711 Load Cell Module
#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC
#include <Wire.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <light.h>
//#include <ct.h>

#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif


#define WEIGHT_THRESHOLD -20        // Temperature threshold for triggering the alarm (adjust as needed)
#define SAMPLE_INTERVAL 5000             // Interval between temperature/humidity readings in milliseconds (adjust as needed)

unsigned long previousMillis = 0;
unsigned long startMillis = 0;
bool isAlarmActive = false;

float baseWeight = 0.00;

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C 

// Define proper RST_PIN if required.
#define RST_PIN -1

#define SPEED_UP 1;

SSD1306AsciiWire oled;


int state = 0;  
int threshold = 20;
int counter = 5;
int forthCounter = counter/4;
int halfCounter = counter/2;
int threeForthCounter = halfCounter + forthCounter;


HX711_ADC LoadCell(D4, D3); // dt pin, sck pin
int taree = 6;
int a = 0;
float b = 0;




bool base = true;



#include <Arduino.h>

 

const int buzzerPin = D5; // Verbinde den Buzzer mit dem digitalen Pin D5

int buttonRight = D8; // rechter Knopf (kein Touch)
unsigned long lastPeriodStart;
const int onDuration = 1000;
const int periodDuration = 500;
bool alarm = true;

const int ledPin = D0; 

int ledState = LOW;

void start_buzzer()
{
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}


void buzzer()
{
  while (alarm == true)
  {
    if (millis() - lastPeriodStart >= periodDuration)
    {
      lastPeriodStart += periodDuration;

 

      if (ledState == LOW)
      {
        ledState = HIGH;
        digitalWrite(ledPin, ledState);
        tone(buzzerPin, 500, onDuration); // Spiele einen 550 Hz Ton für 'onDuration'
      }
      else
      {
        ledState = LOW;
        digitalWrite(ledPin, ledState);
        noTone(buzzerPin); // Schalte den Buzzer aus
      }
    }

 

    if (digitalRead(buttonRight) == HIGH)
    {
      alarm = false;
      digitalWrite(ledPin, LOW);
      noTone(buzzerPin); // Schalte den Buzzer aus
          isAlarmActive = false;
    state += 1;
    Serial.println("State after smiley");
    Serial.println(state);
    }
  }
}



void displayTime(int currentCounter) {
  if (currentCounter > 0) {
    int minutes = currentCounter / 60;
    int seconds = currentCounter % 60;
    int decSeconds = seconds / 10;
    int singSeconds = seconds % 10;
    oled.setCursor(37, 3);
    oled.setFont(lcdnums12x16);
    oled.print(minutes);
    oled.print(":");
    oled.print(decSeconds);
    oled.print(singSeconds);
  } else {
  oled.setCursor(1, 2);
  oled.print(" ***    Press    ***");
  oled.setCursor(1, 5);
  oled.print(" ***    enter    ***");
    buzzer();
  }
}

void full() {
  oled.setFont(System5x7);
  oled.setCursor(1, 0);
  oled.print("     ***********");
  oled.setCursor(1, 1);
  oled.print("   ***         ***");
  oled.setCursor(1, 2);
  oled.print(" ***             ***");
  oled.setCursor(1, 3);
  oled.print("**");
  oled.setCursor(19*6 + 1, 3);
  oled.print("**");
  oled.setCursor(1, 4);
  oled.print("**");
  oled.setCursor(19*6 + 1, 4);
  oled.print("**");
  oled.setCursor(1, 5);
  oled.print(" ***             ***");
  oled.setCursor(1, 6);
  oled.print("   ***         ***");
  oled.setCursor(1, 7);
  oled.print("     ***********");
}

void firstHalfEmpty() {
  oled.setFont(System5x7);
  oled.setCursor(1, 0);
  oled.print("          ");
  oled.setCursor(1, 1);
  oled.print("       ");
  oled.setCursor(1, 2);
  oled.print("    ");
  oled.setCursor(1, 3);
  oled.print("    ");
}

void secondHalfEmpty() {
  oled.setFont(System5x7);
  oled.setCursor(11*6 + 1, 0);
  oled.print("     ");
  oled.setCursor(15*6 + 1, 1);
  oled.print("   ");
  oled.setCursor(17*6 + 1, 2);
  oled.print("   ");
  oled.setCursor(19*6 + 1, 3);
  oled.print("  ");
}
void thirdHalfEmpty() {
  oled.setFont(System5x7);
  oled.setCursor(11*6 + 1, 7);
  oled.print("     ");
  oled.setCursor(15*6 + 1, 6);
  oled.print("   ");
  oled.setCursor(17*6 + 1, 5);
  oled.print("   ");
  oled.setCursor(19*6 + 1, 4);
  oled.print("  ");
}
void lastHalfEmpty() {
  oled.setFont(System5x7);
  oled.setCursor(1, 7);
  oled.print("          ");
  oled.setCursor(1, 6);
  oled.print("       ");
  oled.setCursor(1, 5);
  oled.print("    ");
  oled.setCursor(1, 4);
  oled.print("    ");
}
int getCounter() {
  int currentCount = counter;
  if (counter > 0) {
    counter = counter - SPEED_UP;
  }

  return currentCount;
}



//------------------------------------------------------------------------------
void start_timer() {

  //pinMode (taree, INPUT_PULLUP);
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(1000); // load cells gets 1000ms of time to stabilize

  /////////////////////////////////////
  LoadCell.setCalFactor(375); // Calibarate your LOAD CELL with 100g weight, and change the value according to readings
  /////////////////////////////////////




  Wire.begin();
  Wire.setClock(400000L);

  light_start();

  //pinMode(buzzerPin, OUTPUT); 

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0


  delay(2000);
  
}
//------------------------------------------------------------------------------

void timer() {

    int currentCounter = getCounter();
    
    oled.setFont(System5x7);
    oled.clear();
    oled.setCursor(0, 0);
    full();
    displayTime(currentCounter);
    delay(500);
    if (currentCounter > threeForthCounter) {
        secondHalfEmpty();
    } else if (currentCounter > halfCounter) {
      thirdHalfEmpty();
    } else if (currentCounter > forthCounter) {
      lastHalfEmpty();
    } else if (currentCounter > 0) {
      firstHalfEmpty();
    }
    
    //displayTime(currentCounter);
    delay(500);

}



void weight_timer()
{
    
    while (state == 0) {

    while(base == true) {
    LoadCell.update(); // retrieves data from the load cell
    float baseWeight = LoadCell.getData(); // get output value
    
    Serial.print("Base Weight: ");
    Serial.println(baseWeight);
    base = false;

    }
 

    oled.setFont(System5x7);
    oled.clear();
    oled.setCursor(0,2);
    oled.println("1. Boil water");
    oled.setCursor(0,4);
    oled.println("2. Put water in cup");
    oled.setCursor(0,6);
    oled.println("3. Put cup on top");

   /*
    threshold = 50;
    counter = calculate_time_to_wait(80);
    forthCounter = counter/4;
    halfCounter = counter/2;
    threeForthCounter = halfCounter + forthCounter;
    Serial.println(counter);
   
   */ 

    light_go();

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLE_INTERVAL)
    {
         LoadCell.update(); // retrieves data from the load cell
        float currentWeight = LoadCell.getData(); // get output value

        if (isnan(currentWeight))
        {
            Serial.println("Failed to read weight!");
            return;
        }

        Serial.print("Weight: ");
        Serial.println(currentWeight);

        if (currentWeight < baseWeight - threshold && isAlarmActive == false)
        {
            full();
            displayTime(counter);
            Serial.println("Alarm triggered!");
            isAlarmActive = true;
            while(state == 0) {
                timer();
                light_stop();
            }
            
        }

    }

    delay(1000);

    }

  if (state == 1) {
    oled.setFont(System5x7);
    oled.clear();
    oled.setCursor(0,3);
    oled.println("1. Put tea in cup");
    oled.setCursor(0,5);    
    oled.println("2. Press enter");

    alarm = true;

  }
    
    while (state == 1) {

    counter = 10;
    forthCounter = counter/4;
    halfCounter = counter/2;
    threeForthCounter = halfCounter + forthCounter;

    //light_go();

  if (digitalRead(buttonRight) == HIGH) {
            full();
            displayTime(counter);
            Serial.println("Alarm triggered!");
            isAlarmActive = true;
            while(state == 1) {
                timer();
                light_stop();
            }
      }

          delay(500);

    }

    if (state == 2) {
      counter = 8;
    
    oled.setFont(System5x7);
    oled.clear();
      oled.setCursor(0,3);
      oled.println("Remove the tea leaves");
      oled.setCursor(0,5);  
      oled.println("and enjoy!");

    alarm = true;

    }

    while (state == 2) {

      int drinkTime = getCounter();

      if (drinkTime > 0) {
      Serial.println(drinkTime);
      delay(1000);
      } else {
      oled.setFont(System5x7);
      oled.clear();
      oled.setCursor(0,3);
      oled.println("Don't forget to drink");
      oled.setCursor(0,5);  
      oled.println("your tea!");

      digitalWrite(ledPin, HIGH);
      tone(buzzerPin, 500); // Spiele einen 550 Hz Ton für 'onDuration'
      delay(3000);

      digitalWrite(ledPin, LOW);
      noTone(buzzerPin);


      state = 3;
      }
    }

      if (state == 3) {
        
        digitalWrite(ledPin, LOW);


      }

    delay(1000);
    }










