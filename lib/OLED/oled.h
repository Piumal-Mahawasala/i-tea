#ifndef MY_OLED_H
#define MY_OLED_H
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <dht.h>

// OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET D2    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

//******************************* Initialize **************************************

DHT dht(DHTPIN, DHTTYPE); // initalize DHT

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // initialize the OLED display

void setup()
{
    Serial.begin(9600); // initialize the Serial Monitor at a baud rate of 9600 for debugging purposes
    Serial.println(" ");
    Serial.println("System initialized!");

    // OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    { // initialize the display
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    delay(2000);              // Add a delay to give time for the display to initialize
    display.clearDisplay();      // clear the display
    display.setTextColor(WHITE); // set the text color to white
}

#endif