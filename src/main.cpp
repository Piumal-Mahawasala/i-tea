#include <AsyncElegantOTA.h>
#include <cmath>
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <HX711_ADC.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <SPI.h>
//Telegram Bot library
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
//WIFI connection library and initial setup
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//const char* ssid = "Piumal"; //Enter SSID
//const char* password = "piumal@m"; //Enter Password

const char* ssid = "iPhone von Helena (2)"; //Enter SSID
const char* password = "Ananas22"; //Enter Password
//const char* ssid = "Piumal"; //Enter SSID
//const char* password = "piumal@m"; //Enter Password
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
//WIFI connection library and initial setup ends

#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

// NFC
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

// OLED
Adafruit_SSD1306 display(128, 64, &Wire, -1);
SSD1306AsciiWire oled;

// Weight Scale
HX711_ADC LoadCell(D4, D3); // dt pin, sck pin
bool base = true;

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1
#define SPEED_UP 1;

// variables for timer
// #define WEIGHT_THRESHOLD -20        // Temperature threshold for triggering the alarm (adjust as needed)
#define SAMPLE_INTERVAL 5000 // Interval between weight readings in milliseconds (adjust as needed)
unsigned long previousMillis = 0;
unsigned long startMillis = 0;
bool isAlarmActive = false;
float baseWeight = 0.00;
int state = 0;
int threshold = 20;
int counter = 5;
int forthCounter = counter / 4;
int halfCounter = counter / 2;
int threeForthCounter = halfCounter + forthCounter;
int minutes = 0;
int seconds = 0;

// variables for NFC read
String name;
int infusions;
int spoon;
int g;
int temp1;
int min1;
int sec1;
int temp2;
int min2;
int sec2;
int time1;
int time2;
int timeTea;
int temp;
int timeTemp = 0;

boolean showInfusion1 = true;


// buzzer
const int buzzerPin = D5; // Verbinde den Buzzer mit dem digitalen Pin D5
unsigned long lastPeriodStart;
const int onDuration = 1000;
const int periodDuration = 500;
bool alarm = true;

// buttons
int buttonRight = D8; // right button (no touch)
int touchButton = D7; // touch button left

// LED
const int ledPin = D0;
int ledState = LOW;

// Calculating time
double result;
int countdown;


////// Telegram Bot

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6379353521:AAHgBI83wuEaO52pnlS8CtmB9oTQaGnDryE"

const unsigned long BOT_MTBS = 500; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

String chat_id;

bool Telegram = false;
bool writingLoop = false;
int writeTelegram = 0;

bool a = true;
bool notificationsOn = false;
String localIP;
String messageTelegram;

  String writeName;
  int writeSpoon;
  int writeGram;
  int writeInfusions;
  int writeTemp;
  int writeMin;
  int writeSec;
  int writeTemp2;
  int writeMin2;
  int writeSec2;


