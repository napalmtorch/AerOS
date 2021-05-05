#pragma once
#include "lib/types.h"
#include "lib/string.hpp"
#include "hardware/ports.hpp"

namespace HAL
{
    // used to manage real time clock data from CMOS
    class RTCManager
    {
        public:
            // initialize real time clock
            void Initialize();
            
            // read clock data from CMOS
            void Read();

            // get second value
            uint8_t GetSecond();

            // get minute value
            uint8_t GetMinute();

            // get hour value
            uint8_t GetHour();

            // get day value
            uint8_t GetDay();

            // get month value
            uint8_t GetMonth();

            // get year value
            uint8_t GetYear();

            // get time as string
            char* GetTimeString();

            // get date as string
            char* GetDateString();

            // get both date and time as string
            char* GetDateTimeString();
        private:
            // date and time value strings
            char TimeString[12];
            char DateString[12];
            char DateTimeString[48];
            
            // time values
            uint8_t second  = 0;
            uint8_t minute  = 0;
            uint8_t hour    = 0;
            uint8_t day     = 0;
            uint8_t month   = 0;
            uint8_t year    = 0;

            // ports
            IOPort PortCMD;
            IOPort PortData;

            // manage registers
            void SetRegister(uint16_t reg, uint8_t data);
            uint8_t GetRegister(uint16_t reg);

            // check if real time clock is updating
            bool IsUpdating();
    };
}