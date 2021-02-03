#include "BM8563.h"

void BM8563::begin(int sda, int scl, uint32_t frequency)
{
    _wire.begin(sda, scl, frequency);
    writeReg(Register::ControlStatus1, 0x00);
    writeReg(Register::ControlStatus2, 0x00);
    writeReg(Register::ClkoutControl, 0x00);
}

void BM8563::writeReg(uint8_t reg, uint8_t data)
{
    _wire.beginTransmission(Addr);
    _wire.write(reg);
    _wire.write(data);
    _wire.endTransmission();
}

uint8_t BM8563::readReg(uint8_t reg)
{
    _wire.beginTransmission(Addr);
    _wire.write(reg);
    _wire.endTransmission();
    _wire.requestFrom(Addr, 1);
    return _wire.read();
}

uint8_t BM8563::Bcd2ToByte(uint8_t Value)
{
    uint8_t tmp = ((Value & 0xF0) >> 0x4) * 10;
    return tmp + (Value & 0x0F);
}

uint8_t BM8563::ByteToBcd2(uint8_t Value)
{
    uint8_t bcdhigh = 0;

    while (Value >= 10)
    {
        bcdhigh++;
        Value -= 10;
    }

    return (bcdhigh << 4) | Value;
}

void BM8563::getTime(rtc_time_t *time)
{
    uint8_t buf[3] = {0};

    _wire.beginTransmission(Addr);
    _wire.write(Register::Second);
    _wire.endTransmission();
    _wire.requestFrom(Addr, 3);

    while (_wire.available())
    {
        buf[0] = _wire.read();
        buf[1] = _wire.read();
        buf[2] = _wire.read();
    }

    readTime(time, buf);
}

void BM8563::setTime(const rtc_time_t *time)
{
    if (time == nullptr)
        return;
    _wire.beginTransmission(Addr);
    _wire.write(Register::Second);
    writeTime(time);
    _wire.endTransmission();
}

void BM8563::getDate(rtc_date_t *date)
{

    uint8_t buf[4] = {0};

    _wire.beginTransmission(Addr);
    _wire.write(Register::Day);
    _wire.endTransmission();
    _wire.requestFrom(Addr, 4);

    while (_wire.available())
    {
        buf[0] = _wire.read();
        buf[1] = _wire.read();
        buf[2] = _wire.read();
        buf[3] = _wire.read();
    }

    readDate(date, buf);
}

void BM8563::setDate(const rtc_date_t *date)
{
    if (date == nullptr)
        return;
    _wire.beginTransmission(Addr);
    _wire.write(Register::Day);
    writeDate(date);
    _wire.endTransmission();
}

void BM8563::getDateTime(rtc_date_t *date, rtc_time_t *time)
{
    uint8_t buf[7] = {0};

    _wire.beginTransmission(Addr);
    _wire.write(Register::Second);
    _wire.endTransmission();
    _wire.requestFrom(Addr, 7);
    while (_wire.available())
    {
        buf[0] = _wire.read();
        buf[1] = _wire.read();
        buf[2] = _wire.read();
        buf[3] = _wire.read();
        buf[4] = _wire.read();
        buf[5] = _wire.read();
        buf[6] = _wire.read();
    }

    readTime(time, buf);
    readDate(date, &buf[3]);
}

void BM8563::setDateTime(const rtc_date_t *date, const rtc_time_t *time)
{
    if (date == nullptr || time == nullptr)
        return;
    _wire.beginTransmission(Addr);
    _wire.write(Register::Second);
    writeTime(time);
    writeDate(date);
    _wire.endTransmission();
}

int BM8563::setAlarmIRQ(int afterSeconds)
{
    uint8_t reg_value = 0;
    reg_value = readReg(Register::ControlStatus2);

    if (afterSeconds < 0)
    {
        reg_value &= ~1;
        writeReg(Register::ControlStatus2, reg_value);
        reg_value = 0x03;
        writeReg(Register::TimerControl, reg_value);
        return -1;
    }

    uint8_t type_value = 2;
    uint8_t div = 1;
    if (afterSeconds > 255)
    {
        div = 60;
        type_value = 0x83;
    }
    else
    {
        type_value = 0x82;
    }

    afterSeconds = (afterSeconds / div) & 0xFF;
    writeReg(Register::Timer, afterSeconds);
    writeReg(Register::TimerControl, type_value);

    reg_value |= (1 << 0);
    reg_value &= ~(1 << 7);
    writeReg(Register::ControlStatus2, reg_value);
    return afterSeconds * div;
}

