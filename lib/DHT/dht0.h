#ifndef MY_DHT_H
#define MY_DHT_H
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// DHT
#define DHTPIN D5     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // This defines what kind of sensor is read.  We are using DHT22

void setup()
{
    dht.begin(); // Initialize the DHT sensor
}
void loop()
{

    //********Temperature &Humidity *******
                 delay(2000); // Wait a few seconds between measurements. Until the sensor is read again.  Since the DHT22 also has about 2 seconds to update its values doesn't make sense to read it faster!

    // read temperature and humidity
    float h = dht.readHumidity();    // Read the humidity and store it in the variable h
    float t = dht.readTemperature(); // Read the temperature in °C and store it in the variable t

    if (isnan(h) || isnan(t))
    { // Check if everything was read correctly
        Serial.println("Error reading sensor!");
        return;
    }
    /*
      // Print on the Serial Monitor for DHT

        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.println(" *C");

        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.println(" %");

        delay(1000);
      */

    // Show on the OLED display
    display.clearDisplay(); // Clear display

    // display temperature
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Temperature: ");
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print(t);
    display.print(" ");
    display.setTextSize(1);
    display.cp437(true);
    display.write(167);
    display.setTextSize(2);
    display.print("C");

    // display humidity
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print("Humidity: ");
    display.setTextSize(2);
    display.setCursor(0, 45);
    display.print(h);
    display.print(" %");

    display.display(); // Make the actual printing happen on the hardware

    delay(1000); // new values of sensor will be sent to Arduino serial monitor after every one second
}
#endif