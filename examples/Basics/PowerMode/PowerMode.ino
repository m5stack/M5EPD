/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5Paper sample source code
*                          配套 M5Paper 示例源代码
*
* Visit for more information: https://docs.m5stack.com/en/core/m5paper
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/m5paper
*
* Product: M5Paper.
* Date: 2022/6/30
*******************************************************************************
  上推/按下/下推拨轮分别进入活动/独立/休眠状态(功耗分别为160mA,60mA,100mA)
  注意进入独立/休眠模式后需要重新启动paper来恢复到初始状态
  Push up / press / push down the dial to enter the active / independent /
  hibernation state (power consumption of 160mA, 60mA, 100mA respectively) Note
  that after entering standalone/sleep mode, you need to restart the paper to
  return to the initial state.
*/

#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);

void ButtonTest(char* str) {
    canvas.clear();
    canvas.drawString(str, 100, 100);
    canvas.pushCanvas(100, 300, UPDATE_MODE_DU4);
    delay(500);
}

void setup() {
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    canvas.createCanvas(300, 300);
    canvas.setTextSize(4);
    canvas.fontHeight(LOAD_FONT2);
    ButtonTest("Ready");
}

void loop() {
    if (M5.BtnL.wasPressed()) {
        M5.EPD.Active();
        ButtonTest("Active");
    }
    if (M5.BtnP.wasPressed()) {
        ButtonTest("StandBy");
        delay(1000);
        M5.EPD.StandBy();
    }
    if (M5.BtnR.wasPressed()) {
        ButtonTest("Sleep");
        delay(1000);
        M5.EPD.Sleep();
    }
    M5.BtnL.lastChange();
    M5.update();
    delay(100);
}