#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

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

#define TEMPERATURE_THRESHOLD 30         // Temperature threshold for triggering the alarm (adjust as needed)
#define TEMPERATURE_CHANGE_THRESHOLD 2.0 // Temperature change threshold for triggering the alarm (adjust as needed)
#define SAMPLE_INTERVAL 5000             // Interval between temperature readings in milliseconds (adjust as needed)
#define ALARM_DURATION 180               // Alarm duration in seconds (3 minutes)

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long previousMillis = 0;
float previousTemperature = 0;
bool isAlarmActive = false;

bool isTemperatureRising(float currentTemperature, float previousTemperature); // Function prototype for isTemperatureRising()
void triggerAlarm();                                                           // Function prototype for triggerAlarm()
void stopAlarm();                                                              // Function prototype for stopAlarm()

void setup()
{
    Serial.begin(9600);
    dht.begin();
}

void loop()
{
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

        if (currentTemperature > TEMPERATURE_THRESHOLD && isTemperatureRising(currentTemperature, previousTemperature))
        {
            triggerAlarm();
        }

        previousTemperature = currentTemperature;
        previousMillis = currentMillis;
    }

    if (isAlarmActive && currentMillis - previousMillis >= ALARM_DURATION * 1000)
    {
        stopAlarm();
    }
}

bool isTemperatureRising(float currentTemperature, float previousTemperature)
{
    return (currentTemperature - previousTemperature) >= TEMPERATURE_CHANGE_THRESHOLD;
}

void triggerAlarm()
{
    // Implement your alarm logic here
    Serial.println("Alarm triggered!");
    isAlarmActive = true;
}

void stopAlarm()
{
    // Implement your stop alarm logic here
    Serial.println("Alarm stopped!");
    isAlarmActive = false;
}
