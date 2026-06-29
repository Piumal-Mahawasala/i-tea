#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>

/*In this code, the isTemperatureRising function calculates the difference between
the current and previous temperature readings. If the temperature change is greater than
or equal to the specified threshold (TEMPERATURE_CHANGE_THRESHOLD), the function returns true,
indicating that the temperature is rising quickly.

The loop function checks the time interval (SAMPLE_INTERVAL) and reads the current temperature
from the DHT22 sensor. If the temperature exceeds the threshold and the temperature is rising quickly,
the triggerAlarm function is called.

The triggerAlarm function is responsible for implementing your alarm logic.
In this example, it simply prints a message to the serial monitor indicating that the alarm has been triggered.
You can modify this function to trigger any desired alarm action.

Similarly, the stopAlarm function is called after the specified alarm duration (ALARM_DURATION) has passed.
It can be customized to implement your desired stop alarm action.

Please note that you may need to adjust the temperature threshold, temperature change
threshold, sample interval, and alarm duration according to your specific requirements.*/

#define DHT_PIN D5     // DHT22 data pin
#define DHT_TYPE DHT22 // DHT sensor type

#define TEMPERATURE_THRESHOLD 26         // Temperature threshold for triggering the alarm (adjust as needed)
#define TEMPERATURE_CHANGE_THRESHOLD 1.20 // Temperature change threshold for triggering the alarm (adjust as needed)
#define SAMPLE_INTERVAL 5000             // Interval between temperature readings in milliseconds (adjust as needed)
#define ALARM_DURATION 10               // Alarm duration in seconds (3 minutes)

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long previousMillis = 0;
unsigned long startMillis = 0;
float previousTemperature = 0;
bool isAlarmActive = false;
bool next = false;

bool isTemperatureRising(float currentTemperature, float previousTemperature); // Function prototype for isTemperatureRising()
void triggerAlarm();                                                           // Function prototype for triggerAlarm()
void stopAlarm();                                                              // Function prototype for stopAlarm()


float baseTemperature = 0.00;

int long countdowntime;                  // Countdown Variable bzw. Resetwert
int val;                                 // Variable für aktuellen Zustand 
int lastval;                             // Variable für den vorherigen Zustand
int long minutes;                        // Countdown in Minuten
int long seconds;                        // Countdown in Sekunden
volatile unsigned char tick;             // Variable für den Timer
int inputtime ;                          // Variable für analogeingang 


// OLED
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void start_dht()
{
    dht.begin();
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLE_INTERVAL)
    {
        baseTemperature = dht.readTemperature();

        if (isnan(baseTemperature))
        {
            Serial.println("Failed to read temperature from DHT sensor!");
            return;
        }

        Serial.print("Base Temperature: ");
        Serial.println(baseTemperature);


    }

}

void temp_alarm()
{
    while (next == false) {

        display.clearDisplay(); // Clear display
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.print("Base Temperature: ");
        display.print(baseTemperature);
        display.display();
    
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= SAMPLE_INTERVAL)
    {
        float currentTemperature = dht.readTemperature();

        if (isnan(currentTemperature))
        {
            Serial.println("Failed to read temperature from DHT sensor!");
            return;
        }

        Serial.print("Temperature: ");
        Serial.println(currentTemperature);

        if (currentTemperature > TEMPERATURE_THRESHOLD && isTemperatureRising(currentTemperature, baseTemperature) && isAlarmActive == false)
        {
            triggerAlarm();
            startMillis = currentMillis;
        }


        while (isAlarmActive == true) {
            unsigned long currentMillis = millis();
            if (currentMillis != previousMillis)                   // wenn sich 'val' von 'lastval' unterscheidet
            {                                     //
            countdowntime--;                    // inkrementiere 'countdowntime'
            }                                     //

              minutes = countdowntime/60;           // Aufteilung von 'countdowntime' in Minuten
            seconds = countdowntime%60;           // und Sekunden 
                    
            if (minutes<10)                       // wenn minuten unter 10
            {                                     //   
                Serial.print('0');                     // schreibe "0" vor minuten
                //display.clearDisplay(); // Clear display
                //display.setTextSize(2);
                //display.setCursor(0, 0);
                //display.print("0");
                //display.display();
            }                                     //
                    
            Serial.print(minutes);                   // gebe Minuten aus
           Serial.print(":");                       // schreibe ":"
                //display.setTextSize(2);
                //display.setCursor(0, 35);
                //display.print(minutes);
                //display.print(":");
               // display.display();
                
            if (seconds<10)                       // wenn Sekunden unter 10 
            {                                     //  
                Serial.print("0");                     // schreibe erst Null und dann
                Serial.print(seconds);                 // die Sekunden
                Serial.print(' ');                     // entferne Werte nach sekunden 
            }                                     //
            else                                  //
            {                                     //
                Serial.print(seconds);                 // schreibe die Sekunden normal
                Serial.print(" ");                     //
            } 
                
            previousMillis = currentMillis;
            delay(1000);                           // 1 sekunde Pause

                if (minutes == 0 && seconds == 0)
                {
                    stopAlarm();
                }
            }
        
        
        //previousTemperature = currentTemperature;
                
    }



    delay(3000);
    }
     
}

bool isTemperatureRising(float currentTemperature, float baseTemperature)
{
    return (currentTemperature - baseTemperature) >= TEMPERATURE_CHANGE_THRESHOLD;
}

void triggerAlarm()
{
    // Implement your alarm logic here
    Serial.println("Alarm triggered!");
    isAlarmActive = true;
    countdowntime = ALARM_DURATION;
}

void stopAlarm()
{
    // Implement your stop alarm logic here
    Serial.println("Alarm stopped!");
    isAlarmActive = false;
    next = true;
}

void printTime()
{
  minutes = countdowntime/60;           // Aufteilung von 'countdowntime' in Minuten
  seconds = countdowntime%60;           // und Sekunden 
        
  if (minutes<10)                       // wenn minuten unter 10
  {                                     //   
    Serial.print('0');                     // schreibe "0" vor minuten
  }                                     //
         
  Serial.print(minutes);                   // gebe Minuten aus
  Serial.print(":");                       // schreibe ":"
      
  if (seconds<10)                       // wenn Sekunden unter 10 
  {                                     //  
    Serial.print("0");                     // schreibe erst Null und dann
    Serial.print(seconds);                 // die Sekunden
    Serial.print(' ');                     // entferne Werte nach sekunden 
  }                                     //
  else                                  //
  {                                     //
    Serial.print(seconds);                 // schreibe die Sekunden normal
    Serial.print(" ");                     //
  }                                     // 
  
}

