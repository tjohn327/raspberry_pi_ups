#include <Arduino.h>
#include <Wire.h>
#include "TFT_eSPI.h"
#include <SPI.h>

//Pin Defenitions
#define VIN_DAC 25
#define COUT_DAC 26
#define COUT_DAC_PULLDOWN 27
#define CIN_ADC 37
#define COUT_ADC 38
#define VOUT_ADC 39
#define BUTTON_PIN 32
#define BUZZER_PIN 33
#define BUZZ_CHANNEL 0
#define SDA_PIN 21
#define SCL_PIN 22

//Display definitions
#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif
#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif
#define TFT_MOSI            19
#define TFT_SCLK            18
#define TFT_CS              5
#define TFT_DC              16
#define TFT_RST             23
#define TFT_BL          4   // Display backlight control pin

//I2C defenitions
#define I2C_ADD 0x6A
#define REG_VBAT 0x0E
#define REG_CONV_ADC 0x02
#define BYTE_CONV_ADC_START B10011101
#define BYTE_CONV_ADC_STOP B00011101
#define REG_VBUS 0x11
#define REG_STATUS 0x0B

//VIN DAC Values
#define VIN_14V 0
#define VIN_12V 90
#define VIN_9V 160
#define VIN_7_5V 195
#define VIN_6V 233
#define VIN_5V 255

//COUT DAC Values
#define COUT_1A 1
#define COUT_1_5A 5
#define COUT_2A 10
#define COUT_3A 17

extern uint8_t state;
extern uint8_t progress;
extern TFT_eSPI tft;

extern void IRAM_ATTR ButtonPress();
extern void PinSetup(void);
extern void Beep(uint8_t type);
extern void DisplaySetup(void);
extern void BeepError(void);
extern void I2CSetup (void);
extern bool CheckI2C (void);
extern void Reset(void);
extern unsigned int _readAnalog(int pin);
extern void _adcReady(void);
extern float ReadVBus(void);
extern float ReadVBat(void);
extern byte ReadStatus(void);
extern float ReadCIN(void);
extern float ReadVOUT(void);
extern bool TestAndPrintParams(void);
extern bool TestCOUT(void);
extern bool TestVIN_COUT(void);
extern void Progress(void);

