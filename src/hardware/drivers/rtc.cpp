#include "hardware/drivers/rtc.hpp"

namespace HAL
{
    uint8_t time, last_time;

    // initialize real time clock
    void RTCManager::Initialize()
    {
        PortCMD = IOPort(0x70);
        PortData = IOPort(0x71);
    }

    // read clock data from CMOS
    void RTCManager::Read()
    {
        // only read data if cmos is not updating
        if (!IsUpdating())
        {
            // fetch time
            second = GetRegister(0x00);
            minute = GetRegister(0x02);
            hour   = GetRegister(0x04);

            // fetch date
            day    = GetRegister(0x07);
            month  = GetRegister(0x08);
            year   = GetRegister(0x09);

            // check if time is binary coded decimal
            uint8_t bcd = GetRegister(0x0B);

            // convert from binary coded decimal if required
            if (!(bcd & 0x04))
            {
                second  = (second & 0x0F) + (second / 16) * 10;
                minute  = (minute & 0x0F) + (minute / 16) * 10;
                hour    = ((hour & 0x0F) + (hour / 16) * 10) | (hour & 0x80);
                day     = (day & 0x0F) + (day / 16) * 10;
                month   = (month & 0x0F) + (month / 16) * 10;
                year    = (year & 0x0F) + (year / 16) * 10;
            }

            // update time string
            TimeString[0] = '\0';
            char hr[4];
            strdec(hour, hr);
            if (hour < 10) { stradd(TimeString, '0'); }
            strcat(TimeString, hr);
            strcat(TimeString, ":");
            char min[4];
            strdec(minute, min);
            if (minute < 10) { stradd(TimeString, '0');}
            strcat(TimeString, min);
            strcat(TimeString, ":");
            char sec[4];
            strdec(second, sec);
            if (second < 10) { stradd(TimeString, '0'); }
            strcat(TimeString, sec);
            last_time = time;
        }
    }

    // manage registers
    void RTCManager::SetRegister(uint16_t reg, uint8_t data)
    {
        if (!IsUpdating())
        {
            PortCMD.WriteInt8(reg);
            PortData.WriteInt8(data);
        }
    }

    uint8_t RTCManager::GetRegister(uint16_t reg)
    {
        PortCMD.WriteInt8(reg);
        return PortData.ReadInt8();
    }

    // check if real time clock is updating
    bool RTCManager::IsUpdating()
    {
        PortCMD.WriteInt8(0x0A);
        uint32_t status = PortData.ReadInt8();
        return (status & 0x80);
    }

    // get second value
    uint8_t RTCManager::GetSecond() { return second; }

    // get minute value
    uint8_t RTCManager::GetMinute() { return minute; }

    // get hour value
    uint8_t RTCManager::GetHour() { return hour; }

    // get day value
    uint8_t RTCManager::GetDay() { return day; }

    // get month value
    uint8_t RTCManager::GetMonth() { return month; }

    // get year value
    uint8_t RTCManager::GetYear() { return year; }

    // get time as string
    char* RTCManager::GetTimeString() 
    {
        return TimeString; 
    }

    // get date as string
    char* RTCManager::GetDateString()
    {
        /* TODO: not yet implemented */ 
        DateString[0] = '\0';
        return DateString; 
    }

    // get both date and time as string
    char* RTCManager::GetDateTimeString()
    {
        /* TODO: not yet implemented */ 
        DateTimeString[0] = '\0';
        return DateTimeString; 
    }
}