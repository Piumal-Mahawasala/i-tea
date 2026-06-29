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
#include <timer.h>

#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif


#define WEIGHT_THRESHOLD -50        // Temperature threshold for triggering the alarm (adjust as needed)
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

int counter = 20;
int forthCounter = counter/4;
int halfCounter = counter/2;
int threeForthCounter = halfCounter + forthCounter;
bool ended = false;

HX711_ADC LoadCell(D4, D3); // dt pin, sck pin
int taree = 6;
int a = 0;
float b = 0;



//------------------------------------------------------------------------------
void start_temperature_timer() {

  //pinMode (taree, INPUT_PULLUP);
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(1000); // load cells gets 1000ms of time to stabilize

  /////////////////////////////////////
  LoadCell.setCalFactor(375); // Calibarate your LOAD CELL with 100g weight, and change the value according to readings
  /////////////////////////////////////



  Wire.begin();
  Wire.setClock(400000L);

  light_start();

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0


  delay(2000);
  
}
//------------------------------------------------------------------------------


void temperature_alarm()
{
      Serial.print("Base Weight: ");
    Serial.println(baseWeight);
    while (ended == false) {
    
    light_go();

    unsigned long currentMillis = millis();

  oled.setFont(System5x7);
  oled.clear();
  oled.setCursor(0,2);
    oled.println("1. Boil water");
    oled.setCursor(0,4);
    oled.println("2. Put water in cup");
        oled.setCursor(0,6);
    oled.println("3. Put cup on top");

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

        if (currentWeight < WEIGHT_THRESHOLD && isAlarmActive == false)
        {
            full();
            displayTime(counter);
            Serial.println("Alarm triggered!");
            isAlarmActive = true;
            while(ended == false) {
                timer();
                light_stop();
            }
            
        }

    }

    delay(1000);
    }
     
}
