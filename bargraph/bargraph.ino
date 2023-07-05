

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

//Analog Measurement Declarations
const int analogIn = A0;
int RawValue[5] = {0,0,0,0,0};
int LastPercent[5] = {0,0,0,0,0};

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello!");

  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(BACKCOLOR);
  

 
// tft.setRotation(tft.getRotation()+1);

  
}

void loop() 
{  
    int i,newPercent[5];

    for(i = 0; i < 5 ; i++);
    {
        RawValue[i] = analogRead(analogIn);
        newPercent[i] = int((RawValue[i]/1024.0)* 100.0);
    
        if (newPercent[i] != LastPercent[i])
        {
         drawBar(i,newPercent[i]); 
         delay(50);    
        }
    }
     
}

void drawBar (int SliderNo, int nPer)
{
  switch(SliderNo) 
  {
        case 0:
            if(nPer < LastPercent[SliderNo])
                {
                tft.fillRect(11, 20 + (100-LastPercent[SliderNo]), 22, LastPercent[SliderNo] - nPer,  BACKCOLOR);     
                }
                else
                {
                tft.fillRect(11, 20 + (100-nPer), 22, nPer - LastPercent[SliderNo],  BARCOLOR);
                 }    
             LastPercent[SliderNo] = nPer;
            break;
        case 1:
              if(nPer < LastPercent[SliderNo])
                {
                tft.fillRect(34, 20 + (100-LastPercent[SliderNo]), 22, LastPercent[SliderNo] - nPer,  BACKCOLOR);     
                }
                else
                {
                tft.fillRect(34, 20 + (100-nPer), 22, nPer - LastPercent[SliderNo],  BARCOLOR);
                 }    
             LastPercent[SliderNo] = nPer;
            break;
        case 2:
            if(nPer < LastPercent[SliderNo])
                {
                tft.fillRect(57, 20 + (100-LastPercent[SliderNo]), 22, LastPercent[SliderNo] - nPer,  BACKCOLOR);     
                }
                else
                {
                tft.fillRect(57, 20 + (100-nPer), 22, nPer - LastPercent[SliderNo],  BARCOLOR);
                 }    
             LastPercent[SliderNo] = nPer;
            break;
        case 3:
            if(nPer < LastPercent[SliderNo])
                {
                tft.fillRect(80, 20 + (100-LastPercent[SliderNo]), 22, LastPercent[SliderNo] - nPer,  BACKCOLOR);     
                }
                else
                {
                tft.fillRect(80, 20 + (100-nPer), 22, nPer - LastPercent[SliderNo],  BARCOLOR);
                 }    
             LastPercent[SliderNo] = nPer;
            break;
        case 4:
            if(nPer < LastPercent[SliderNo])
                {
                tft.fillRect(103, 20 + (100-LastPercent[SliderNo]), 22, LastPercent[SliderNo] - nPer,  BACKCOLOR);     
                }
                else
                {
                tft.fillRect(103, 20 + (100-nPer), 22, nPer - LastPercent[SliderNo],  BARCOLOR);
                 }    
             LastPercent[SliderNo] = nPer;
            break;
        default:
            
        
            break;
    
  }
}


