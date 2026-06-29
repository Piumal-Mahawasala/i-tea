// Test for minimum program size.

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>


// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C 

// Define proper RST_PIN if required.
#define RST_PIN -1

#define SPEED_UP 1;

SSD1306AsciiWire oled;

int counter = 120;
bool ended = false;


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
    oled.setCursor(37, 2);
    oled.print("  #    #");
    oled.setCursor(37, 3);
    oled.print("         ");
    oled.setCursor(35, 4);
    oled.print("         ");
    oled.setCursor(39, 5);
    oled.print("  \\___/");
    ended = true;
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
  Wire.begin();
  Wire.setClock(400000L);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  full();
  displayTime(counter);
  delay(2000);
  
}
//------------------------------------------------------------------------------
void timer() {
  if (!ended) {
    int currentCounter = getCounter();
    
    oled.setFont(System5x7);
    oled.setCursor(0, 0);
    full();
    displayTime(currentCounter);
    delay(500);
    if (currentCounter > 90) {
      firstHalfEmpty();
    } else if (currentCounter > 60) {
      secondHalfEmpty();
    } else if (currentCounter > 30) {
      thirdHalfEmpty();
    } else if (currentCounter > 0) {
      lastHalfEmpty();
    }
    
    displayTime(currentCounter);
    delay(500);
  }
}