void handleNewMessages(int numNewMessages)
{


  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";


    if (text == "/start")
    {
      String welcome = "Welcome to The ITea Bot, " + from_name + ".\n";
      welcome += "/start : if you start brewing tea and want to get notifications\n";
      welcome += "/stop : if you want to stop getting notifications \n";
      welcome += "/write : to write on a card for your tea box\n";  
      welcome += "This is the link to the website: ";    
      welcome += localIP;  
      bot.sendMessage(chat_id, welcome, "Markdown");
      notificationsOn = true;
    }    
    
    if (writeTelegram == 7) {
      writeSec = bot.messages[i].text.toInt();
      Serial.println(writeSec);
      if(writeInfusions == 1) {
        writingLoop = true;
        bot.sendMessage(chat_id, "These are all the necessary informations. The ITea device will now begin to write on the card.", "");
      }
    }

    if (writeInfusions == 2) {

      if (writeTelegram == 10) {
          writeSec2 = bot.messages[i].text.toInt();
        Serial.println(writeSec2);
        writingLoop = true;
        bot.sendMessage(chat_id, "These are all the necessary informations. Put the tea card next to the tea device. The ITea device will now begin to write on the card.", "");
      }

      if (writeTelegram == 9) {
        writeMin2 = bot.messages[i].text.toInt();
        Serial.println(writeMin2);
        bot.sendMessage(chat_id, "Now type in the seconds.", "");
        writeTelegram = 10;
      }

      if (writeTelegram == 8) {
        writeTemp2 = bot.messages[i].text.toInt();
        Serial.println(writeTemp2);
        bot.sendMessage(chat_id, "Over the next two messages type in the time the SECOND INFUSION of the tea needs to steep. First type in the minutes.", "");
        writeTelegram = 9;
      }
      
      if (writeTelegram == 7) {
        bot.sendMessage(chat_id, "Type in the temperature the SECOND INFUSION of the tea needs in Celsius.", "");
        writeTelegram = 8;
      }
    }

    if (writeTelegram == 6) {
      writeMin = bot.messages[i].text.toInt();
      Serial.println(writeMin);
      bot.sendMessage(chat_id, "Now type in the seconds.", "");
      writeTelegram = 7;
    }

    if (writeTelegram == 5) {
      writeTemp = bot.messages[i].text.toInt();
      Serial.println(writeTemp);
      bot.sendMessage(chat_id, "Over the next two messages type in the time the tea (or its first infusion) needs to steep. First type in the minutes.", "");
      writeTelegram = 6;
    }

    if (writeTelegram == 4) {
      writeInfusions = bot.messages[i].text.toInt();
      Serial.println(writeInfusions);
      bot.sendMessage(chat_id, "Type in the temperature the tea (or its first infusion) needs in Celsius.", "");
      writeTelegram = 5;
    }
    
    if (writeTelegram == 3) {
      writeGram = bot.messages[i].text.toInt();
      Serial.println(writeGram);
      bot.sendMessage(chat_id, "Some types of tea leaves can be used multiple times (infusions), but they need different instructions. How many infusions do you want to save? Type in '1' or '2'.", "");
      writeTelegram = 4;
    }

    if (writeTelegram == 2) {
      writeSpoon = bot.messages[i].text.toInt();
      Serial.println(writeSpoon);
      bot.sendMessage(chat_id, "How many tea leaves do you need in gramm? Type in the weight in g (only the number) or '0' if not applicable.", "");
      writeTelegram = 3;
    }

    if (writeTelegram == 1) {
      writeName = bot.messages[i].text;
      Serial.println(writeName);
      bot.sendMessage(chat_id, "How many spoons of tea leaves do you need? Type in the number of tea spoons or '0' if not applicable.", "");
      writeTelegram = 2;
    }

    if (text == "/write")
    {
      bot.sendMessage(chat_id, "Enter the information about the tea brewing. Usually, you find it in the instructions when you buy new tea. You can also look up recommendations online, e.g., here. If you don't have an information, type in '0'", "");
      bot.sendMessage(chat_id, "Type in the name of the tea.", "");
      writeTelegram = 1;
      Telegram = true;
    }

    if(writingLoop == true) {
      messageTelegram = writeName + ";\n" + String(writeInfusions) + "Inf;\n" + String(writeSpoon) + "spoon " + String(writeGram) + "g;\n";
      if(writeInfusions == 1) {
        messageTelegram += String(writeTemp) + "C;\n" + String(writeMin) + "min " + String(writeSec) + "s;" ;
      }
      if(writeInfusions == 2) {
        messageTelegram += String(writeTemp) + "C/" + String(writeTemp2) + "C;\n" + String(writeMin) + "min " + String(writeSec) + "s/" + String(writeMin2) + "min " + String(writeSec2) + "s;" ;
      }
    } 


    if (text == "/stop")
    {
      bot.sendMessage(chat_id, "Stop sending notifications", "");
      notificationsOn = false;
    }

    Serial.println(writeTelegram);

  }
}


void checkTelegram()
{

  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    //Serial.println(chat_id);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}


////////// Formula to calculate cool down time //////////

double calculateT(double reqtemp)
{
  result = (-log((reqtemp - 22.00) / (100.00 - 22.00))) / 0.00047632; // reqtemp = wished endtemperature; Ambient temperature = 22; Initial temperature = 100; Cooling coefficient(k) = 0.00047632; k = h*A/C                                          // k = h*A/C; h = 99.55; A(Diametar of cub)= 8cm; c(heat capacity of water) = 4.19
  return result;
}

////////// Function for buzzer (alarm sound) and LED blinking //////////

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
        tone(buzzerPin, 2000, onDuration); // play 550 Hz tone for 'onDuration'
      }
      else
      {
        ledState = LOW;
        digitalWrite(ledPin, ledState);
        noTone(buzzerPin); // Turn off buzzer
      }
    }

    if (digitalRead(buttonRight) == HIGH)
    {
      alarm = false;
      digitalWrite(ledPin, LOW);
      noTone(buzzerPin);
      isAlarmActive = false;
      state += 1;
      Serial.println("State after smiley");
      Serial.println(state);
    }
  }

}

////////// Function to show circle around timer //////////

