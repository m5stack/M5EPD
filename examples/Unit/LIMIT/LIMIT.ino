/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5Paper sample source code
*                          配套  M5Paper 示例源代码
* Visit the website for more information: https://docs.m5stack.com/zh_CN/products
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/products
* 
* Please connect UNIT-LIMIT to PORT.B
* 请将 UNIT-LIMIT 连接到 PORT.B
*
* describe: Limit.
* date: 2022/6/1
*******************************************************************************
*/

#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);

#define KEY_PIN 33  // Define Limit Pin.  定义Limit连接引脚

void setup() {
    M5.begin();  // Init M5Stack  初始化M5Stack
    M5.EPD.SetRotation(90);   //Set the rotation of the display.  设置屏幕旋转角度
    M5.EPD.Clear(true);  //Clear the screen.  清屏
    M5.RTC.begin();  //Init the RTC.  初始化 RTC
    canvas.createCanvas(540, 960);  //Create a canvas.  创建画布
    canvas.setTextSize(3); //Set the text size.  设置文字大小
    canvas.drawString("UNIT-LIMIT Example", 110, 350);  //Draw a string.  绘制字符串
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);  //Update the screen.  更新屏幕

    Serial.begin(115200);

    pinMode(KEY_PIN, INPUT_PULLUP);  // Init Limit pin.  初始化Limit引脚.
}

void loop() {
    if (!digitalRead(KEY_PIN)) {  // If Limit was hit.  如果触碰了Limit.
        Serial.println("Hit limit!");
    }
    delay(100);
}
