#include "BM8563.h"

BM8563::BM8563()
{
}

void BM8563::begin(void)
{
    Wire.begin(21, 22);
    writeReg(0x00, 0x00);
    writeReg(0x01, 0x00);
    writeReg(0x0D, 0x00);
}

void BM8563::writeReg(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(0x51);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

uint8_t BM8563::readReg(uint8_t reg)
{
    Wire.beginTransmission(0x51);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(0x51, 1);
    return Wire.read();
}

void BM8563::getTime(void)
{
    Wire.beginTransmission(0x51);
    Wire.write(0x02);
    Wire.endTransmission();
    Wire.requestFrom(0x51, 7);
    while (Wire.available())
    {

        trdata[0] = Wire.read();
        trdata[1] = Wire.read();
        trdata[2] = Wire.read();
        trdata[3] = Wire.read();
        trdata[4] = Wire.read();
        trdata[5] = Wire.read();
        trdata[6] = Wire.read();
    }

    DataMask();
}

void BM8563::DataMask()
{
    trdata[0] = trdata[0] & 0x7f;
    trdata[1] = trdata[1] & 0x7f;
    trdata[2] = trdata[2] & 0x3f;
    trdata[3] = trdata[3] & 0x3f;
    trdata[4] = trdata[4] & 0x07;
    trdata[5] = trdata[5] & 0x1f;
    trdata[6] = trdata[6] & 0xff;
}

uint8_t BM8563::Bcd2ToByte(uint8_t Value)
{
    uint8_t tmp = 0;
    tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
    return (tmp + (Value & (uint8_t)0x0F));
}

uint8_t BM8563::ByteToBcd2(uint8_t Value)
{
    uint8_t bcdhigh = 0;

    while (Value >= 10)
    {
        bcdhigh++;
        Value -= 10;
    }

    return ((uint8_t)(bcdhigh << 4) | Value);
}

void BM8563::getTime(rtc_time_t *time)
{
    uint8_t buf[3] = {0};

    Wire.beginTransmission(0x51);
    Wire.write(0x02);
    Wire.endTransmission();
    Wire.requestFrom(0x51, 3);

    while (Wire.available())
    {

        buf[0] = Wire.read();
        buf[1] = Wire.read();
        buf[2] = Wire.read();
    }

    time->sec = Bcd2ToByte(buf[0] & 0x7f);
    time->min = Bcd2ToByte(buf[1] & 0x7f);
    time->hour = Bcd2ToByte(buf[2] & 0x3f);
}

void BM8563::setTime(rtc_time_t *time)
{

    if (time == NULL)
        return;

    Wire.beginTransmission(0x51);
    Wire.write(0x02);
    Wire.write(ByteToBcd2(time->sec));
    Wire.write(ByteToBcd2(time->min));
    Wire.write(ByteToBcd2(time->hour));
    Wire.endTransmission();
}

void BM8563::getDate(rtc_date_t *date)
{

    uint8_t buf[4] = {0};

    Wire.beginTransmission(0x51);
    Wire.write(0x05);
    Wire.endTransmission();
    Wire.requestFrom(0x51, 4);

    while (Wire.available())
    {

        buf[0] = Wire.read();
        buf[1] = Wire.read();
        buf[2] = Wire.read();
        buf[3] = Wire.read();
    }

    date->day = Bcd2ToByte(buf[0] & 0x3f);
    date->week = Bcd2ToByte(buf[1] & 0x07);
    date->mon = Bcd2ToByte(buf[2] & 0x1f);

    if (buf[2] & 0x80)
    {
        date->year = 1900 + Bcd2ToByte(buf[3] & 0xff);
    }
    else
    {
        date->year = 2000 + Bcd2ToByte(buf[3] & 0xff);
    }
}

void BM8563::setDate(rtc_date_t *date)
{

    if (date == NULL)
        return;
    Wire.beginTransmission(0x51);
    Wire.write(0x05);
    Wire.write(ByteToBcd2(date->day));
    Wire.write(ByteToBcd2(date->week));

    if (date->year < 2000)
    {

        Wire.write(ByteToBcd2(date->mon) | 0x80);
        Wire.write(ByteToBcd2((uint8_t)(date->year % 100)));
    }
    else
    {
        /* code */
        Wire.write(ByteToBcd2(date->mon) | 0x00);
        Wire.write(ByteToBcd2((uint8_t)(date->year % 100)));
    }

    Wire.endTransmission();
}

int BM8563::setAlarmIRQ(int afterSeconds)
{
    uint8_t reg_value = 0;
    reg_value = readReg(0x01);

    if (afterSeconds < 0)
    {
        reg_value &= ~(1 << 0);
        writeReg(0x01, reg_value);
        reg_value = 0x03;
        writeReg(0x0E, reg_value);
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
    writeReg(0x0F, afterSeconds);
    writeReg(0x0E, type_value);

    reg_value |= (1 << 0);
    reg_value &= ~(1 << 7);
    writeReg(0x01, reg_value);
    return afterSeconds * div;
}

int BM8563::setAlarmIRQ(const rtc_time_t &time)
{
    uint8_t irq_enable = false;
    uint8_t out_buf[4] = {0x80, 0x80, 0x80, 0x80};

    if (time.min >= 0)
    {
        irq_enable = true;
        out_buf[0] = ByteToBcd2(time.min) & 0x7f;
    }

    if (time.hour >= 0)
    {
        irq_enable = true;
        out_buf[1] = ByteToBcd2(time.hour) & 0x3f;
    }

    out_buf[2] = 0x00;
    out_buf[3] = 0x00;

    uint8_t reg_value = readReg(0x01);

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
        writeReg(0x09 + i, out_buf[i]);
    }
    writeReg(0x01, reg_value);

    return irq_enable ? 1 : 0;
}

int BM8563::setAlarmIRQ(const rtc_date_t &date, const rtc_time_t &time)
{
    uint8_t irq_enable = false;
    uint8_t out_buf[4] = {0x80, 0x80, 0x80, 0x80};

    if (time.min >= 0)
    {
        irq_enable = true;
        out_buf[0] = ByteToBcd2(time.min) & 0x7f;
    }

    if (time.hour >= 0)
    {
        irq_enable = true;
        out_buf[1] = ByteToBcd2(time.hour) & 0x3f;
    }

    if (date.day >= 0)
    {
        irq_enable = true;
        out_buf[2] = ByteToBcd2(date.day) & 0x3f;
    }

    if (date.week >= 0)
    {
        irq_enable = true;
        out_buf[3] = ByteToBcd2(date.week) & 0x07;
    }

    uint8_t reg_value = readReg(0x01);

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
        writeReg(0x09 + i, out_buf[i]);
    }
    writeReg(0x01, reg_value);

    return irq_enable ? 1 : 0;
}

void BM8563::clearIRQ()
{
    uint8_t data = readReg(0x01);
    writeReg(0x01, data & 0xf3);
}
void BM8563::disableIRQ()
{
    clearIRQ();
    uint8_t data = readReg(0x01);
    writeReg(0x01, data & 0xfC);
}