void full()
{
  oled.setFont(System5x7);
  oled.setCursor(1, 0);
  oled.print("     ***********");
  oled.setCursor(1, 1);
  oled.print("   ***         ***");
  oled.setCursor(1, 2);
  oled.print(" ***             ***");
  oled.setCursor(1, 3);
  oled.print("**");
  oled.setCursor(19 * 6 + 1, 3);
  oled.print("**");
  oled.setCursor(1, 4);
  oled.print("**");
  oled.setCursor(19 * 6 + 1, 4);
  oled.print("**");
  oled.setCursor(1, 5);
  oled.print(" ***             ***");
  oled.setCursor(1, 6);
  oled.print("   ***         ***");
  oled.setCursor(1, 7);
  oled.print("     ***********");
}

////////// Function to show timer //////////

void displayTime(int currentCounter)
{
  if (currentCounter > 0)
  {
    minutes = currentCounter / 60;
    seconds = currentCounter % 60;
    int decSeconds = seconds / 10;
    int singSeconds = seconds % 10;
    if ((minutes == 9) & (seconds == 59)) {
      oled.clear();
      full();
    }
    oled.setCursor(37, 3);
    oled.setFont(lcdnums12x16);
    oled.print(minutes);
    oled.print(":");
    oled.print(decSeconds);
    oled.print(singSeconds);
  }
  else
  {
    oled.clear();
    full();
    if(notificationsOn) {
      if(state == 3) {
        Serial.println(chat_id);
        delay(1000);
        bot.sendMessage(chat_id, "Alarm: Your boiled water has cooled down to the correct temperature. Time to put the tea in.", "");
      }
      if(state == 4) {
        Serial.println(chat_id);
        delay(1000);
        bot.sendMessage(chat_id, "Alarm: Your tea is ready.", "");
      }
    }
    oled.setCursor(1, 2);
    oled.print(" ***    Press    ***");
    oled.setCursor(1, 5);
    oled.print(" ***    enter    ***");
    ws.textAll("Press enter");
    buzzer();

  }
}

////////// Functions to visualize different states of the timer //////////

