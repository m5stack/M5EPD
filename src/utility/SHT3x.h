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
#ifndef _SHT3x_H_
#define _SHT3x_H_

#include <stddef.h>
#include <stdint.h>

class SHT3x {
   public:
    enum TemperatureScale {
        Cel,
        Fah,
        // aliases
        Celsius    = Cel,
        Fahrenheit = Fah,
    };
    enum AbsHumidityScale {
        Torr,
    };
    enum Error {
        SHT3x_ERROR_OK  = 0,
        SHT3x_ERROR_CRC = 100,
    };
    void Begin();
    uint8_t UpdateData();
    float GetTemperature(TemperatureScale Degree = Cel);
    float GetRelHumidity();
    // Not implemented.
    float GetAbsHumidity(AbsHumidityScale Scale = Torr);

    /**
     * Return the last result of `UpdateData()`
     */
    uint8_t GetError() const {
        return _lastError;
    }

   private:
    uint8_t UpdateDataImpl();
    uint8_t SendMeasurementCommand();
    uint8_t ReceiveResult(uint8_t* buf);
    bool CheckCrc(uint8_t* buf);
    uint8_t Crc8(uint8_t* buf, size_t len);

    uint16_t _rawTemperature = 0;
    uint16_t _rawHumidity    = 0;
    uint8_t _lastError       = 0;
};

#endif /* _SHT3x_H_ */
