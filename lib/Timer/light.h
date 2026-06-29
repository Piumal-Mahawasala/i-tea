#include <Arduino.h>

void light_start() {
    pinMode(D0, OUTPUT);
    //pinMode(D5, OUTPUT);
}

void light_stop() {
    digitalWrite(D0, LOW); // green off
}

void light_go() {
    digitalWrite(D0, HIGH); // green on 
}

void light_blink() {
    digitalWrite(D0, HIGH); // green on 
    delay(500);
    digitalWrite(D0, LOW); // green off
    delay(500);
}


