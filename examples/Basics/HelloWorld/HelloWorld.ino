#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);

void setup() {
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    canvas.createCanvas(400, 300);
    canvas.setTextSize(3);
    canvas.drawString("Hello World", 0, 0);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
}

void loop() {
}
