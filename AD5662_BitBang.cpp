//Author Dr Matthew Harvey, United Kingdoms, 2018
#include "AD5662_BitBang.h"

AD5662::AD5662(int sync, int sclk, int din)
{ 
    _sync = sync;
    _sclk = sclk;
    _din = din;
}   

void AD5662::init()
{
    pinMode(_sync, OUTPUT);
    pinMode(_sclk, OUTPUT);
    pinMode(_din, OUTPUT);
    
    digitalWrite(_sync, HIGH);
    digitalWrite(_sclk, HIGH);
    digitalWrite(_din, HIGH);   
}

void AD5662::setVoltage(unsigned int voltage)
{
    // <x><x><x><x><x><x><PD1><PD0><D15><D14><D13><D12><D11><D10><D9><D8><D7><D6><D5><D4><D3><D2><D1><D0>
    uint8_t byte1 = 0b00000000;
    uint8_t byte2 = (voltage >> 8) & 0b11111111;
    uint8_t byte3 = voltage & 0b11111111;
    digitalWrite(_sync, LOW);
    sendByte(byte1);
    sendByte(byte2);
    sendByte(byte3);
    digitalWrite(_sync, HIGH);
}

void AD5662::sendByte(uint8_t dataByte)
{
    for(int i = 7; i > -1; i--)
    {
        digitalWrite(_din, ((dataByte >> i) & B00000001));
        pulseClk();
    }
}

void AD5662::pulseClk()
{
    digitalWrite(_sclk, LOW);
    digitalWrite(_sclk, HIGH);
}
