#include "tester.h"

uint8_t state = 0;
uint8_t progress = 0;
TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

void IRAM_ATTR ButtonPress()
{
    if (state == 0)
    {
        Serial.println("Button pressed");
        pinMode(COUT_DAC_PULLDOWN, INPUT);
        state = 10;
        delay(1000);
    }
}

void PinSetup(void)
{
    pinMode(CIN_ADC, INPUT);
    pinMode(COUT_ADC, INPUT);
    pinMode(VIN_DAC, OUTPUT);
    pinMode(COUT_DAC, OUTPUT);
    pinMode(COUT_DAC_PULLDOWN, OUTPUT);
    pinMode(COUT_DAC_PULLDOWN, LOW);
    dacWrite(VIN_DAC, VIN_5V);
    dacWrite(COUT_DAC, 0);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(BUTTON_PIN, ButtonPress, FALLING);
    pinMode(BUZZER_PIN, OUTPUT);
    ledcSetup(BUZZ_CHANNEL, 1000, 8);
    ledcAttachPin(BUZZER_PIN, BUZZ_CHANNEL);
    ledcWrite(BUZZ_CHANNEL, 0);
}

void DisplaySetup(void)
{
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
}

void Beep(uint8_t type)
{
    switch (type)
    {
    case 1: //start
        ledcWrite(BUZZ_CHANNEL, 50);
        delay(50);
        ledcWrite(BUZZ_CHANNEL, 0);
        break;
    case 2: //error
        for (int i = 0; i < 3; i++)
        {
            ledcWrite(BUZZ_CHANNEL, 50);
            delay(30);
            ledcWrite(BUZZ_CHANNEL, 0);
            delay(30);
        }
        break;
    case 3: //input remove
        ledcWrite(BUZZ_CHANNEL, 50);
        delay(30);
        ledcWrite(BUZZ_CHANNEL, 0);
        break;
    case 4: //tesp passed
        ledcWrite(BUZZ_CHANNEL, 50);
        delay(500);
        ledcWrite(BUZZ_CHANNEL, 0);
        break;
    default:
        break;
    }
    
}

void BeepError(void)
{
    for (int i = 0; i < 3; i++)
    {
        ledcWrite(BUZZ_CHANNEL, 50);
        delay(30);
        ledcWrite(BUZZ_CHANNEL, 0);
        delay(30);
    }
}

void I2CSetup(void)
{
    Serial.println("Setting up I2C");
    Wire.begin(SDA_PIN, SCL_PIN);
}

bool CheckI2C(void)
{
    Wire.beginTransmission(I2C_ADD);
    byte err = Wire.endTransmission();
    if (err == 0)
    {
        Serial.println("I2C device found");
        Wire.beginTransmission(I2C_ADD);
        Wire.write(0x07);
        Wire.write(B10001101);
        Wire.endTransmission();
        Wire.beginTransmission(I2C_ADD);
        Wire.write(0x03);
        Wire.write(B00010000);
        Wire.endTransmission();
        Wire.beginTransmission(I2C_ADD);
        Wire.write(0x00);
        Wire.write(B01111111);
        Wire.endTransmission();
        Wire.beginTransmission(I2C_ADD);
        Wire.write(0x04);
        Wire.write(B00010000);
        Wire.endTransmission();
        Wire.beginTransmission(I2C_ADD);
        Wire.write(0x09);
        Wire.write(B01001000);
        Wire.endTransmission();
        return true;
    }
    Serial.println("I2C device not found");
    return false;
}

void Reset(void)
{
    state = 0;
    pinMode(COUT_DAC_PULLDOWN, OUTPUT);
    pinMode(COUT_DAC_PULLDOWN, LOW);
    dacWrite(VIN_DAC,VIN_5V);
    delay(3000);
    progress = 0;
    tft.fillScreen(TFT_BLACK);
}

float ReadVBus(void)
{
    byte vbus_byte = 0;
    float vbus = 0;
    _adcReady();
    Wire.beginTransmission(I2C_ADD);
    Wire.write(REG_VBUS);
    Wire.endTransmission(false);
    Wire.requestFrom(I2C_ADD, 1);
    while (Wire.available() >= 1)
    {
        vbus_byte = Wire.read();
    }
    vbus = 2.6;
    vbus += bitRead(vbus_byte, 6) * 6.4;
    vbus += bitRead(vbus_byte, 5) * 3.2;
    vbus += bitRead(vbus_byte, 4) * 1.6;
    vbus += bitRead(vbus_byte, 3) * 0.8;
    vbus += bitRead(vbus_byte, 2) * 0.4;
    vbus += bitRead(vbus_byte, 1) * 0.2;
    vbus += bitRead(vbus_byte, 0) * 0.1;
    return vbus;
}

