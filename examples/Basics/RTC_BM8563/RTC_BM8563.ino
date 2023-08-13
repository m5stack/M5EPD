#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);

rtc_time_t RTCtime;
rtc_date_t RTCDate;

char timeStrbuff[64];

void flushTime() {
    M5.RTC.getTime(&RTCtime);
    M5.RTC.getDate(&RTCDate);

    sprintf(timeStrbuff, "%d/%02d/%02d %02d:%02d:%02d", RTCDate.year,
            RTCDate.mon, RTCDate.day, RTCtime.hour, RTCtime.min, RTCtime.sec);

    canvas.drawString(timeStrbuff, 0, 0);
    canvas.pushCanvas(100, 200, UPDATE_MODE_DU4);
}

void setupTime() {
    RTCtime.hour = 23;
    RTCtime.min  = 33;
    RTCtime.sec  = 33;
    M5.RTC.setTime(&RTCtime);

    RTCDate.year = 2020;
    RTCDate.mon  = 11;
    RTCDate.day  = 27;
    M5.RTC.setDate(&RTCDate);
}

void setup() {
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    M5.RTC.begin();
    canvas.createCanvas(400, 300);
    canvas.setTextSize(3);
    setupTime();
}

void loop() {
    flushTime();
    delay(1000);
}