#include <M5EPD.h>

char temStr[10];
char humStr[10];

float tem;
float hum;

M5EPD_Canvas canvas(&M5.EPD);
void setup()
{
    M5.begin();
    M5.SHT30.Begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    canvas.createCanvas(400, 300);
    canvas.setTextSize(2);
}

void loop()
{
    
    M5.SHT30.UpdateData();
    tem = M5.SHT30.GetTemperature();
    hum = M5.SHT30.GetRelHumidity();
    Serial.printf("Temperatura: %2.2f*C  Humedad: %0.2f%%\r\n", tem, hum);
    dtostrf(tem, 2, 2 , temStr);
    dtostrf(hum, 2, 2 , humStr);
    canvas.drawString("Temperatura:" + String(temStr) + "*C", 100, 100);
    canvas.drawString("Humedad:" + String(humStr) , 100, 200);
    canvas.pushCanvas(0,300,UPDATE_MODE_A2);
    delay(1000);
}
