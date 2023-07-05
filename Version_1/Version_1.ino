
#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>
#include "Ucglib.h"


#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);  
Ucglib_ST7735_18x128x160_SWSPI ucg(/*sclk=*/ 13, /*data=*/ 11, /*cd=*/ 9, /*cs=*/ 10, /*reset=*/ 8);


#define BACKCOLOR 0x18E3
#define BARCOLOR 0x07e0 + 0x001f
#define SCALECOLOR 0xFFFF

const int NUM_SLIDERS = 5;

// Slider Pins
const int slider1Pin = A0;
const int slider2Pin = A1;
const int slider3Pin = A2;
const int slider4Pin = A3;

// Rotary Encoder Pins
const int clkPin = 3;  // CLK pin
const int dtPin = 4;  // DT pin
const int swPin = 2;   // SW pin

// Encoder Sensitivity (pips for max volume)
const int sens1 = 30;   // One full turn
const int sens2 = 60;   // Two full turns
const int sens3 = 120;  // Four full turns

// Button States
const int numStates = 3;
const int LOW_STATE = 0;
const int MED_STATE = 1;
const int HIGH_STATE = 2;

// Variables
unsigned long previousTime = 0;
bool Mute = false;
int unMute = 0; 
volatile int counter = 0;                 // Rotary encoder counter
volatile boolean clkState = 0;            // CLK pin state
volatile boolean encoderPressed = false;  // Button press flag
unsigned long debounceTime = 0;           // Debounce time
const unsigned long debounceDelay = 200;  // Debounce delay in milliseconds
int currentState = LOW_STATE;
bool shutdownActive = false;
bool shutdownPressed = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
int range = sens1;  // Encoder range for max volume (how many pips to turn, for me 30 pips = 1 full rotation)

int analogSliderValues[NUM_SLIDERS];

int LastPercent[NUM_SLIDERS] = {0,0,0,0,0};

void setup() 
{
  // Setup sliders and inputs
  pinMode(slider1Pin, INPUT);
  pinMode(slider2Pin, INPUT);
  pinMode(slider3Pin, INPUT);
  pinMode(slider4Pin, INPUT);

  // Rotary encoder pins as input with pull-up resistors
  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dtPin, INPUT_PULLUP);
  pinMode(swPin, INPUT_PULLUP);

  // Attach interrupts to the CLK and SW pins
  attachInterrupt(digitalPinToInterrupt(clkPin), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(swPin), handleSwitch, FALLING);

  Serial.begin(9600);
  Serial.print("Hello!");

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(BACKCOLOR);
  DisplayText();
  tft.setRotation(0);
}

void loop() 
{


  // If encoder was pressed
  if (encoderPressed && (millis() - debounceTime) >= debounceDelay) 
    {
    Serial.println("Encoder button pressed!");
        if (Mute == true) 
        {
           Mute = false;
           counter = unMute;
        } 
        else 
        {
           Mute = true;
           unMute = counter;
        }
    encoderPressed = false;
    // debounceTime = 0;

    }  
  
  updateSliderValues();
  sendSliderValues();   // Actually send data (all the time)
  printSliderValues();  // For debug
  delay(500);
}

void DisplayText()
{
   //First Line
    tft.setRotation(3);
    tft.setCursor(5, 18);
    tft.setTextColor(BARCOLOR);
    tft.setTextSize(1);
    tft.println("MASTER");
   //Second Line
    tft.setRotation(3);
    tft.setCursor(5, 40);
    tft.setTextColor(BARCOLOR);
    tft.setTextSize(1);
    tft.println("MIC");
  //Third Line
    tft.setRotation(3);
    tft.setCursor(5, 60);
    tft.setTextColor(BARCOLOR);
    tft.setTextSize(1);
    tft.println("DISCORD");
  //Fourth Line
    tft.setRotation(3);
    tft.setCursor(5, 80);
    tft.setTextColor(BARCOLOR);
    tft.setTextSize(1);
    tft.println("GAMES");
  //Fifth Line
    tft.setRotation(3);
    tft.setCursor(5, 102);
    tft.setTextColor(BARCOLOR);
    tft.setTextSize(1);
    tft.println("MEDIA");
}