void firstHalfEmpty()
{
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

void secondHalfEmpty()
{
  oled.setFont(System5x7);
  oled.setCursor(11 * 6 + 1, 0);
  oled.print("     ");
  oled.setCursor(15 * 6 + 1, 1);
  oled.print("   ");
  oled.setCursor(17 * 6 + 1, 2);
  oled.print("   ");
  oled.setCursor(19 * 6 + 1, 3);
  oled.print("  ");
}

void thirdHalfEmpty()
{
  oled.setFont(System5x7);
  oled.setCursor(11 * 6 + 1, 7);
  oled.print("     ");
  oled.setCursor(15 * 6 + 1, 6);
  oled.print("   ");
  oled.setCursor(17 * 6 + 1, 5);
  oled.print("   ");
  oled.setCursor(19 * 6 + 1, 4);
  oled.print("  ");
}

void lastHalfEmpty()
{
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

////////// Function calculate current timer state //////////

int getCounter()
{
  int currentCount = counter;
  if (counter > 0)
  {
    counter = counter - SPEED_UP;
  }

  return currentCount;
}

////////// Function to start the weight scale //////////

void start_timer()
{
  LoadCell.begin();           // start connection to HX711
  LoadCell.start(1000);       // load cells gets 1000ms of time to stabilize
  LoadCell.setCalFactor(375); // Calibarate your LOAD CELL with 100g weight, and change the value according to readings
  Wire.begin();
  Wire.setClock(400000L);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else  // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  delay(2000);
}

////////// Function to start the timer //////////

void timer()
{
  int currentCounter = getCounter();
  oled.setFont(System5x7);
  // oled.clear();
  oled.setCursor(0, 0);
  full();
  displayTime(currentCounter);
  delay(500);
  if (currentCounter > threeForthCounter)
  {
    secondHalfEmpty();
  }
  else if (currentCounter > halfCounter)
  {
    thirdHalfEmpty();
  }
  else if (currentCounter > forthCounter)
  {
    lastHalfEmpty();
  }
  else if (currentCounter > 0)
  {
    firstHalfEmpty();
  }
  // displayTime(currentCounter);
  delay(400);
}

////////// Function to start the 2 timers triggered by weight scale //////////

void weight_timer()
{
  checkTelegram();
  while (state == 3)
  {
    counter = timeTemp; //excluded for Bazar
    //counter = 12;
    forthCounter = counter / 4;
    halfCounter = counter / 2;
    threeForthCounter = halfCounter + forthCounter;

    while (base == true)
    {
      LoadCell.update();                     // retrieves data from the load cell
      float baseWeight = LoadCell.getData(); // get output value

      Serial.print("Base Weight: ");
      Serial.println(baseWeight);
      base = false;
    }

    oled.setFont(System5x7);
    oled.setCursor(0, 2);
    oled.println("1. Boil water");
    oled.setCursor(0, 4);
    oled.println("2. Put water in cup");
    oled.setCursor(0, 6);
    oled.println("3. Put cup on top");
    ws.textAll("1. Boil water\n\n2. Put water in cup\n\n3. Put cup on top");

    digitalWrite(ledPin, HIGH);

    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLE_INTERVAL)
    {
      LoadCell.update();                        // retrieves data from the load cell
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
        if (temp < 95) // should be 95 (changed for bazar)
        {
          //full();
          displayTime(counter);
          Serial.println("Alarm triggered!");
          checkTelegram();
          ws.textAll("temptime:" + String(countdown));
         // ws.textAll("boiled:");
          isAlarmActive = true;
          oled.clear();
          while (state == 3)
          {
            timer();
            digitalWrite(ledPin, LOW);
          }
        }
        else
        {
          state = 4;
        }
      }
    }

    delay(1000);
  }

  if (state == 4)
  {
    oled.setFont(System5x7);
    oled.clear();
    oled.setCursor(0, 3);
    oled.println("1. Put tea in cup");
    oled.setCursor(0, 5);
    oled.println("2. Press enter");
    ws.textAll("1. Put tea in cup\n2. Press enter");

    alarm = true;
    checkTelegram();
  }

  while (state == 4)
  {
    counter = timeTea; 
    //counter = 10;
    forthCounter = counter / 4;
    halfCounter = counter / 2;
    threeForthCounter = halfCounter + forthCounter;

    if (digitalRead(buttonRight) == HIGH)
    {
      //full();
      displayTime(counter);
      Serial.println("Alarm triggered!");
      ws.textAll("temptime1:" + String(counter));
      isAlarmActive = true;
      oled.clear();
      while (state == 4)
      {
        timer();
        digitalWrite(ledPin, LOW);
      }
    }
    delay(500);
  }

  if (state == 5)
  {
    counter = (15 * 60) - (timeTemp + timeTea); // 15 min since first timer started (to get on a good drinking temperature around 60C) //excluded for Bazar
    //counter = 13;

    oled.setFont(System5x7);
    oled.clear();
    oled.setCursor(0, 3);
    oled.println("Remove the tea leaves");
    oled.setCursor(0, 5);
    oled.println("and enjoy!");
    ws.textAll("Remove the tea leaves\nand enjoy!");

    alarm = true;
    checkTelegram();
  }

  while (state == 5)
  {
    int drinkTime = getCounter();

    if (drinkTime > 0)
    {
      Serial.println(drinkTime);
      delay(1000);
    }
    else
    {
      oled.setFont(System5x7);
      oled.clear();
      oled.setCursor(0, 3);
      oled.println("Don't forget to drink");
      oled.setCursor(0, 5);
      oled.println("your tea!");
      ws.textAll("Don't forget to drink\nyour tea!");

    if(notificationsOn) {
        bot.sendMessage(chat_id, "Reminder: Your tea should have a good drinking temperature now. Don't forget to drink it. Enjoy!", "");
      }

  

      digitalWrite(ledPin, HIGH);
      tone(buzzerPin, 2000);
      delay(3000);

      digitalWrite(ledPin, LOW);
      noTone(buzzerPin);
      state = 0;
    }
  }


  delay(1000);
}

////////// Function to start nfc reader //////////

void start_read_nfc()
{
  Serial.println("NFC TAG READER"); // Header used when using the serial monitor
}

////////// Function to read nfc and to extract variables //////////

