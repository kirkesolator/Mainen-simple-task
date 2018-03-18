#include <Arduino.h>

unsigned long time;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Hello Computer");
    Serial.println("Hello Computer");
}

void loop() {
    // put your main code here, to run repeatedly:
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Serial.print("Time: ");
    time = micros();
    Serial.println(time);
    Serial.println("Test");
    Serial.println("Test");
}
