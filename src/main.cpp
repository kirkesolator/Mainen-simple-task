#include <Arduino.h>

// Pin indentities
int pokePin = 9;
int rewardPins[] = {13,13};
int odorPin = 13;
int ledPins[] = {13,13};

// Various intitial conditions
int pokeState = 1; // Initial poke state
int pokeStateP; // Previous poke state
int pokeNum = 0; // Poke counter
int pokeIn;
int pokeOut;
int state = 1;
int nextState;
int randInt;

// ITI
int ITI = 2000; // Intertrial interval

// Light stimulus
int ledDuration = 500; // LED stimulus on time (ms)
int ledID;

// Odor delivery
int odorDelay = 150;
int odorDuration = 500;

// Reward devlivery
int rewardDelay = 250;
int rewardDuration;
int rewardID;

// Timer variables
int timerStart;
int timeWait;
int timeNow;
bool doTimer = false;

// Block variables
int nTrials = 20;
int trialN = 0; // Trial counter

// Trial variables
int PledON = 90; // Probability of LED stimulus
int PledCorrect = 95; // Probability the LED stimulus is odor-congruent
int Preward = 90; // Probability of reward delivery
int Podor = 50; // Probability odor 1 vs 2 (50 = 50%)

//---------------------------------SETUP----------------------------------------

void setup() {
    Serial.begin(9600);
    pinMode(pokePin, INPUT);
    pinMode(rewardPins[0], OUTPUT);
    pinMode(rewardPins[1], OUTPUT);
    pinMode(odorPin, OUTPUT);
    pinMode(ledPins[0], OUTPUT);
    pinMode(ledPins[1], OUTPUT);

    // Wait with next stage until serial monitor is running
    while (! Serial);

    // Verify
    Serial.println("Script running");
}

//----------------------------------LOOP----------------------------------------

void loop() {
    // Read and compare poke values
    pokeStateP = pokeState; // store previous poke state
    pokeState = digitalRead(pokePin); // record new poke state

    //..................Poke Detection..................
    if (pokeState-pokeStateP < 0) {
      pokeNum = pokeNum + 1; // Update poke counter
      Serial.print("Poke #"); // Print to serial monitor
      Serial.println(pokeNum);
      pokeIn = millis(); // Record poke entry
    }
    else if (pokeState-pokeStateP > 0) { // Record poke exit
      pokeOut = millis();
    }


    //..................Global Timer..................
    if (doTimer){
      timeNow = millis(); // Time now
      if (timeNow - timerStart >= timeWait) { // Check if timer limit reached
        state = nextState; // Move on
        doTimer = false; // Turn off timer
      }
    }

    //..................Trial State Progression..................
    switch (state) {
    case 0: // Default state with no activity
      break;

    case 1: // ITI
      Serial.println("State 1: ITI"); // Print state to serial monitor

      // Timer initialize
      timeWait = ITI;
      timerStart = millis();
      doTimer = true;
      nextState = 2;
      state = 0;
      break;

    case 2: // LED stimulus ON
      Serial.println("State 2: LED on"); // Print state to serial monitor

      // Determine this trial's reward location
      randInt = random(0, 100);
      if (randInt <= 50) {
        rewardID = 0;
      }
      else {
        rewardID = 1;
      }

      // Determine if this trial's LED stim is congruent with reward location
      randInt = random(0, 100);
      if (randInt < PledCorrect) { // Congruent
        ledID = rewardID;
        Serial.println("  Congruent LED stimulus");
      }
      else { // Not congruent
        ledID = (rewardID+1) % 2;
        Serial.println("  Incongruent LED stimulus!");
      }

      // Switch on LED
      digitalWrite(ledPins[ledID], HIGH);

      // Timer initialize
      timeWait = ledDuration;
      timerStart = millis();
      doTimer = true;
      nextState = 3;
      state = 0;
      break;

    case 3: // LED stimulus off
      Serial.println("State 3: LED off"); // Print state to serial monitor
      digitalWrite(ledPins[ledID], LOW);

      // Timer initialize
      timeWait = 1;
      timerStart = millis();
      doTimer = true;
      nextState = 4;
      state = 0;
      break;

    case 4: // Waiting time between LED offset and reward devlivery
      Serial.println("State 4: odor delivery delay"); // Print state to serial monitor

      // Timer initialize
      timeWait = odorDelay;
      timerStart = millis();
      doTimer = true;
      nextState = 5;
      state = 0;
      break;

    case 5: // Odor delivery on
      Serial.println("State 5: odor delivery on"); // Print state to serial monitor
      digitalWrite(odorPin, HIGH);

      // Timer initialize
      timeWait = odorDuration;
      timerStart = millis();
      doTimer = true;
      nextState = 6;
      state = 0;
      break;

    case 6: // Odor delivery off
      Serial.println("State 6: odor delivery off"); // Print state to serial monitor
      digitalWrite(odorPin, LOW);

      // Timer initialize
      timeWait = 1;
      timerStart = millis();
      doTimer = true;
      nextState = 7;
      state = 0;
      break;

    case 7: // Waiting time between LED offset and reward devlivery
      Serial.println("State 7: reward delay"); // Print state to serial monitor

      // Timer initialize
      timeWait = rewardDelay;
      timerStart = millis();
      doTimer = true;
      nextState = 8;
      state = 0;
      break;

    case 8: // Odor delivery on
      Serial.println("State 8: reward delivery on"); // Print state to serial monitor
      digitalWrite(rewardPins[rewardID], HIGH);

      // Timer initialize
      timeWait = rewardDuration;
      timerStart = millis();
      doTimer = true;
      nextState = 9;
      state = 0;
      break;

  }
}
