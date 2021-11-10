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
    int8_t mon; // [1, 12]
    int8_t day;
    int16_t year; // AD
    RTC_Date() : week(), mon(), day(), year() {}
    RTC_Date(int8_t w, int8_t m, int8_t d, int16_t y) : week(w), mon(m), day(d), year(y) {}
} rtc_date_t;

class BM8563
{
public:
    BM8563(TwoWire &wire = Wire) : _wire(wire){};

    void begin(int sda = -1, int scl = -1, uint32_t frequency = 0);
    void writeReg(uint8_t reg, uint8_t data);
    uint8_t readReg(uint8_t reg);

    void setTime(const rtc_time_t *time);
    void setDate(const rtc_date_t *date);
    void setDateTime(const rtc_date_t *date, const rtc_time_t *time);
    void setTime(const rtc_time_t &time)
    {
        setTime(&time);
    }
    void setDate(const rtc_date_t &date)
    {
        setDate(&date);
    }
    void setDateTime(const rtc_date_t &date, const rtc_time_t &time)
    {
        setDateTime(&date, &time);
    }

    void getTime(rtc_time_t *time);
    void getDate(rtc_date_t *date);
    void getDateTime(rtc_date_t *date, rtc_time_t *time);
    void getTime(rtc_time_t &time)
    {
        getTime(&time);
    }
    void getDate(rtc_date_t &date)
    {
        getDate(&date);
    }
    void getDateTime(rtc_date_t &date, rtc_time_t &time)
    {
        getDateTime(&date, &time);
    }

    int setAlarmIRQ(int afterSeconds);
    int setAlarmIRQ(const rtc_time_t &time);
    int setAlarmIRQ(const rtc_date_t &date, const rtc_time_t &time);
    void clearIRQ(void);
    void disableIRQ(void);

private:
    TwoWire &_wire;
    uint8_t Bcd2ToByte(uint8_t Value);
    uint8_t ByteToBcd2(uint8_t Value);
    void readDate(rtc_date_t *date, uint8_t *buf);
    void readTime(rtc_time_t *time, uint8_t *buf);
    void writeDate(const rtc_date_t *date);
    void writeTime(const rtc_time_t *time);

    static constexpr int Addr = 0x51;
    enum Register
    {
        ControlStatus1 = 0x00,
        ControlStatus2 = 0x01,
        Second = 0x02,
        Day = 0x05,
        MinuteAlarm = 0x09,
        ClkOutControl = 0x0D,
        TimerControl = 0x0E,
        Timer = 0x0F
    };

    struct Mask
    {
        static constexpr int Second = 0x7f;
        static constexpr int Minute = 0x7f;
        static constexpr int Hour = 0x3f;
        static constexpr int Day = 0x3f;
        static constexpr int Week = 0x07;
        static constexpr int Month = 0x1f;
        static constexpr int Year = 0xff;
    };
};

#endif
