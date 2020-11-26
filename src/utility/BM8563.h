#ifndef __BM8563_H__
#define __BM8563_H__

#include <Wire.h>

typedef struct RTC_Time
{
    int8_t hour;
    int8_t min;
    int8_t sec;
    RTC_Time() : hour(), min(), sec() {}
    RTC_Time(int8_t h, int8_t m, int8_t s) : hour(h), min(m), sec(s) {}
} rtc_time_t;

typedef struct RTC_Date
{
    int8_t week;
    int8_t mon;
    int8_t day;
    int16_t year;
    RTC_Date() : week(), mon(), day(), year() {}
    RTC_Date(int8_t w, int8_t m, int8_t d, int16_t y) : week(w), mon(m), day(d), year(y) {}
} rtc_date_t;

class BM8563
{
public:
    BM8563();

    void begin(void);
    void writeReg(uint8_t reg, uint8_t data);
    uint8_t readReg(uint8_t reg);
    void getTime(void);

    void setTime(rtc_time_t *time);
    void setDate(rtc_date_t *date);

    void getTime(rtc_time_t *time);
    void getDate(rtc_date_t *date);

    int setAlarmIRQ(int afterSeconds);
    int setAlarmIRQ(const rtc_time_t &time);
    int setAlarmIRQ(const rtc_date_t &date, const rtc_time_t &time);
    void clearIRQ();
    void disableIRQ();

private:
    void DataMask();
    uint8_t Bcd2ToByte(uint8_t Value);
    uint8_t ByteToBcd2(uint8_t Value);

private:
    uint8_t trdata[7];
};

#endif