void updateSliderValues() 
{
  // Map counter from rotary encoder to same range as other sliders
  analogSliderValues[0] = map(counter, 0, range, 0, 1024);
  //Slider Values
  analogSliderValues[1] = analogRead(slider1Pin);
  analogSliderValues[2] = analogRead(slider2Pin);
  analogSliderValues[3] = analogRead(slider3Pin);
  analogSliderValues[4] = analogRead(slider4Pin);
  
  int newPercent[NUM_SLIDERS];
   
    newPercent[0] = int((analogSliderValues[0]/1024.0)* 100.0);
    newPercent[1] = int((analogSliderValues[1]/1024.0)* 100.0);
    newPercent[2] = int((analogSliderValues[2]/1024.0)* 100.0);
    newPercent[3] = int((analogSliderValues[3]/1024.0)* 100.0);
    newPercent[4] = int((analogSliderValues[4]/1024.0)* 100.0);
    //Encoder 
    if (newPercent[0] != LastPercent[0])
      {
      drawBar0(newPercent[0]); 
      delay(50);    
      }
    //Slider1 
    if (newPercent[1] != LastPercent[1])
      {
        drawBar1(newPercent[1]); 
        delay(50);

      }
    //Slider2
    if (newPercent[2] != LastPercent[2])
      {
      drawBar2(newPercent[2]); 
      delay(50);    
      }
    //Slider3
    if (newPercent[3] != LastPercent[3])
      {
      drawBar3(newPercent[3]); 
      delay(50);    
      }
    //Slider4
    if (newPercent[4] != LastPercent[4])
      {
      drawBar4(newPercent[4]); 
      delay(50);    
      }
}

void drawBar0 (int nPer)
{

  if(nPer < LastPercent[0])
    {
    tft.fillRect(11,10+(100-LastPercent[0]), 20, (LastPercent[0] - nPer),  BACKCOLOR);     
    }
  else
    {
    tft.fillRect(11,10+(100-nPer), 20, (nPer - LastPercent[0]),  BARCOLOR);
    }    
  LastPercent[0] = nPer;  
  
}

void drawBar1 (int nPer)
{

  if(nPer < LastPercent[1])
    {
    tft.fillRect(32, 10 + (100-LastPercent[1]), 20, LastPercent[1] - nPer,  BACKCOLOR);     
    }
  else
    {
    tft.fillRect(32, 10 + (100-nPer), 20, nPer - LastPercent[1],  BARCOLOR);
    }    
  LastPercent[1] = nPer;  
  
}

void drawBar2 (int nPer)
{

  if(nPer < LastPercent[2])
    {
    tft.fillRect(53, 10 + (100-LastPercent[2]), 20, LastPercent[2] - nPer,  BACKCOLOR);     
    }
  else
    {
    tft.fillRect(53, 10 + (100-nPer), 20, nPer - LastPercent[2],  BARCOLOR);
    }    
  LastPercent[2] = nPer;  
  
}

void drawBar3 (int nPer)
{

  if(nPer < LastPercent[3])
    {
    tft.fillRect(74, 10 + (100-LastPercent[3]), 20, LastPercent[3] - nPer,  BACKCOLOR);     
    }
  else
    {
    tft.fillRect(74, 10 + (100-nPer), 20, nPer - LastPercent[3],  BARCOLOR);
    }    
  LastPercent[3] = nPer;  
  
}

void drawBar4 (int nPer)
{

  if(nPer < LastPercent[4])
    {
    tft.fillRect(95, 10 + (100-LastPercent[4]), 20, LastPercent[4] - nPer,  BACKCOLOR);     
    }
  else
    {
    tft.fillRect(95, 10 + (100-nPer), 20, nPer - LastPercent[4],  BARCOLOR);
    }    
  LastPercent[4] = nPer;  
  
}

void sendSliderValues() {
  // Create String
  String builtString = String("");

  // For each slider add the value to the string
  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  // Send the built string
  Serial.println(builtString);
}

void printSliderValues() 
{
  for (int i = 0; i < NUM_SLIDERS; i++) 
  {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}


// Interrupt handler for CLK pin
void handleEncoder() 
{
  // Read the current state of CLK and DT pins
  boolean clkStateNew = digitalRead(clkPin);
  boolean dtState = digitalRead(dtPin);

  // Check if CLK state has changed
  if (clkState != clkStateNew) 
  {
    // If CLK and DT pins are different, increment the counter
    if (clkStateNew != dtState) 
      {
      counter++;
      }
    // If CLK and DT pins are the same, decrement the counter
    else 
      {
      counter--;
      }
    // Limit how many pips to count for max volume
    counter = constrain(counter, 0, range);
  }

  // Update the CLK pin state
  clkState = clkStateNew;
}

// Interrupt handler for SW pin
void handleSwitch() 
{
  // Check if the debounce time has not elapsed
  if ((millis() - debounceTime) < debounceDelay) 
   {
    return;  // Ignore the button press
   }

  // Set the button pressed flag
  encoderPressed = true;

  // Update the debounce time
  debounceTime = millis();
}



void changeState() 
{
  currentState++;
  if (currentState >= numStates) 
    {
    currentState = LOW_STATE;
    }
  switch (currentState) 
    {
    case LOW_STATE:
      Serial.println("LOW STATE");
      counter = counter * sens1 / sens3;
      range = 30;     
      break;
    case MED_STATE:
      Serial.println("MED STATE");
      counter = counter * sens2 / sens1;
      range = 60;    
      break;
    case HIGH_STATE:
      Serial.println("HIGH STATE");
      counter = counter * sens3 / sens2;
      range = 120;
      break;
    }
}

