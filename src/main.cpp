#include <Arduino.h>

// Pin indentities
int pokePin = 9;
int rewardLpin = 13;
int rewardRpin = 13;
int odorPin = 13;
int ledLpin = 13;
int ledRpin = 13;

// Various intitial conditions
int val = 1; // Initial poke state
int valO;
int pokeNum = 0; // Poke counter
int timer;
int state = 0;
int nextState;
int ITI = 2000; // Intertrial interval

// Timer variables
int timerStart;
int timeWait;
int timeNow;
bool doTimer;

// Block variables
int nTrials = 20;

//---------------------------------SETUP----------------------------------------

void setup() {
    Serial.begin(9600);
    pinMode(pokePin, INPUT);
    pinMode(rewardLpin, OUTPUT);
    pinMode(rewardRpin, OUTPUT);

    while (! Serial);
    Serial.println("Script running");
}

//----------------------------------LOOP----------------------------------------

void loop() {
    // Read and compare poke values
    valO = val; // store previous poke state
    val = digitalRead(pokePin); // record new poke state

    //..................Poke Detection..................
    if (val-valO < 0) {
      pokeNum = pokeNum + 1; // Update poke counter
      Serial.print("Poke #"); // Print to serial monitor
      Serial.println(pokeNum);
    }

    //..................Global Timer..................
    if (doTimer){
      timeNow = millis();
      if (timeNow - timerStart >= timeWait) {
        state = nextState;
        doTimer = 0;
      }
    }

    //..................Trial State Progression..................
    switch (state) {
    case 0: // ITI
      timeWait = ITI;
      timerStart = millis();
      doTimer = true;
      nextState = 1;
      Serial.print("State 0: ITI"); // Print state to serial monitor
      break;
    case 1: // Odor delivery

      break;
  }
}
