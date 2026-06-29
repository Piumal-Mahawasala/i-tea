


#include <Arduino.h>

 

const int buzzerPin = D5; // Verbinde den Buzzer mit dem digitalen Pin D5

int buttonRight = D8; // rechter Knopf (kein Touch)
unsigned long lastPeriodStart;
const int onDuration = 1000;
const int periodDuration = 500;
bool alarm = true;

const int ledPin = D0; 

int ledState = LOW;

void start_buzzer()
{
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}


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
        tone(buzzerPin, 500, onDuration); // Spiele einen 550 Hz Ton für 'onDuration'
      }
      else
      {
        ledState = LOW;
        digitalWrite(ledPin, ledState);
        noTone(buzzerPin); // Schalte den Buzzer aus
      }
    }

 

    if (digitalRead(buttonRight) == HIGH)
    {
      alarm = false;
      digitalWrite(ledPin, LOW);
      noTone(buzzerPin); // Schalte den Buzzer aus
    }
  }
}


