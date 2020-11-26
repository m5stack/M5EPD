#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.TP.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    canvas.createCanvas(540, 960);
    canvas.setTextSize(3);
    canvas.drawString("Press PWR Btn for sleep!", 45, 350);
    canvas.drawString("after 5 sec wakeup!", 70, 450);
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
    
    
}

void loop()
{
    if(M5.BtnP.wasPressed()){
        
        canvas.drawString("I'm going to sleep.zzzZZZ~", 45, 550);
        canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
        delay(1000);
        M5.shutdown(5);
    }
    M5.update();
    delay(100);
}