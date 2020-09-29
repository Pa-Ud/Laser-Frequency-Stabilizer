//Author Dr Matthew Harvey, United Kingdoms, 2018
#ifndef _AD5662_BITBANG_H
//_dacAD5662_H
#define _AD5662_BITBANG_H

#include <Arduino.h>

// Bit banged class to send updates to AD5662. 
// Pins:
// 1 Vdd   5V
// 2 Vref
// 3 Vfb
// 4 Vout
// 5 !SYNC (equivalent to !CS in mcp4822)
// 6 SCLK
// 7 DIN
// 8 GND

class AD5662
{
    private:
        // Private functions and variables here.  They can only be accessed
        // by functions within the class.
        int _sync;
        int _sclk;
        int _din;
        //uint8_t _Byte1;
        //uint8_t _Byte2;
        //uint8_t _Byte3;

        void pulseClk();
        void sendByte(uint8_t dataByte);

    public:
        // Public functions and variables.  These can be accessed from
        // outside the class.
        AD5662(int sync, int sclk, int din); 
    
        void init();
        void setVoltage(unsigned int voltage);
        //void setAllVoltages(unsigned int voltage);
};
#endif
