#include <Arduino.h>
#include "tester.h"

uint8_t y_pos=15;
void setup()
{
  Serial.begin(115200);
  PinSetup();
  Beep(1);
  I2CSetup();
  DisplaySetup();
}

void loop()
{
  switch (state)
  {
  case 0:    
    tft.drawString("Ready",  tft.width() / 2, tft.height() / 2 );
    yield();
    break;

  case 10:
    y_pos=30;
    if (CheckI2C())
    {
      tft.fillScreen(TFT_BLACK);
      state = 20;
      tft.drawString("I2C OK",  0, y_pos );
      Progress();
    }
    else
    {
      tft.fillScreen(TFT_BLACK);      
      tft.drawString("I2C Failed!",  0, y_pos );
      Beep(2);
      Reset();
    }
    y_pos+=15;
    break;

  case 20:
    tft.drawString("VIN_COUT Test Started",  0, y_pos );
    y_pos+=15;
    if(TestVIN_COUT())
    {
      tft.drawString("VIN_COUT Test OK",  0, y_pos );
      state = 30;
    }
    else
    {
      tft.drawString("VIN_COUT Test Failed",  0, y_pos );
      Beep(2);
      Reset();
    }
    y_pos+=15;
    break;
  
  case 30:
    tft.drawString("Remove Input",  0, y_pos);
    y_pos+=15;
    Beep(3);
    delay(3000);
    tft.drawString("COUT Test started",  0, y_pos );
    y_pos+=15;
    if(TestCOUT())
    {
      tft.drawString("COUT Test OK",  0, y_pos );
      state = 40;
    }
    else
    {
      tft.drawString("COUT Test Failed",  0, y_pos );
      Beep(2);
      Reset();
    }
    y_pos+=15;
    break;
  case 40:
    tft.drawString("All tests Passed",  0, y_pos);
    Beep(4);
    tft.drawRect(0,0,230,5,TFT_WHITE);
    Reset();
    break;
  default:
    break;
  }
}