void read_show_nfc()
{
  
  nfc.begin();
  Serial.println("Put tea box left to start");
  oled.clear();
  oled.setCursor(0, 0);
  oled.setFont(Callibri11);
  oled.println("Welcome to ITea");
  oled.println("Put tea box left to start");
  ws.textAll("Welcome to ITea \nPut tea box left to start");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("SSD1306 allocation failed");
    while (true)
      ;
  }

  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    Serial.println(tag.getTagType());
    Serial.print("UID: ");
    Serial.println(tag.getUidString()); // Retrieves the Unique Identification from your tag

    if (tag.hasNdefMessage()) // If your tag has a message
    {

      NdefMessage message = tag.getNdefMessage();
      Serial.print("\nThis Message in this Tag is ");
      Serial.print(message.getRecordCount());
      Serial.print(" NFC Tag Record");
      if (message.getRecordCount() != 1)
      {
        Serial.print("s");
      }
      Serial.println(".");

      // If you have more than 1 Message then it wil cycle through them
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++)
      {
        Serial.print("\nNDEF Record ");
        Serial.println(i + 1);
        NdefRecord record = message.getRecord(i);

        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);

        String payloadAsString = ""; // Processes the message as a string vs as a HEX value
        for (int c = 0; c < payloadLength; c++)
        {
          payloadAsString += (char)payload[c];
        }
        Serial.println("Information (as String): ");
        Serial.println(payloadAsString);
        ws.textAll(payloadAsString+"\n\n Press enter to proceed");

        delay(10);

        if ((payloadAsString.indexOf('?') == -1) & (payloadAsString.indexOf('@') == -1) & (payloadAsString.indexOf('}') == -1) & (payloadAsString.indexOf('"') == -1) & (payloadAsString.indexOf('+') == -1) & (payloadAsString.indexOf('!') == -1))
        {
          Serial.println(payloadAsString.indexOf('?'));
          int Input1 = payloadAsString.indexOf("en");      // searching for first Input (after "en")
          int end1 = payloadAsString.indexOf(';', Input1); // searching for end of temperature Input
          name = payloadAsString.substring(Input1 + 2, end1);
          // name.replace(" ", "");
          Serial.println("Name String:");
          Serial.println(Input1);
          Serial.println(end1);
          Serial.println(name);

          int endInf = payloadAsString.indexOf("Inf", end1); // searching for end of temperature Input
          String infusionsString = payloadAsString.substring(end1 + 1, endInf);
          infusionsString.replace(" ", "");
          infusions = infusionsString.toInt(); // convert to int
          Serial.println("Infusion String:");
          Serial.println(infusionsString);

          int end2 = payloadAsString.indexOf("spoon", endInf);              // searching for end of temperature Input
          String spoonString = payloadAsString.substring(endInf + 4, end2); // get String from second input to the end of temperature input
          spoonString.replace(" ", "");                                     // remove empty characters
          spoon = spoonString.toInt();                                      // convert to int
          Serial.println("Spoon String:");
          Serial.println(spoonString);

          int end3 = payloadAsString.indexOf("g", end2);              // searching for end of temperature Input
          String gString = payloadAsString.substring(end2 + 5, end3); // get String from second input to the end of temperature input
          gString.replace(" ", "");                                   // remove empty characters
          g = gString.toInt();                                        // convert to int
          Serial.println("g String:");
          Serial.println(gString);

          if (infusions == 2)
          {
            int end4 = payloadAsString.indexOf("C", end3);                 // searching for end of temperature Input
            String tempString = payloadAsString.substring(end3 + 2, end4); // get String from second input to the end of temperature input
            tempString.replace(" ", "");                                   // remove empty characters
            temp1 = tempString.toInt();                                    // convert to int
            Serial.println("Temp String:");
            Serial.println(tempString);

            int end5 = payloadAsString.indexOf("C", end4 + 2);              // searching for end of temperature Input
            String tempString2 = payloadAsString.substring(end4 + 2, end5); // get String from second input to the end of temperature input
            tempString2.replace(" ", "");                                   // remove empty characters
            temp2 = tempString2.toInt();                                    // convert to int
            Serial.println("Temp2 String:");
            Serial.println(tempString2);

            int end6 = payloadAsString.indexOf("min", end5);              // searching for end of temperature Input
            String minString = payloadAsString.substring(end5 + 2, end6); // get String from second input to the end of temperature input
            minString.replace(" ", "");                                   // remove empty characters
            min1 = minString.toInt();                                     // convert to int
            Serial.println("min1 String:");
            Serial.println(minString);

            int end7 = payloadAsString.indexOf("s", end6);                // searching for end of temperature Input
            String secString = payloadAsString.substring(end6 + 3, end7); // get String from second input to the end of temperature input
            secString.replace(" ", "");                                   // remove empty characters
            sec1 = secString.toInt();                                     // convert to int
            Serial.println("Sec1 String:");
            Serial.println(secString);

            int end8 = payloadAsString.indexOf("min", end7);               // searching for end of temperature Input
            String minString2 = payloadAsString.substring(end7 + 2, end8); // get String from second input to the end of temperature input
            minString2.replace(" ", "");                                   // remove empty characters
            min2 = minString2.toInt();                                     // convert to int
            Serial.println("min2 String:");
            Serial.println(minString2);

            int end9 = payloadAsString.indexOf("s", end8);                 // searching for end of temperature Input
            String secString2 = payloadAsString.substring(end8 + 3, end9); // get String from second input to the end of temperature input
            secString2.replace(" ", "");                                   // remove empty characters
            sec2 = secString2.toInt();                                     // convert to int
            Serial.println("Sec2 String:");
            Serial.println(secString2);

            time1 = min1 * 60 + sec1;
            time2 = min2 * 60 + sec2;
          }

          if (infusions == 1)
          {
            int end4 = payloadAsString.indexOf("C", end3);                 // searching for end of temperature Input
            String tempString = payloadAsString.substring(end3 + 2, end4); // get String from second input to the end of temperature input
            tempString.replace(" ", "");                                   // remove empty characters
            temp1 = tempString.toInt();                                    // convert to int
            Serial.println("Temp String:");
            Serial.println(tempString);

            int end5 = payloadAsString.indexOf("min", end4);              // searching for end of temperature Input
            String minString = payloadAsString.substring(end4 + 2, end5); // get String from second input to the end of temperature input
            gString.replace(" ", "");                                     // remove empty characters
            min1 = minString.toInt();                                     // convert to int
            Serial.println("min1 String:");
            Serial.println(minString);

            int end6 = payloadAsString.indexOf("s", end5);                // searching for end of temperature Input
            String secString = payloadAsString.substring(end5 + 3, end6); // get String from second input to the end of temperature input
            secString.replace(" ", "");                                   // remove empty characters
            sec1 = secString.toInt();                                     // convert to int
            Serial.println("Sec1 String:");
            Serial.println(secString);

            time1 = min1 * 60 + sec1;
          }
          oled.clear();
          state = 2;
          Serial.println(state);
          break;
        }

        else
        {
          oled.clear();
          oled.setCursor(0, 0);
          oled.setFont(Callibri11);
          oled.print("Reading failed.");
          oled.print("Try again!");
          ws.textAll("Reading failed.\nTry again!");
        }
      }
    }
  }

  delay(1000);
}








