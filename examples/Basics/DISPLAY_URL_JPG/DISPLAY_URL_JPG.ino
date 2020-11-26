#include <M5EPD.h>
#include <WiFi.h>

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    WiFi.begin("WIFI-SSID", "WIFI-PASSWORD");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    canvas.createCanvas(540, 960);
    canvas.setTextSize(3);
    canvas.drawJpgUrl("https://m5stack.oss-cn-shenzhen.aliyuncs.com/image/example_pic/flower.jpg");
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
}

void loop()
{

}
