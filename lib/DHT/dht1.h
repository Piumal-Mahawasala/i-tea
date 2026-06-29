#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>


/*In this code, the temperature is continuously read from the DHT22 sensor. 
If the temperature exceeds the threshold and the timer is not already active, 
the timer is started by recording the current time. Once the timer is active, 
the code checks if the elapsed time has reached the specified duration (3 minutes). 
If the timer has expired, you can perform any desired actions inside the if condition. 
The startTimer() and stopTimer() functions handle the timer activation and deactivation, respectively.

Adjust the temperature threshold and timer duration according to your requirements.*/


#define DHT_PIN D5        // DHT22 data pin
#define DHT_TYPE DHT22   // DHT sensor type

#define TEMPERATURE_THRESHOLD 27   // Temperature threshold for triggering the timer (adjust as needed)
#define TIMER_DURATION 180         // Timer duration in seconds (3 minutes)

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long startTime = 0;
bool isTimerActive = false;

void startTimer();  // Function prototype for startTimer()
void stopTimer();   // Function prototype for stopTimer()

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000);  // Delay between readings (adjust as needed)

  float temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println("Failed to read temperature from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temperature);

  if (temperature > TEMPERATURE_THRESHOLD && !isTimerActive) {
    startTimer();
  }

  if (isTimerActive && millis() - startTime >= TIMER_DURATION * 1000) {
    // Timer expired
    stopTimer();
    // Perform any action you want when the timer expires
    Serial.println("Timer expired!");
  }
}

void startTimer() {
  startTime = millis();
  isTimerActive = true;
  Serial.println("Timer started!");
}

void stopTimer() {
  isTimerActive = false;
  Serial.println("Timer stopped!");
}
