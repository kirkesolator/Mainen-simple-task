#include <Arduino.h>

int pokePin = 9;
int val;
int valO;
int pokeNum = 0;
int timer;

/*
----------------------------------------------------------------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(pokePin, INPUT);
    Serial.println("Script running");
}

/*
----------------------------------------------------------------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------
*/

void loop() {
    // put your main code here, to run repeatedly:
    valO = val; // store previous poke state
    val = digitalRead(pokePin); // record new poke state
    // delay(50);
    if (val-valO < 0)
    {
      pokeNum = pokeNum + 1;
      Serial.println(pokeNum);
      // timer = millis();
      // while (millis() - timer < 50) {
      //   // Waiting...
      // }
    }
}
