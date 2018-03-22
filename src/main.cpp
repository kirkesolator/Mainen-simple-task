#include <Arduino.h>

int pokePin = 9;
int val = 1;
int valO;
int pokeNum = 0;
int timer;
//---------------------------------SETUP----------------------------------------

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(pokePin, INPUT);
    while (! Serial);
    Serial.println("Script running");
}

//----------------------------------LOOP----------------------------------------

void loop() {
    // Read and compare poke values
    valO = val; // store previous poke state
    val = digitalRead(pokePin); // record new poke state

    // check for poke entry:
    if (val-valO < 0) {
      pokeNum = pokeNum + 1; // Update poke counter
      Serial.print("Poke #"); // Print to serial monitor
      Serial.println(pokeNum);
    }
}