const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ITea</title>
  <meta name="viewport" content="width=device-width, initial-scale=1" charset="UTF-8">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   //.button:hover {background-color: #0f8b8d}
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
   .container {
      display: flex;
    }
    .heading-container {
      text-align: center;
      background-color: #236fa1;
      padding: 10px;
      margin-bottom: 10px;
    }

    .heading-container h2 {
      font-family: Calibri, Arial, sans-serif;
      color: #fff;
      margin: 0;
    }
   #oledtxt {
       background-color: black;
       width: 500px;
       height: 250px;
       color: white;
       font-size: 20px;
       
    }
  .box {
      flex: 1;
      padding: 10px;
      background-color: #f2f2f2;
	    font-family: Calibri, Arial, sans-serif;
      justify-content: center;
      align-items: center;
      
    }

    .box:nth-child(1) {
      background-color: #f2f2f2;
      padding-right: 20px;
	  margin: 10px;
    }

    .box:nth-child(2) {
      background-color: #f2f2f2;
      padding: 0 20px;
	  margin: 10px;
    }

    .box:nth-child(3) {
      background-color: #f2f2f2;
      padding-left: 20px;
	  margin: 10px;
    }
	.widget {
		background-color: #fffff; // Set the background color 
		padding: 10px;
		border: 5px solid #ccc;
		text-align: center;
		margin: 10px;
	}
  </style>

<link rel="icon" href="data:,">
<script>
    function updateDateTime() {
      // Get the current date and time
      var now = new Date();
      
      // Format the date and time as desired
      var date = now.toDateString();
      var time = now.toLocaleTimeString();

      // Update the HTML elements with the date and time values
      document.getElementById('date').textContent = date;
      document.getElementById('time').textContent = time;
    }

    // Call the updateDateTime function initially and set an interval to update it every second
    setInterval(updateDateTime, 1000);
  </script>
