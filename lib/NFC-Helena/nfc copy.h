#if 0
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
#else

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#include <iostream>
#include <string>


// OLED
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// should be in setup
void start_read_nfc () {
  Serial.println("NFC TAG READER"); // Header used when using the serial monitor

}

// should be inside loop
String read_nfc () {
  String tea_description = "";
  
  Serial.println("\nPut tea box on the left side\n");  // Command so that you an others will know what to do 
  display.clearDisplay(); // Clear display
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("\nPut tea box on the left side\n");
  display.display(); // Make the actual printing happen on the hardware

  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    Serial.println(tag.getTagType());
    Serial.print("UID: ");Serial.println(tag.getUidString()); // Retrieves the Unique Identification from your tag

#ifdef NDEF_USE_SERIAL
  Serial.print("my own error");
#endif

    if (tag.hasNdefMessage()) // If your tag has a message
    {

      NdefMessage message = tag.getNdefMessage();
      Serial.print("\nThis Message in this Tag is ");
      Serial.print(message.getRecordCount());
      Serial.print(" NFC Tag Record");
      if (message.getRecordCount() != 1) { 
        Serial.print("s");
      }
      Serial.println(".");

      // If you have more than 1 Message then it wil cycle through them
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++)
      {
        Serial.print("\nNDEF Record ");Serial.println(i+1);
        NdefRecord record = message.getRecord(i);

        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);


        String payloadAsString = ""; // Processes the message as a string vs as a HEX value
        for (int c = 0; c < payloadLength; c++) {
          payloadAsString += (char)payload[c];
        }
        Serial.print("  Information (as String): ");
        Serial.println(payloadAsString);
        tea_description = payloadAsString;

        String uid = record.getId();
        if (uid != "") {
          Serial.print("  ID: ");Serial.println(uid); // Prints the Unique Identification of the NFC Tag
        }

        
      }
    }
  }
  
  return tea_description;
  
  delay(10000);
}

// should be inside loop
void read_nfc_original () {
  Serial.println("\nScan your NFC tag on the NFC Shield\n");  // Command so that you an others will know what to do 

  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    Serial.println(tag.getTagType());
    Serial.print("UID: ");Serial.println(tag.getUidString()); // Retrieves the Unique Identification from your tag

    if (tag.hasNdefMessage()) // If your tag has a message
    {

      NdefMessage message = tag.getNdefMessage();
      Serial.print("\nThis Message in this Tag is ");
      Serial.print(message.getRecordCount());
      Serial.print(" NFC Tag Record");
      if (message.getRecordCount() != 1) { 
        Serial.print("s");
      }
      Serial.println(".");

      // If you have more than 1 Message then it wil cycle through them
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++)
      {
        Serial.print("\nNDEF Record ");Serial.println(i+1);
        NdefRecord record = message.getRecord(i);

        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);


        String payloadAsString = ""; // Processes the message as a string vs as a HEX value
        for (int c = 0; c < payloadLength; c++) {
          payloadAsString += (char)payload[c];
        }
        Serial.print("  Information (as String): ");
        Serial.println(payloadAsString);

        String uid = record.getId();
        if (uid != "") {
          Serial.print("  ID: ");Serial.println(uid); // Prints the Unique Identification of the NFC Tag
        }
      }
    }
  }
  delay(10000);
}

String print = "Welcome to ITea\nPut tea box left to start\n";

// should be inside loop
void read_show_nfc () {
  
  nfc.begin();
  
  Serial.println("\nPut tea box left\n");  // Command so that you an others will know what to do 

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while(true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,28);
  display.println(print);
  display.display();

int a = 0;

while (a == 0) {
if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    Serial.println(tag.getTagType());
    Serial.print("UID: ");Serial.println(tag.getUidString()); // Retrieves the Unique Identification from your tag

    if (tag.hasNdefMessage()) // If your tag has a message
    {

      NdefMessage message = tag.getNdefMessage();
      Serial.print("\nThis Message in this Tag is ");
      Serial.print(message.getRecordCount());
      Serial.print(" NFC Tag Record");
      if (message.getRecordCount() != 1) { 
        Serial.print("s");
      }
      Serial.println(".");

      // If you have more than 1 Message then it wil cycle through them
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++)
      {
        Serial.print("\nNDEF Record ");Serial.println(i+1);
        NdefRecord record = message.getRecord(i);

        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);


        String payloadAsString = ""; // Processes the message as a string vs as a HEX value
        for (int c = 0; c < payloadLength; c++) {
          payloadAsString += (char)payload[c];
        }
        Serial.println("Information (as String): ");
        Serial.println(payloadAsString);
        print = payloadAsString;


        int firstInfusion = payloadAsString.indexOf("1. Infusion");
        int secondInfusion = payloadAsString.indexOf("2. Infusion");
        String firstInfusionString = payloadAsString.substring(firstInfusion, secondInfusion);
        Serial.println("first Infusion as String:");
        Serial.println(firstInfusion);
        Serial.println(secondInfusion);
        
        int firstInput = payloadAsString.indexOf(':'); // searching for first Input (after ":")
        String name = payloadAsString.substring(firstInput+1, firstInfusion);
        name.replace(" ", ""); 
        Serial.println("Name String:");
        Serial.println(name);

        int secondInput = payloadAsString.indexOf(':', firstInput+1); // searching for second Input
        int secondEnd = payloadAsString.indexOf('C', secondInput); // searching for end of temperature Input
        String tempString = payloadAsString.substring(secondInput+1, secondEnd); // get String from second input to the end of temperature input
        tempString.replace(" ", ""); //remove empty characters
        int temp1 = tempString.toInt(); //convert to int


        //std::string message = std::string(payloadAsString.c_str());
        //std::string min = "min";
        //std::string tempString = message.find(min);
        Serial.println("Temp String:");
        Serial.println(temp1);

        String uid = record.getId();
        if (uid != "") {
          Serial.print("  ID: ");Serial.println(uid); // Prints the Unique Identification of the NFC Tag
        }
        if(print != "") {
          a = 0;
        }
        
      }
    }
  }
}

  delay(10000);
}

/*
string start,end;
start = theEntireString.substr(1,theEntireString.find(":")-2);
end = theEntireString.substr(theEntireString.find(":")+2,theEntireString.size()-1);


x.substr(x.find(":") + 1); 

*/












void start_write_nfc() {
      Serial.println("NDEF Writer");
      nfc.begin();
}

void write_nfc() {
    Serial.println("\nPlace a formatted Mifare Classic or Ultralight NFC tag on the reader.");
    if (nfc.tagPresent()) {
        NdefMessage message = NdefMessage();
        message.addUriRecord("Tea: Earl Gray\nTemperature: 90C\nTime: 2 min");

        bool success = nfc.write(message);
        if (success) {
          Serial.println("Success. Try reading this tag with your phone.");        
        } else {
          Serial.println("Write failed.");
        }
    }
    delay(5000);
}


void show_nfc() {

  String tea_description = read_nfc();

    // Show on the OLED display
    display.clearDisplay(); // Clear display

    // display temperature
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(tea_description);
  
    display.display(); // Make the actual printing happen on the hardware

    delay(1000); // new values of sensor will be sent to Arduino serial monitor after every one second
}