int BM8563::setAlarmIRQ(const rtc_time_t &time)
{
    bool irq_enable = false;
    uint8_t out_buf[4] = {0x80, 0x80, 0x80, 0x80}; //alarm disabled

    if (time.min >= 0)
    {
        irq_enable = true;
        out_buf[0] = ByteToBcd2(time.min) & Mask::Minute;
    }

    if (time.hour >= 0)
    {
        irq_enable = true;
        out_buf[1] = ByteToBcd2(time.hour) & Mask::Hour;
    }

    uint8_t reg_value = readReg(Register::ControlStatus2);

    if (irq_enable)
    {
        reg_value |= (1 << 1);
    }
    else
    {
        reg_value &= ~(1 << 1);
    }

    for (int i = 0; i < 4; i++)
    {
        writeReg(Register::MinuteAlarm + i, out_buf[i]);
    }
    writeReg(Register::ControlStatus2, reg_value);

    return irq_enable ? 1 : 0;
}

int BM8563::setAlarmIRQ(const rtc_date_t &date, const rtc_time_t &time)
{
    bool irq_enable = false;
    uint8_t out_buf[4] = {0x80, 0x80, 0x80, 0x80}; //alarm disabled

    if (time.min >= 0)
    {
        irq_enable = true;
        out_buf[0] = ByteToBcd2(time.min) & Mask::Minute;
    }

    if (time.hour >= 0)
    {
        irq_enable = true;
        out_buf[1] = ByteToBcd2(time.hour) & Mask::Hour;
    }

    if (date.day >= 0)
    {
        irq_enable = true;
        out_buf[2] = ByteToBcd2(date.day) & Mask::Day;
    }

    if (date.week >= 0)
    {
        irq_enable = true;
        out_buf[3] = ByteToBcd2(date.week) & Mask::Week;
    }

    uint8_t reg_value = readReg(Register::ControlStatus2);

    if (irq_enable)
    {
        reg_value |= (1 << 1);
    }
    else
    {
        reg_value &= ~(1 << 1);
    }

    for (int i = 0; i < 4; i++)
    {
        writeReg(Register::MinuteAlarm + i, out_buf[i]);
    }
    writeReg(Register::ControlStatus2, reg_value);

    return irq_enable ? 1 : 0;
}

void BM8563::clearIRQ(void)
{
    writeReg(Register::ControlStatus2, readReg(Register::ControlStatus2) & 0xf3);
}

void BM8563::disableIRQ(void)
{
    clearIRQ();
    writeReg(Register::ControlStatus2, readReg(Register::ControlStatus2) & 0xfC);
}

void BM8563::readDate(rtc_date_t *date, uint8_t *buf)
{
    date->day = Bcd2ToByte(buf[0] & Mask::Day);
    date->week = Bcd2ToByte(buf[1] & Mask::Week);
    date->mon = Bcd2ToByte(buf[2] & Mask::Month);

    date->year = buf[2] & 0x80 ? 1900 : 2000; // century
    date->year += Bcd2ToByte(buf[3]);
}

void BM8563::readTime(rtc_time_t *time, uint8_t *buf)
{
    time->sec = Bcd2ToByte(buf[0] & Mask::Second);
    time->min = Bcd2ToByte(buf[1] & Mask::Minute);
    time->hour = Bcd2ToByte(buf[2] & Mask::Hour);
}

void BM8563::writeDate(const rtc_date_t *date)
{
    _wire.write(ByteToBcd2(date->day));
    _wire.write(ByteToBcd2(date->week));

    if (date->year < 2000)
    {
        _wire.write(ByteToBcd2(date->mon) | 0x80);
    }
    else
    {
        _wire.write(ByteToBcd2(date->mon));
    }
    _wire.write(ByteToBcd2(date->year % 100));
}

void BM8563::writeTime(const rtc_time_t *time)
{
    _wire.write(ByteToBcd2(time->sec));
    _wire.write(ByteToBcd2(time->min));
    _wire.write(ByteToBcd2(time->hour));
}
