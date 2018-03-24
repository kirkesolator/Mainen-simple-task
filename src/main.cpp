#include <Arduino.h>

// Pin indentities
int pokePin = 9;
int rewardPins[] = {13,13};
int odorPin = 13;
int ledPins[] = {13,13};

// Pokes
int pokeState = 1; // Initial poke state
int pokeStateP; // Previous poke state
int pokeNum; // Poke counter
int pokeIn;
int pokeOut;

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
int rewardDuration = 100;
int rewardID;
char* rewardStr[]={"Left", "Right", "None"};

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
int state = 1;
int nextState;

// --------------------------------RSG------------------------------------------
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/atomic.h>
// The following addresses a problem in version 1.0.5 and earlier of the Arduino IDE
// that prevents randomSeed from working properly.
//        https://github.com/arduino/Arduino/issues/575
#define randomSeed(s) srandom(s)

volatile uint32_t seed;  // These two variables can be reused in your program after the
volatile int8_t nrot;    // function CreateTrulyRandomSeed()executes in the setup()
                         // function.

void CreateTrulyRandomSeed()
{
  seed = 0;
  nrot = 32; // Must be at least 4, but more increased the uniformity of the produced
             // seeds entropy.

  // The following five lines of code turn on the watch dog timer interrupt to create
  // the seed value
  cli();
  MCUSR = 0;
  _WD_CONTROL_REG |= (1<<_WD_CHANGE_BIT) | (1<<WDE);
  _WD_CONTROL_REG = (1<<WDIE);
  sei();

  while (nrot > 0);  // wait here until seed is created

  // The following five lines turn off the watch dog timer interrupt
  cli();
  MCUSR = 0;
  _WD_CONTROL_REG |= (1<<_WD_CHANGE_BIT) | (0<<WDE);
  _WD_CONTROL_REG = (0<< WDIE);
  sei();
}

ISR(WDT_vect)
{
  nrot--;
  seed = seed << 8;
  seed = seed ^ TCNT1L;
}
//---------------------------------SETUP----------------------------------------

void setup() {
    CreateTrulyRandomSeed(); // Calls the true random seed generator from above
    randomSeed(seed);

    // Open serial port
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
    Serial.print("Random seed generated: ");
    Serial.println(seed);
    Serial.println("Block start");
}

//----------------------------------LOOP----------------------------------------

void loop() {
  while (trialN <= nTrials){
    // Read and compare poke values
    pokeStateP = pokeState; // store previous poke state
    pokeState = digitalRead(pokePin); // record new poke state

    //..................Poke Detection..................
    if (pokeState-pokeStateP < 0) {
      pokeNum = pokeNum + 1; // Update poke counter
      Serial.print("."); // Print to serial monitor
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
        pokeNum = 0;
        trialN = trialN + 1;
        if (trialN > nTrials) { // Break statement if number of trials have been reached
          Serial.println("Block finished"); // Print state to serial monitor
          break;
        }
        Serial.print("\nTrial #: "); // Print state to serial monitor
        Serial.print(trialN); // Print state to serial monitor
        Serial.print("\n Stage 1: ITI"); // Print state to serial monitor

        // Timer initialize
        timeWait = ITI;
        timerStart = millis();
        doTimer = true;
        nextState = 2;
        state = 0;
        break;

      case 2: // LED stimulus ON
        Serial.print("\n Stage 2: LED on"); // Print state to serial monitor

        // Determine this trial's reward location
        randInt = random(100);
        if (randInt <= Podor) {
          rewardID = 0;
        }
        else if (randInt > Podor){
          rewardID = 1;
        }
        // Serial.print(randInt); // Show randomly generated number

        // Determine if this trial's LED stim is congruent with reward location
        randInt = random(100);
        if (randInt <= PledCorrect) { // Congruent
          ledID = rewardID;
          Serial.print(": Congruent LED stimulus");
        }
        else if (randInt > PledCorrect) { // Not congruent
          ledID = (rewardID+1) % 2;
          Serial.print(": Incongruent LED stimulus!");
        }
        // Serial.println(randInt); // Show randomly generated number

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
        Serial.print("\n Stage 3: LED off"); // Print state to serial monitor
        digitalWrite(ledPins[ledID], LOW);

        // Timer initialize
        timeWait = 1;
        timerStart = millis();
        doTimer = true;
        nextState = 4;
        state = 0;
        break;

      case 4: // Waiting time between LED offset and reward devlivery
        Serial.print("\n Stage 4: odor delivery delay"); // Print state to serial monitor

        // Timer initialize
        timeWait = odorDelay;
        timerStart = millis();
        doTimer = true;
        nextState = 5;
        state = 0;
        break;

      case 5: // Odor delivery on
        Serial.print("\n Stage 5: odor delivery start: "); // Print state to serial monitor
        Serial.print(rewardStr[rewardID]); // Print state to serial monitor
        digitalWrite(odorPin, HIGH);

        // Timer initialize
        timeWait = odorDuration;
        timerStart = millis();
        doTimer = true;
        nextState = 6;
        state = 0;
        break;

      case 6: // Odor delivery off
        Serial.print("\n Stage 6: odor delivery end"); // Print state to serial monitor
        digitalWrite(odorPin, LOW);

        // Timer initialize
        timeWait = 1;
        timerStart = millis();
        doTimer = true;
        nextState = 7;
        state = 0;
        break;

      case 7: // Waiting time between LED offset and reward devlivery
        Serial.print("\n Stage 7: reward delay"); // Print state to serial monitor

        // Timer initialize
        timeWait = rewardDelay;
        timerStart = millis();
        doTimer = true;
        nextState = 8;
        state = 0;
        break;

      case 8: // Reward delivery started
        Serial.print("\n Stage 8: reward delivery start"); // Print state to serial monitor
        digitalWrite(rewardPins[rewardID], HIGH);

        // Timer initialize
        timeWait = rewardDuration;
        timerStart = millis();
        doTimer = true;
        nextState = 9;
        state = 0;
        break;

      case 9: // Reward delivery ended
        Serial.print("\n Stage 9: reward delivery end \n"); // Print state to serial monitor
        digitalWrite(rewardPins[rewardID], LOW);

        // Timer initialize
        timeWait = 1;
        timerStart = millis();
        doTimer = true;
        nextState = 1;
        state = 0;
        break;
      }
    }
}
