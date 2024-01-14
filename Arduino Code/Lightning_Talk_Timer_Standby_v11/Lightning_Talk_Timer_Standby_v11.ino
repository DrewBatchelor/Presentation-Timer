//Deek Robot Arduino Pro Mini
// ACBatchelor 14/01/24 vX
//Needs File:  Functions.ino

#include <Adafruit_NeoPixel.h>

#define LED_PIN 4
#define COLOR_ORDER GRB
#define NUM_LEDS 27

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);


// Inputs
#define potPin1 A0     // select the input pin for the potentiometer
#define potPin2 A1     // select the input pin for the potentiometer
#define goButton 3     //This is the start button, it can be on any digital pin
#define resetButton 2  //This is the reset button it must be on an interrupt pin, thats 2 or 3 on a Uno but different on other boards.

// Timers

unsigned long duration1;  // total time of countdown typically 5 mins

int minMinutes = 1;
int maxMinutes = 27;

long minduration = 60000L * minMinutes;  //will be 60 000 which is 1 min in millis
//this should be at least 2*shortEnd (test at 20000)
long endTime;
long shortEnd = 10000;  //will be 10 000 which is 10 sec in millis (test at 6000)
long longEnd = 60000;   //will be 60 000 which is 1 mins in millis test at 12000)
long potStep = 60000;   // 30 000 is 0.5mins per click of the pot, so max is 6 mins

unsigned int greenInt;  //beware, max duraion about 38 min
unsigned int yellowInt;
unsigned int redInt;
unsigned int discoSpeed = 50;  //disco pace
int discoduration = 6000;

int potValue1 = 0;  // variable to store the value coming from the sensor1
int potValue2 = 0;  // variable to store the value coming from the sensor2
int bright = 100;
int brightOld =20;

// Logic
int state = 0;
void (*resetFunc)(void) = 0;  // create a standard reset function

/*--------Standby------------------------*/

int standbyDelay = 5000;
long lastChange = 0;
int TimerOld = 0;

/*--------FIRE------------------------*/
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING 55  //75

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 180  //180

#define FIREDELAY 175

/*--------Colours------------------------*/
uint32_t green = strip.Color(0, bright, 0);
uint32_t yellow = strip.Color(bright / 2, bright / 2, 0);
uint32_t red = strip.Color(bright, 0, 0);
uint32_t purple = strip.Color(bright / 2, 0, bright / 2);

void setup() {
  //Serial.begin(9600);
  pinMode(resetButton, INPUT);  //have physical pullup
  pinMode(goButton, INPUT);     //have physical pullup
  pinMode(potPin1, INPUT);
  pinMode(potPin2, INPUT);

  attachInterrupt(digitalPinToInterrupt(resetButton), button_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(goButton), buttonGo_ISR, FALLING);

  delay(100);  // sanity delay

  strip.begin();  // Initialize NeoPixel strip object (REQUIRED)
  strip.clear();
  strip.show();  // Initialize all pixels to 'off'
}

void loop() {
  switch (state) {
    case 0:  //Ready
      readyState();
      break;
    case 1:  //going
      whenGo();
      colorWipe(green, greenInt);    // Green
      colorWipe(yellow, yellowInt);  // Yellow
      colorWipe(red, redInt);        // Red
      theaterChaseRainbow(discoSpeed);
      state = 0;
      break;

      strip.clear();
      strip.show();
  }
}

/*Interrupt pin for Reset*/
void button_ISR() {
  state = 0;
  strip.clear();
  strip.show();
  resetFunc();
}

void buttonGo_ISR() {
  state = 1;
}

void whenGo() {
  strip.clear();
  strip.fill(purple, 0, 8);    
  strip.show();
  delay(400);
  strip.clear();
  TimerOld = 0; //forces it to display the time when restarted
}

void setTimes() {
  duration1 = potStep * potValue1;
  duration1 = max(duration1, minduration);  //stops it going less that 1 minute

  if (duration1 < (longEnd * 2)) {
    endTime = shortEnd;  //final should be 1 minute 60 000
  } else {
    endTime = longEnd;  //final is longer
  }
  greenInt = (duration1 - endTime) / NUM_LEDS;
  yellowInt = endTime / (2 * NUM_LEDS);
  redInt = endTime / (2 * NUM_LEDS);
}


// Change bright on a second Potentiometer
void changeBright() {
  green = strip.Color(0, bright, 0);
  yellow = strip.Color(bright / 2, bright / 2, 0);
  red = strip.Color(bright, 0, 0);
  purple = strip.Color(bright / 2, 0, bright / 2);
  brightOld = bright/5;
}

//

/*******ready is the default state for this beast ********/
void readyState() {

  potValue1 = analogRead(potPin1);                     //0 to 1023
  potValue1 = map(potValue1, 0, 1010, 1, maxMinutes);  // should be 1024, but I got some flicker at the end so reduced to 1010 as a cludge fix
 
  potValue2 = analogRead(potPin2);                //0 to 1023
  bright = map(potValue2, 0, 1023, 40, 254);  // should be 1024, but I got some flicker at the end so reduced to 1010 as a cludge fix
 

  /*Serial.print("lastChange ");
  Serial.print(lastChange);
  Serial.print(" TimerOld ");
  Serial.print(TimerOld);
  Serial.print(" potValue1 ");
  Serial.println(potValue1);*/
  if (TimerOld != potValue1||brightOld != bright/5) {
    setTimes();
    changeBright();
    strip.fill(0, potValue1, NUM_LEDS);  
    for (int i = 0; i < potValue1; i++) {
      if ((i + 1) % 5 == 0) {
        strip.setPixelColor(i, purple);  //  Set pixel's color (in RAM)
      } else {
        strip.setPixelColor(i, green);  //  Set pixel's color (in RAM)
      }
      strip.show();
      lastChange = millis();
      TimerOld = potValue1;
    }
  delay(10);
  }

  if ((millis() - lastChange) > standbyDelay) {
    Fire(COOLING, SPARKING, FIREDELAY);
  }
}
