#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);

void ButtonTest(char* str)
{
    canvas.fillCanvas(0);
    canvas.drawString(str, 100, 100);
    canvas.pushCanvas(100,300,UPDATE_MODE_DU4);
    delay(500);
}

void setup() {

    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    canvas.createCanvas(300, 300);
    canvas.setTextSize(3);
}

void loop() {
    if( M5.BtnL.wasPressed()) ButtonTest("Btn L Pressed");
    if( M5.BtnP.wasPressed()) ButtonTest("Btn P Pressed");
    if( M5.BtnR.wasPressed()) ButtonTest("Btn R Pressed");
    M5.update();
    delay(100);
}