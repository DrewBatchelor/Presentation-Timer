//Deek Robot Arduino Pro Mini 
// ACBatchelor 08/03/20 v7
//Needs File:  NeoPixelFunctions

#include <Adafruit_NeoPixel.h>

int potPin1 = A0;             // select the input pin for the potentiometer
int potPin2 = A1;             // select the input pin for the potentiometer
const int goButton = 3;      //This is the start button, it can be on any digital pin
const int resetButton = 2;      //This is the reset button it must be on an interrupt pin, thats 2 or 3 on a Uno but different on other boards.

#define PIXEL_PIN    4       // Digital IO pin connected to the NeoPixels.
const int PIXEL_COUNT = 27;  // Number of NeoPixels 28 fro strip 44 for ring

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

unsigned long duration1;       // total time of countdown typically 5 mins

int minMinutes = 1;
int maxMinutes = 27;

long minduration = 60000L * minMinutes; //will be 60 000 which is 1 min in millis
//this should be at least 2*shortEnd (test at 20000
long endTime;
long shortEnd = 10000;          //will be 10 000 which is 10 sec in millis (test at 6000)
long longEnd = 60000;           //will be 60 000 which is 1 mins in millis test at 12000)
long potStep = 60000;          // 30 000 is 0.5mins per click of the pot, so max is 6 mins

unsigned int greenInt;  //beware, max duraion about 38 min
unsigned int yellowInt;
unsigned int redInt;
unsigned int discoSpeed = 30;  //disco pace
int discoduration = 5000;

int potValue1 = 0;  // variable to store the value coming from the sensor1
int potValue2 = 0;  // variable to store the value coming from the sensor2
int bright = 20;

uint32_t green = strip.Color(0, bright, 0);
uint32_t yellow = strip.Color(bright / 2, bright / 2, 0);
uint32_t red = strip.Color(bright, 0, 0);
uint32_t purple = strip.Color(bright / 2, 0, bright / 2);

int state = 0;

void(* resetFunc) (void) = 0; // create a standard reset function

/*--------FastLED------------------------*/




// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  75

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 180
/*--------------------------------------*/



void setup() {
  pinMode(resetButton, INPUT); //have physical pullup
  pinMode(goButton, INPUT);   //have physical pullup
  pinMode(potPin1, INPUT);
  pinMode(potPin2, INPUT);

  attachInterrupt(digitalPinToInterrupt(resetButton), button_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(goButton), buttonGo_ISR, FALLING);
   
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.clear();
  strip.show();  // Initialize all pixels to 'off'
}

//bailout:
void loop() {

 switch (state)  {
  case 0: //Ready
    readyState();
  break;
  case 1: //going
  whenGo();
  colorWipe(green, greenInt);    // Green
  colorWipe(yellow, yellowInt); // Yellow
  colorWipe(red, redInt);      // Red
  theaterChaseRainbow(discoSpeed);
  state = 0;
  break;

  strip.clear();
  strip.show();

}
}

/*Interrupt pin for Reset*/
void button_ISR()
{
  state = 0;
  strip.clear();
  strip.show();
  resetFunc();
}


void buttonGo_ISR()
{
  state = 1;

}

void whenGo(){
  strip.clear();
  strip.show();
  strip.setPixelColor(0, purple);  
  strip.setPixelColor(1, purple);  
  strip.setPixelColor(2, purple); 
  strip.setPixelColor(3, purple); 
  strip.setPixelColor(4, purple);    
  strip.show();
  delay(400);
  strip.clear();
}

void setTimes() {
  duration1 = potStep * potValue1;
  duration1 = max(duration1, minduration); //stops it going less that 1 minute

  if (duration1 < (longEnd * 2)) {
    endTime = shortEnd; //final should be 1 minute 60 000
  } else {
    endTime = longEnd; //final is longer
  }
  greenInt = (duration1 - endTime) / PIXEL_COUNT;
  yellowInt = endTime / (2 * PIXEL_COUNT);
  redInt = endTime / (2 * PIXEL_COUNT);

}


// Change brightness on a second Potentiometer 
void changeBright(){
      potValue2 = analogRead(potPin2); //0 to 1023
      bright = map(potValue2, 0, 1024, 40, 200); // should be 1024, but I got some flicker at the end so reduced to 1010 as a cludge fix
      green = strip.Color(0, bright, 0);
      yellow = strip.Color(bright / 2, bright / 2, 0);
      red = strip.Color(bright, 0, 0);
      purple = strip.Color(bright / 2, 0, bright / 2);
    }

// 

/*******ready is the default state for this beast ********/
void readyState() {
    changeBright();
    potValue1 = analogRead(potPin1); //0 to 1023
    potValue1 = map(potValue1, 0, 1010, 1, maxMinutes);   // should be 1024, but I got some flicker at the end so reduced to 1010 as a cludge fix
    setTimes();
    strip.clear();
    for (int i = 0; i < potValue1; i++) {
      strip.setPixelColor(i, green);//  Set pixel's color (in RAM)
    }   
    strip.show();
    delay(100);
}