</head>
<body>
  <div class="topnav">
    <h1>iTea Dashboard</h1>
  </div>
  
  <div class="container">
    <div class="box">
		<div class="widget">
			<h1 style="color: #383838; text-align: center;">Date and Time</h1>
			<h2 style="color: #787878; text-align: center;"> <span id="date"></span></h2>
			<h2 style="color: #787878; text-align: center;"> <span id="time"></span></h2>
		</div>
		<div class= "widget">
		 <h1 style="color: #383838; text-align: center;">Weather</h1>
			<h2 id="temperature" style="color: #787878; text-align: center;">Loading...</h2>

		<script>
		// Fetch temperature data from the API
			fetch('https://api.openweathermap.org/data/2.5/weather?lat=49.23&lon=7.00&appid=6c8ca0a1a271d513a0ec8a038966a1d7')
			.then(response => response.json())
			.then(data => {
				const temperatureElement = document.getElementById('temperature');

				if (data.main && data.main.temp) {
				// Extract temperature value from the API response
				const temperature = data.main.temp;

				// Convert temperature from Kelvin to Celsius or Fahrenheit
				const temperatureCelsius = temperature - 273.15;
				const temperatureFahrenheit = (temperature - 273.15) * 9/5 + 32;

				// Update the HTML with the temperature value
				temperatureElement.textContent = `Temperature: ${temperatureCelsius.toFixed(2)}°C / ${temperatureFahrenheit.toFixed(2)}°F`;
				} 		else {
          temperatureElement.textContent = 'Temperature data unavailable';
					}
			})
				.catch(error => {
					console.error('Error fetching temperature data:', error);
					document.getElementById('temperature').textContent = 'Failed to fetch temperature';
				});
  </script>
		</div>
    </div>
   <div class="box">
      <h2 style="color: #383838; text-align: center;">Serial Monitor</h2>
      <p>Current Status</p>
	  <div id="oledtxt"></div>
    
    </div>
    <div class="box">
		  <div class="widget">
			  <div id="div3">
			    <h1 style="color: #383838; text-align: center;">Temperature of Water</h1>
	    		<h2 style="color: #787878; text-align: center;"> <span id="temperature1"></span></h2>	
	  		</div>	
      </div>
      <div class="widget">
			  <div>
			    <h1 style="color: #383838; text-align: center;">Waiting Time</h1>
	    		<h2 style="color: #787878; text-align: center;"> <span id="countdown"></span></h2>	
	  		</div>	
      </div>
    </div>
    
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  var countdownTime;
  
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  
function startTemperatureCountdown() {
  // Constants
  const startingTemperature = 100; // Starting temperature in degrees Celsius
  const roomTemperature = 25; // Room temperature in degrees Celsius
  const coolingCoefficient = 0.00037632; // Cooling coefficient for Newton's cooling equation

  // Calculate the time required to reach room temperature
  const timeToCool = Math.log(roomTemperature / startingTemperature) / (-coolingCoefficient);

  // Function to update the temperature countdown
  function updateTemperatureCountdown() {
    const currentTime = new Date().getTime() / 1000; // Current time in seconds
    const elapsedTime = currentTime - startTime; // Elapsed time since countdown started

    // Calculate the temperature at the current time using Newton's cooling equation
    const currentTemperature = startingTemperature * Math.exp(-coolingCoefficient * elapsedTime);

    // Update the temperature display on the web page
    const temperatureElement1 = document.getElementById('temperature1');
    temperatureElement1.textContent = `${currentTemperature.toFixed(2)}°C`;

    // Check if the countdown is finished (reached room temperature)
    if (elapsedTime >= timeToCool) {
      temperatureElement1.textContent = 'Temperature: Room temperature reached';
      return;
    }

    // Schedule the next update after 1 second
    setTimeout(updateTemperatureCountdown, 1000); // Adjust the update interval here
  }

  // Start the temperature countdown
  const startTime = new Date().getTime() / 1000; // Start time in seconds
  updateTemperatureCountdown();
}
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    // Received a message from the WebSocket server
  const message = event.data;
  if (message.includes(":")) {
    // Split the message into identifier and value
    var parts = message.split(":");
    var identifier = parts[0];
    var value = parts[1];
    countdownTime = value;
    // Start the temperature countdown
     
    

    // Check the identifier and update the corresponding variable
    if (identifier === "temptime") {
      startTemperatureCountdown();
    }
    if (identifier === "temptime1"){
      updateCountdown();
    }
  } else {
    // Update the oledtxt element
   // var oledtxtElement = document.getElementById("oledtxt");
    document.getElementById("oledtxt").innerText = message;
   //document.getElementById("oledtxt").innerText = message;
    }
  }
  function onLoad(event) {
    initWebSocket();
   // initButton();
  }
  //function initButton() {
    //document.getElementById('button').addEventListener('click', toggle);
 // }
  function toggle(){
    websocket.send('toggle');
  }
  
  // Get the countdown element by its id
    const countdownElement = document.getElementById('countdown');

    // Function to update the countdown
    function updateCountdown() {
      // Calculate the minutes and seconds
      const minutes = Math.floor(countdownTime / 60);
      const seconds = countdownTime % 60;

      // Display the countdown
      countdownElement.textContent = `Time remaining: ${minutes} minutes ${seconds} seconds`;

      // Decrease the countdown time by 1 second
      countdownTime--;

      // Check if the countdown is finished
      if (countdownTime < 0) {
        countdownElement.textContent = "Countdown finished";
        // Optionally, you can perform some actions here when the countdown finishes
        return;
      }

      // Schedule the next update after 1 second
      setTimeout(updateCountdown, 900);
    }

    // Start the countdown
    updateCountdown();
</script>
</body>
</html> 
)rawliteral";

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    ws.textAll((char*)data);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var) {
  Serial.println(var);
  if (var == "TEXT") {
    return "Welcome to ITea";
  }
  // Add more conditions if needed
  
  // Return a default value for unmatched cases
  return String();
}


