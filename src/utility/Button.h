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
#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <stdint.h>

class Button
{
public:
    Button(uint8_t pin, uint8_t invert, uint32_t dbTime);
    uint8_t read();
    uint8_t isPressed();
    uint8_t isReleased();
    uint8_t wasPressed();
    uint8_t wasReleased();
    uint8_t pressedFor(uint32_t ms);
    uint8_t releasedFor(uint32_t ms);
    uint8_t wasReleasefor(uint32_t ms);
    uint32_t lastChange();
};
#endif /* __BUTTON_H__ */
