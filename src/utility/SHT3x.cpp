/*
 * The MIT License
 *
 * Copyright (c) IKEDA Yasuyuki
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "SHT3x.h"
#include <Wire.h>

// Hack - add missing defines
#define I2C_ERROR_OK      (0)
#define I2C_ERROR_TIMEOUT (3)

namespace {
const uint16_t I2C_ADDRESS = 0x44;

// Repatability: High, Clock stretching: Enabled
const uint8_t MEASUREMENT_MSB = 0x2c;
const uint8_t MEASUREMENT_LSB = 0x06;

const long TIMEOUT_MSEC = 50;
const size_t BUF_SIZE   = 6;
}  // namespace

void SHT3x::Begin() {
    // Use the default SDA=21 / SCL=22
    Wire.begin();
}

uint8_t SHT3x::UpdateData() {
    _lastError = UpdateDataImpl();
    return _lastError;
}

uint8_t SHT3x::UpdateDataImpl() {
    Wire.flush();
    {
        uint8_t error = SendMeasurementCommand();
        if (error != 0) {
            return error;
        }
    }

    uint8_t buf[BUF_SIZE];
    {
        uint8_t error = ReceiveResult(buf);
        if (error != 0) {
            return error;
        }
    }

    if (!CheckCrc(buf)) {
        return SHT3x_ERROR_CRC;
    }

    _rawTemperature = buf[0] << 8 | buf[1];
    _rawHumidity    = buf[3] << 8 | buf[4];

    return SHT3x_ERROR_OK;
}

uint8_t SHT3x::SendMeasurementCommand() {
    Wire.beginTransmission(I2C_ADDRESS);

    uint8_t err;

    err = Wire.write(MEASUREMENT_MSB);
    if (err != 1) {
        return err;
    }
    err = Wire.write(MEASUREMENT_LSB);
    if (err != 1) {
        return err;
    }

    return Wire.endTransmission();
}

uint8_t SHT3x::ReceiveResult(uint8_t* buf) {
    Wire.requestFrom(I2C_ADDRESS, BUF_SIZE);
    Wire.setTimeout(TIMEOUT_MSEC);
    if (Wire.readBytes(buf, BUF_SIZE) != BUF_SIZE) {
        return 3;
    }
    return 0;
}

bool SHT3x::CheckCrc(uint8_t* buf) {
    return (Crc8(buf, 2) == buf[2] && Crc8(buf + 3, 2) == buf[5]);
}

uint8_t SHT3x::Crc8(uint8_t* buf, size_t len) {
    uint8_t crc = 0xff;  // initialization
    while (len-- > 0) {
        crc ^= *buf++;
        for (int i = 0; i < 8; ++i) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31  // polynominal
                               : (crc << 1);
        }
    }
    return crc;
}

float SHT3x::GetTemperature(TemperatureScale Degree) {
    switch (Degree) {
        case Cel: {
            return static_cast<float>(_rawTemperature) * 175.0f / 65535.0f -
                   45.0f;
        }
        case Fah: {
            return static_cast<float>(_rawTemperature) * 315.0f / 65535.0f -
                   49.0f;
        }
    }
    return 0.0f;
}

float SHT3x::GetRelHumidity() {
    return static_cast<float>(_rawHumidity) * 100.0f / 65535.0f;
}

float SHT3x::GetAbsHumidity(AbsHumidityScale Scale) {
    // Not implemented.
    return 0.0f;
}
