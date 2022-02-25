/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5Paper sample source code
*                          配套  M5Paper 示例源代码
* Visit the website for more
information：https://docs.m5stack.com/en/app/scales_kit
* 获取更多资料请访问：https://docs.m5stack.com/zh_CN/app/scales_kit
*
* describe: SCALES KIT WEIGHT UNIT EXAMPLE.
* date：2022/02/25
*******************************************************************************
  Libraries:
  - [HX711](https://github.com/bogde/HX711)

*/

#include <M5EPD.h>
#include <M5GFX.h>
#include "HX711.h"

M5GFX display;
M5Canvas canvas(&display);

// HX711 related pin Settings.  HX711 相关引脚设置
#define LOADCELL_DOUT_PIN 33
#define LOADCELL_SCK_PIN  26

HX711 scale;

void setup() {
    M5.begin(true, false, true, true, false); 

    display.begin();

    if (display.isEPD()) {
        display.setEpdMode(epd_mode_t::epd_quality);
        display.invertDisplay(true);
        display.clear(TFT_BLACK);
    }
    if (display.width() < display.height()) {
        display.setRotation(display.getRotation() ^ 1);
    }

    canvas.createSprite(display.width(), display.height());
    canvas.setTextDatum(MC_DATUM);
    canvas.setFont(&fonts::lgfxJapanGothicP_40);
    canvas.drawString("Calibration sensor....", 480, 270);
    canvas.pushSprite(0, 0);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    // The scale value is the adc value corresponding to 1g
    scale.set_scale(27.61f);  // set scale
    scale.tare();             // auto set offset
}

char info[100];

float last_weight = 0;

void loop() {
    float weight = scale.get_units(20) / 1000.0;
    canvas.setTextSize(5);
    if (weight != last_weight) {
        canvas.clear();
        if (weight >= 0) {
            Serial.printf("%.2f", weight);
            sprintf(info, "%.2f", weight);
            canvas.drawString(String(info) + "kg", 480, 270);
        } else {
            canvas.drawString("0.00kg", 480, 270);
        }
        canvas.pushSprite(0, 0);
        last_weight = weight;
    }
    M5.update();
}