void wifiTelegram()
{
  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  localIP = WiFi.localIP().toString();
  

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void setup() {
 
//wifi connection block begins
 Serial.begin(9600);

  wifiTelegram();
  // Connect to WiFi
  /*
    WiFi.begin(ssid, password);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(5000);
     Serial.print("*");
  }
  
  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.println(WiFi.localIP());// Print the IP address */
  initWebSocket();
   // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
   // Start ElegantOTA
   AsyncElegantOTA.begin(&server);  
   //Start server 
  server.begin();
  Serial.println("HTTP server started");
  secured_client.setInsecure();

//wifi connection block ends
  


pinMode(ledPin, OUTPUT);
pinMode(buzzerPin, OUTPUT);
pinMode(buttonRight, INPUT);
pinMode(touchButton, INPUT);
start_read_nfc();
start_timer();

}



void loop() {
 ws.cleanupClients();
  //digitalWrite(ledPinTest, ledStateTest);
  checkTelegram();

while(state == 0) {
    
    
    if(a) { // update oled only once
      digitalWrite(ledPin, HIGH);
      oled.clear();
      oled.setCursor(0, 0);
      oled.setFont(Callibri14);
      oled.println("      Welcome to ITea");
      oled.setFont(Callibri11_bold);
      oled.println("To write to tea card:");
      oled.setFont(Callibri11);
      oled.println("   text the bot '/write'");
      oled.setFont(Callibri11_bold);
      oled.print("To brew tea: ");  
      oled.setFont(Callibri11);     
      oled.println("press enter");
      a = false;
    }
    ws.textAll("Welcome to ITea");
    
    checkTelegram();

    if (digitalRead(buttonRight) == HIGH)
    {
    state = 1;
    }


    while(Telegram) {
        
        while(!a) {
          nfc.begin();
          oled.clear();
          oled.setCursor(0, 0);
          oled.setFont(Callibri11);
          oled.println("Provide info in Telegram.");
          a = true;
        }

      
      while(!writingLoop) {
        checkTelegram();
      }

      while(writingLoop) {
            oled.println("Place tea card left.");
            Serial.println("Writing loop"); 
            if (nfc.tagPresent()) {
            NdefMessage message = NdefMessage();
            message.addTextRecord(messageTelegram);

            bool success = nfc.write(message);
            if (success) {
              Serial.println("Success. Try reading this tag with your phone."); 
              oled.clear();
              oled.setCursor(0, 0);
              oled.setFont(Callibri11);
              oled.println("Writing on tea card"); 
              oled.println("was successful."); 
              delay(3000);
              writingLoop = false;  
              Telegram = false;    
            } else {
              Serial.println("Write failed.");
            }
            }
            
          delay(1000);
      }

      delay(50);
    }

}

  while (state == 1)
  {
    read_show_nfc();
    Serial.println("I am inside state 1");
    checkTelegram();
  }


  while (state == 2)
  {
    
    yield(); // This will prevent wdt reset
    // Serial.println("I am inside state 2");
    // oled.clear();
    oled.setCursor(0, 0);
    oled.setFont(Callibri11_bold);
    oled.println(name);
    oled.setFont(Callibri11);
    oled.print(spoon);
    oled.print(" spoons, ");
    oled.print(g);
    oled.println("g");

    
    if (infusions == 2)
    {
      
      if (digitalRead(touchButton) == HIGH)
      {
        showInfusion1 = !showInfusion1;
        delay(50);
      }

      if (showInfusion1)
      {
        oled.println("1. Infusion > ");
        oled.print(temp1);
        oled.print("C; ");
        oled.print(min1);
        oled.print("min ");
        oled.print(sec1);
        oled.println("s    ");
      }
      else
      {
        oled.println("< 2. Infusion");
        oled.print(temp2);
        oled.print("C; ");
        oled.print(min2);
        oled.print("min ");
        oled.print(sec2);
        oled.println("s   ");
      }
    }

    if (infusions == 1)
    {
      oled.print("Temperature: ");
      oled.print(temp1);
      oled.println("C");
      oled.print("Time: ");
      oled.print(min1);
      oled.print("min ");
      oled.print(sec1);
      oled.println("s     ");
    }
   
    if (digitalRead(buttonRight) == HIGH)
    {

      if (showInfusion1)
      {
        temp = temp1;
        timeTea = time1;
      }
      else
      {
        temp = temp2;
        timeTea = time2;
      }

      timeTemp = calculateT(temp); // if formula works
      state = 3;
      countdown = timeTemp;
      
      //AsyncWebSocket.sendTXT();
    }
  }

  if (state == 3)
  {
    oled.clear();
     
    weight_timer();
   
  }
  


delay(2000);

}


