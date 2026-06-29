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


// OLED
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// should be in setup
void start_read_nfc () {
  Serial.println("NFC TAG READER"); // Header used when using the serial monitor

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

  delay(1000);
        

        if((payloadAsString.indexOf('?')== -1) & (payloadAsString.indexOf('@') == -1)) {
        int Input1 = payloadAsString.indexOf("en"); // searching for first Input (after ":")
        int end1 = payloadAsString.indexOf(';', Input1); // searching for end of temperature Input
        String name = payloadAsString.substring(Input1+2, end1);
        name.replace(" ", ""); 
        Serial.println("Name String:");
        Serial.println(name);

        int endInf = payloadAsString.indexOf("Inf", end1); // searching for end of temperature Input
        String infusionsString = payloadAsString.substring(end1+1, endInf);
        infusionsString.replace(" ", ""); 
        int infusions = infusionsString.toInt(); //convert to int
        Serial.println("Infusion String:");
        Serial.println(infusionsString);

        int end2 = payloadAsString.indexOf("spoon", endInf); // searching for end of temperature Input
        String spoonString = payloadAsString.substring(endInf+4, end2); // get String from second input to the end of temperature input
        spoonString.replace(" ", ""); //remove empty characters
        int spoon = spoonString.toInt(); //convert to int
        Serial.println("Spoon String:");
        Serial.println(spoonString);

        int end3 = payloadAsString.indexOf("g", end2); // searching for end of temperature Input
        String gString = payloadAsString.substring(end2+5, end3); // get String from second input to the end of temperature input
        gString.replace(" ", ""); //remove empty characters
        int g = gString.toInt(); //convert to int
        Serial.println("g String:");
        Serial.println(gString);

        if(infusions == 2) {
        int end4 = payloadAsString.indexOf("C", end3); // searching for end of temperature Input
        String tempString = payloadAsString.substring(end3+2, end4); // get String from second input to the end of temperature input
        tempString.replace(" ", ""); //remove empty characters
        int temp1 = tempString.toInt(); //convert to int
        Serial.println("Temp String:");
        Serial.println(tempString);

       int end5 = payloadAsString.indexOf("C", end4+2); // searching for end of temperature Input
        String tempString2 = payloadAsString.substring(end4+2, end5); // get String from second input to the end of temperature input
        tempString2.replace(" ", ""); //remove empty characters
        int temp2 = tempString2.toInt(); //convert to int
        Serial.println("Temp2 String:");
        Serial.println(tempString2);

        int end6 = payloadAsString.indexOf("min", end5); // searching for end of temperature Input
        String minString = payloadAsString.substring(end5+2, end6); // get String from second input to the end of temperature input
        gString.replace(" ", ""); //remove empty characters
        int min1 = minString.toInt(); //convert to int
        Serial.println("min1 String:");
        Serial.println(minString);

        int end7 = payloadAsString.indexOf("s", end6); // searching for end of temperature Input
        String secString = payloadAsString.substring(end6+3, end7); // get String from second input to the end of temperature input
        secString.replace(" ", ""); //remove empty characters
        int sec1 = secString.toInt(); //convert to int
        Serial.println("Sec1 String:");
        Serial.println(secString);

        int end8 = payloadAsString.indexOf("min", end7); // searching for end of temperature Input
        String minString2 = payloadAsString.substring(end7+2, end8); // get String from second input to the end of temperature input
        minString2.replace(" ", ""); //remove empty characters
        int min2 = minString2.toInt(); //convert to int
        Serial.println("min2 String:");
        Serial.println(minString2);

        int end9 = payloadAsString.indexOf("s", end8); // searching for end of temperature Input
        String secString2 = payloadAsString.substring(end8+3, end9); // get String from second input to the end of temperature input
        secString2.replace(" ", ""); //remove empty characters
        int sec2 = secString2.toInt(); //convert to int
        Serial.println("Sec2 String:");
        Serial.println(secString2);
        }


        if (infusions == 1) {
        int end4 = payloadAsString.indexOf("C", end3); // searching for end of temperature Input
        String tempString = payloadAsString.substring(end3+2, end4); // get String from second input to the end of temperature input
        tempString.replace(" ", ""); //remove empty characters
        int temp1 = tempString.toInt(); //convert to int
        Serial.println("Temp String:");
        Serial.println(tempString);

        int end5 = payloadAsString.indexOf("min", end4); // searching for end of temperature Input
        String minString = payloadAsString.substring(end4+2, end5); // get String from second input to the end of temperature input
        gString.replace(" ", ""); //remove empty characters
        int min1 = minString.toInt(); //convert to int
        Serial.println("min1 String:");
        Serial.println(minString);

        int end6 = payloadAsString.indexOf("s", end5); // searching for end of temperature Input
        String secString = payloadAsString.substring(end5+3, end6); // get String from second input to the end of temperature input
        secString.replace(" ", ""); //remove empty characters
        int sec1 = secString.toInt(); //convert to int
        Serial.println("Sec1 String:");
        Serial.println(secString);

        }     



        }

        else {
          print = "Reading failed. Try again!";
        }


        String uid = record.getId();
        if (uid != "") {
          Serial.print("  ID: ");Serial.println(uid); // Prints the Unique Identification of the NFC Tag
        }

        
      }
    }
  }

  delay(1000);
}

/*
string start,end;
start = theEntireString.substr(1,theEntireString.find(":")-2);
end = theEntireString.substr(theEntireString.find(":")+2,theEntireString.size()-1);


x.substr(x.find(":") + 1); 

*/

