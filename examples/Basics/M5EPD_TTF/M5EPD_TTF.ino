//The Fonts file (M5EPD_TTF) used in the example is in the same path as the program, please install Fonts file in TF card or SPIFFS before compiling.

#include "M5EPD.h"
#include "FS.h"
#include "SPIFFS.h"
#include "binaryttf.h"

//Example for TTF font
#define USE_SPIFFS

M5EPD_Canvas canvas1(&M5.EPD);

void setup()
{
    M5.begin();
    M5.TP.SetRotation(90);
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);

    if (!SPIFFS.begin(true))
    {
        log_e("SPIFFS Mount Failed");
        while(1);
    }

#ifdef USE_SPIFFS
    canvas1.loadFont("/GenSenRounded-R.ttf", SPIFFS); // Load font files from SPIFFS
#elif USE_SD
    canvas1.loadFont("/GenSenRounded-R.ttf", SD); // Load font files from SD Card
#else
    canvas1.loadFont(binaryttf, sizeof(binaryttf)); // Load font files from binary data
#endif

    canvas1.createCanvas(540, 960);
    canvas1.createRender(48, 256);
    canvas1.setTextSize(48);
    canvas1.setTextColor(0);
    canvas1.setTextDatum(TC_DATUM);
    canvas1.fillRect(0, 0, 540, 200, 15);
    canvas1.drawString("TTF font rendering", 270, 20);
    canvas1.drawString("TTF レンダリング", 270, 80);
    canvas1.drawString("TTF 字体渲染", 270, 140);

    canvas1.setTextColor(15);
    canvas1.setTextDatum(BL_DATUM);
    uint16_t x = 15;
    for(int i = 8; i < 72; i += 8)
    {
        if(i != 48)
        {
            canvas1.createRender(i);
        }
        
        canvas1.setTextSize(i);
        canvas1.drawString("✿", x, 300);
        x += i;
    }

    for(int i = 72 - 16; i >= 8; i -= 8)
    {
        canvas1.setTextSize(i);
        canvas1.drawString("✿", x, 300);
        x += i;

        if(i != 48)
        {
            canvas1.destoryRender(i);
        }
    }

    canvas1.createRender(32, 256);
    canvas1.setTextSize(32);
    canvas1.setTextDatum(TC_DATUM);
    canvas1.drawString("Flexible font size", 270, 310);
    canvas1.drawString("柔軟なフォントサイズ", 270, 355);
    canvas1.drawString("灵活的字体大小", 270, 400);
    
    canvas1.setTextSize(48);
    canvas1.setTextColor(15);
    canvas1.setTextDatum(TC_DATUM);
    canvas1.drawString("❖★∑❀Ⅻゑǜψ※Ж≌╋", 10, 480);
    canvas1.setTextSize(32);
    canvas1.setTextDatum(TC_DATUM);
    canvas1.drawString("Perfect Unicode support", 270, 550);
    canvas1.drawString("完善的Unicode支持", 270, 595);
    canvas1.drawString("完全なUnicodeサポート", 270, 640);

    const uint16_t kDatumTy = 720;
    const uint16_t kDatumBy = 960;
    const uint16_t kDatumCy = kDatumTy + ((kDatumBy - kDatumTy) / 2);
    canvas1.setTextSize(48);
    canvas1.setTextColor(15);
    canvas1.setTextDatum(TL_DATUM);
    canvas1.drawString("TL←", 0, kDatumTy);
    canvas1.setTextDatum(TC_DATUM);
    canvas1.drawString("→TC←", 270, kDatumTy);
    canvas1.setTextDatum(TR_DATUM);
    canvas1.drawString("→TR", 540, kDatumTy);
    canvas1.setTextDatum(CL_DATUM);
    canvas1.drawString("CL←", 0, kDatumCy);
    canvas1.setTextDatum(CC_DATUM);
    canvas1.drawString("→CC←", 270, kDatumCy);
    canvas1.setTextDatum(CR_DATUM);
    canvas1.drawString("→CR", 540, kDatumCy);
    canvas1.setTextDatum(BL_DATUM);
    canvas1.drawString("BL←", 0, kDatumBy);
    canvas1.setTextDatum(BC_DATUM);
    canvas1.drawString("→BC←", 270, kDatumBy);
    canvas1.setTextDatum(BR_DATUM);
    canvas1.drawString("→BR", 540, kDatumBy);

    canvas1.pushCanvas(0, 0, UPDATE_MODE_GC16);
}

void loop()
{
}