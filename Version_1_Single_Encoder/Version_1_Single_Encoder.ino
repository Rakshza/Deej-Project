
#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>

#define TFT_CS     10
#define TFT_RST    8
#define TFT_DC     9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);  

#define BACKCOLOR 0x18E3
#define BARCOLOR 0x0620
#define SCALECOLOR 0xFFFF


// Rotary Encoder Pins - CLK and SW must go on interrupt pins, pins 2 and 3 for most Arduinos
const int clkPin = 3;  // CLK pin
const int dtPin = 4;  // DT pin
const int swPin = 2;   // SW pin

// Variables
unsigned long previousTime = 0;
bool Mute = false;                        // Mute status
int encoderValue;                         // mV out of 1023 sent to Deej
int range = 30;                           // Encoder range, 30 = 3 volume per pip, 100 = 1 volume per pip, bigger number = more turns to get to max volume
int unMute = 0;                           // Unmute volume
volatile int counter = range / 2;         // Rotary encoder counter (start at half)
volatile boolean clkState = 0;            // CLK pin state
volatile boolean encoderPressed = false;  // Button press flag
unsigned long debounceTime = 0;           // Debounce time
const unsigned long debounceDelay = 500;  // Debounce delay in milliseconds, ignore button presses after the first one for this duration

int LastPercent = 0;

void setup() 
{

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
  
// tft.setRotation(tft.getRotation()+1);
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
    } else 
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

void updateSliderValues() 
{
  // Map counter from rotary encoder to same range as other sliders
  if (Mute == true) 
  {
    encoderValue = 0;
  } else 
  {
    encoderValue = map(counter, 0, range, 0, 1024);
  }

  int newPercent;
   
  newPercent = int((encoderValue/1024.0)* 100.0);
  //Encoder 
    if (newPercent != LastPercent)
      {
      drawBar0(newPercent); 
      delay(50);    
      }

}

void drawBar0 (int nPer)
{

  if(nPer < LastPercent)
    {
    tft.fillRect(11, 20 + (100-LastPercent), 20, LastPercent - nPer,  BACKCOLOR);     
    }
  else
    {
    tft.fillRect(11, 20 + (100-nPer), 20, nPer - LastPercent,  BARCOLOR);
    }    
  LastPercent = nPer;  
  
}

void sendSliderValues() 
{
  // Create String
  String builtString = String("");
  // Add encoder value
  builtString += String((int)encoderValue);
  // Send the built string
  Serial.println(builtString);
}

void printSliderValues() 
{
  String printedString = String("Counter: ") + String(counter) + String("\tEncoder Value: ") + String(encoderValue) + String(" mV");
  Serial.write(printedString.c_str());
  Serial.write("\n");
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
    else {
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