float ReadVBat(void)
{
    byte vbat_byte = 0;
    float vbat;
    _adcReady();
    Wire.beginTransmission(I2C_ADD);
    Wire.write(REG_VBAT);
    Wire.endTransmission(false);
    Wire.requestFrom(I2C_ADD, 1);
    while (Wire.available() >= 1)
    {
        vbat_byte = Wire.read();
    }
    Wire.endTransmission();

    vbat = 2.304;
    vbat += bitRead(vbat_byte, 6) * 1.280;
    vbat += bitRead(vbat_byte, 5) * 0.640;
    vbat += bitRead(vbat_byte, 4) * 0.320;
    vbat += bitRead(vbat_byte, 3) * 0.160;
    vbat += bitRead(vbat_byte, 2) * 0.08;
    vbat += bitRead(vbat_byte, 1) * 0.04;
    vbat += bitRead(vbat_byte, 0) * 0.02;
    return vbat;
}

byte ReadStatus(void)
{
    byte status = 0;
    Wire.beginTransmission(I2C_ADD);
    Wire.write(REG_STATUS);
    Wire.endTransmission(false);
    Wire.requestFrom(I2C_ADD, 1);
    while (Wire.available() >= 1)
    {
        status = Wire.read();
    }
    return status;
}

void _adcReady(void)
{
    Wire.beginTransmission(I2C_ADD);
    Wire.write(REG_CONV_ADC);
    Wire.write(BYTE_CONV_ADC_START);
    Wire.endTransmission();
    delay(1100);
    Wire.beginTransmission(I2C_ADD);
    Wire.write(REG_CONV_ADC);
    Wire.write(BYTE_CONV_ADC_STOP);
    Wire.endTransmission();
}

unsigned int _readAnalog(int pin)
{
    unsigned int raw = 0;
    for (int i = 0; i < 100; i++)
    {
        raw += analogRead(pin);
    }
    raw /= 100;
    return raw;
}

float ReadCIN(void)
{
    unsigned int cin_raw = _readAnalog(CIN_ADC);
    Serial.println((float)cin_raw * (3.3 / 4096));
    float cin = ((cin_raw * (3.3 / 4096.0)) - 2.34) / 0.100;
    return cin;
}

float ReadVOUT(void)
{
    unsigned int vout_raw = _readAnalog(VOUT_ADC);
    Serial.println(vout_raw);
    float vout = 0.001565 * vout_raw + 0.5449;
    return vout;
}

float ReadCOUT(void)
{
    unsigned int cout_raw = _readAnalog(COUT_ADC);
    float cout = (float)cout_raw * (3.3 / 4096.0) * 10;
    return cout;
}


bool TestAndPrintParams(void)
{
    float vin = ReadVBus();
    float cin = ReadCIN();
    float vout = ReadVOUT();
    float cout = ReadCOUT();
    float eff = ((vout * cout) / (vin * cin)) * 100;
    char str[50];
    sprintf(str, "|IN:%.2fV%.2fA|OUT:%.2fV%.2fA|E:%.0f", vin, cin, vout, cout, eff);
    Serial.println(str);
    tft.drawString(str, 0, 15);

    byte status = ReadStatus();
    if (bitRead(status, 2))
    {
        if (!(vin > 4 || vin < 15))
        {
            Serial.println("VIN Error");
            return false;
        }
    }
    if (!(vout > 4.75 && vout < 5.25))
    {
        Serial.println("VOUT Error");
        return false;
    }
    Progress();
    return true;
}

bool TestCOUT(void)
{
    dacWrite(COUT_DAC,COUT_1A);
    delay(100);
    if(!TestAndPrintParams())
    {
        return false;
    }
    dacWrite(COUT_DAC,COUT_2A);
    delay(100);
    if(!TestAndPrintParams())
    {
        return false;
    }
    dacWrite(COUT_DAC,COUT_3A);
    delay(100);
    if(!TestAndPrintParams())
    {
        return false;
    }
    return true;
}

bool TestVIN_COUT(void)
{
    dacWrite(VIN_DAC,VIN_5V);
    delay(100);
    if(!TestCOUT())
    {
        return false;
    }
    dacWrite(VIN_DAC,VIN_9V);
    delay(100);
    if(!TestCOUT())
    {
        return false;
    }
    dacWrite(VIN_DAC,VIN_14V);
    delay(100);
    if(!TestCOUT())
    {
        return false;
    }
    return true;
}

void Progress(void)
{
    progress += 18;
    tft.drawRect(0,0,progress,5,TFT_WHITE);
}


