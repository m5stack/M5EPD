#ifndef _M5EPD_CANVAS_H_
#define _M5EPD_CANVAS_H_

#include <Arduino.h>
#include <FS.h>
#include "utility/In_eSPI.h"
#include "utility/Sprite.h"
#include "utility/qrcode.h"
#include "M5EPD_Driver.h"

typedef enum {
        JPEG_DIV_NONE,
        JPEG_DIV_2,
        JPEG_DIV_4,
        JPEG_DIV_8,
        JPEG_DIV_MAX
    } jpeg_div_t;

class M5EPD_Canvas : private TFT_eSprite
{
public:
    typedef enum
    {
        G0 = 0,G1,G2,G3,G4,G5,G6,G7,G8,G9,G10,G11,G12,G13,G14,G15
    } grayscale_t;

public:
    M5EPD_Canvas(M5EPD_Driver *driver);
    M5EPD_Canvas();
    ~M5EPD_Canvas();
    void setDriver(M5EPD_Driver *driver);
    void qrcode(const char *string, uint16_t x = 50, uint16_t y = 10, uint16_t width = 220, uint8_t version = 6);
    void qrcode(const String &string, uint16_t x = 50, uint16_t y = 10, uint16_t width = 220, uint8_t version = 6);
    void fillCenterSquare(int32_t cx, int32_t cy, uint16_t w, uint8_t color);
    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t thickness, uint8_t color);
    void drawFastVLine(int32_t x, int32_t y, int32_t h, uint8_t thickness, uint8_t color);
    void drawFastHLine(int32_t x, int32_t y, int32_t w, uint8_t thickness, uint8_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    uint32_t getBufferSize(void) {return _buffer_size;}
    void ReverseColor(void);
    void ReversePartColor(int32_t x, int32_t y, int32_t w, int32_t h);
    void operator=(const M5EPD_Canvas &src);
    
    bool drawBmpFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y);
    bool drawBmpFile(fs::FS &fs, String path, uint16_t x, uint16_t y);

    bool drawJpgFile(fs::FS &fs, const char *path, uint16_t x = 0,
                  uint16_t y = 0, uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                  uint16_t offX = 0, uint16_t offY = 0,
                  jpeg_div_t scale = JPEG_DIV_NONE);
                  
    bool drawJpgFile(fs::FS &fs, String path, uint16_t x = 0,
                  uint16_t y = 0, uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                  uint16_t offX = 0, uint16_t offY = 0,
                  jpeg_div_t scale = JPEG_DIV_NONE);
    
    bool drawJpg(const uint8_t *jpg_data, size_t jpg_len, uint16_t x = 0,
                  uint16_t y = 0, uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                  uint16_t offX = 0, uint16_t offY = 0,
                  jpeg_div_t scale = JPEG_DIV_NONE);

    bool drawJpgUrl(String url, uint16_t x = 0,
                           uint16_t y = 0, uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                           uint16_t offX = 0, uint16_t offY = 0, jpeg_div_t scale = JPEG_DIV_NONE);
    
    bool drawPngFile(fs::FS &fs, const char *path, uint16_t x = 0, uint16_t y = 0,
                    uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                    uint16_t offX = 0, uint16_t offY = 0,
                    double scale = 1.0, uint8_t alphaThreshold = 127);

    bool drawPngFile(fs::FS &fs, String path, uint16_t x = 0, uint16_t y = 0,
                    uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                    uint16_t offX = 0, uint16_t offY = 0,
                    double scale = 1.0, uint8_t alphaThreshold = 127);

    bool drawPngUrl(const char *url, uint16_t x = 0, uint16_t y = 0,
                uint16_t maxWidth = 0, uint16_t maxHeight = 0,
                uint16_t offX = 0, uint16_t offY = 0,
                double scale = 1.0, uint8_t alphaThreshold = 127);

    // Re-implement functions
    void *frameBuffer(int8_t f = 1);
    void pushCanvas(int32_t x, int32_t y, m5epd_update_mode_t mode);
    void pushCanvas(m5epd_update_mode_t mode);
    void pushToCanvas(int32_t x, int32_t y, M5EPD_Canvas* canvas);
    uint16_t readPixel(int32_t x, int32_t y);
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data);
    void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t transp, const uint8_t *data);
    void fillCanvas(uint32_t color);
    void drawPixel(int32_t x, int32_t y, uint32_t color);
    // uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);
    uint16_t alphaBlend16(uint8_t alpha, uint8_t fgc, uint8_t bgc);
    void drawFreetypeBitmap(int32_t cx, int32_t cy, uint16_t bw, uint16_t bh, uint8_t *bitmap);

public:
    // Parent functions - freetype
    #ifdef FREETYPE_FONT
    void setTextFillMargin(uint16_t left, uint16_t right, int16_t top, int16_t bottom){TFT_eSPI::setTextFillMargin(left, right, top, bottom);}
    void setTextLineSpace(uint8_t space){TFT_eSPI::setTextLineSpace(space);}
    esp_err_t loadFont(String path, fs::FS &ffs) {return TFT_eSPI::loadFont(path, ffs);}
    esp_err_t loadFont(const uint8_t *memory_ptr, uint32_t length) {return TFT_eSPI::loadFont(memory_ptr, length);}
    esp_err_t unloadFont() {return TFT_eSPI::unloadFont();}
    esp_err_t createRender(uint16_t size, uint16_t cache_size = 1) {return TFT_eSPI::createRender(size, cache_size);}
    esp_err_t destoryRender(uint16_t size) {return TFT_eSPI::destoryRender(size);}
    esp_err_t preRender(uint16_t code) {return TFT_eSPI::preRender(code);}
    bool isRenderExist(uint16_t size) {return TFT_eSPI::isRenderExist(size);}
    uint32_t getNumOfRender(void) {return TFT_eSPI::getNumOfRender();}
    void drawFreetypeGlyph(uint16_t code) {TFT_eSprite::drawFreetypeGlyph(code);}
    void useFreetypeFont(bool isuse = true) {TFT_eSPI::useFreetypeFont(isuse);}
    #endif //FREETYPE_FONT
    // Parent functions - other
    uint32_t getExceedOffset(void) {return TFT_eSPI::getExceedOffset();}
    void setTextArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {TFT_eSprite::setTextArea(x,y,w,h);}
    size_t write(uint8_t utf8){return TFT_eSprite::write(utf8);}
    void *createCanvas(int16_t width, int16_t height, uint8_t frames = 1) { return TFT_eSprite::createSprite(width, height, frames); };
    void deleteCanvas(void) { TFT_eSprite::deleteSprite(); }
    void setPivot(int16_t x, int16_t y) { TFT_eSprite::setPivot(x, y); }
    int16_t getPivotX(void) { return _xpivot; }
    int16_t getPivotY(void) { return _ypivot; }
    int16_t width(void) { return _dwidth; }
    int16_t height(void) { return _dheight; }
    void setCursor(int16_t x, int16_t y) { TFT_eSPI::setCursor(x, y); }
    void setCursor(int16_t x, int16_t y, uint8_t font) { TFT_eSPI::setCursor(x, y, font); }
    int16_t getCursorX(void) { return TFT_eSPI::getCursorX(); }
    int16_t getCursorY(void) { return TFT_eSPI::getCursorY(); }
    uint16_t fontsLoaded(void) { return TFT_eSPI::fontsLoaded(); }
#ifdef LOAD_GFXFF
    void setFreeFont(const GFXfont *f = NULL) { TFT_eSPI::setFreeFont(f); }
    void setTextFont(uint8_t font) { TFT_eSPI::setTextFont(font); }
#else
    void setFreeFont(uint8_t font) { TFT_eSPI::setFreeFont(font); }
    void setTextFont(uint8_t font) { TFT_eSPI::setTextFont(font); }
#endif
    int16_t textWidth(const char *string, uint8_t font) {return TFT_eSPI::textWidth(string, font);}
    int16_t textWidth(const char *string) {return TFT_eSPI::textWidth(string);}
    int16_t textWidth(const String& string, uint8_t font) {return TFT_eSPI::textWidth(string, font);}
    int16_t textWidth(const String& string) {return TFT_eSPI::textWidth(string);}

    // Parent functions - drawing
    void drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) { TFT_eSPI::drawCircle(x0, y0, r, color); }
    void drawCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint32_t color) { TFT_eSPI::drawCircleHelper(x0, y0, r, cornername, color); }
    void fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color) { TFT_eSPI::fillCircle(x0, y0, r, color); }
    void fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint32_t color) { TFT_eSPI::fillCircleHelper(x0, y0, r, cornername, delta, color); }
    void setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1) { TFT_eSprite::setWindow(x0, y0, x1, y1); }
    void pushColor(uint32_t color) { TFT_eSprite::pushColor(color); }
    void pushColor(uint32_t color, uint16_t len) { TFT_eSprite::pushColor(color, len); }
    void writeColor(uint16_t color) { TFT_eSprite::writeColor(color); }
    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) { TFT_eSprite::drawLine(x0, y0, x1, y1, color); }
    void drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) { TFT_eSprite::drawFastVLine(x, y, h, color); }
    void drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) { TFT_eSprite::drawFastHLine(x, y, w, color); }
    void drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size) { TFT_eSprite::drawChar(x, y, c, color, bg, size); }
    int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y) { return TFT_eSprite::drawChar(uniCode, x, y); }
    int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font) { return TFT_eSprite::drawChar(uniCode, x, y, font); }
    void drawGlyph(uint16_t code) { TFT_eSprite::drawGlyph(code); }
    void printToSprite(String string) { TFT_eSprite::printToSprite(string); }
    void printToSprite(char *cbuffer, uint16_t len) { TFT_eSprite::printToSprite(cbuffer, len); }
    int16_t printToSprite(int16_t x, int16_t y, uint16_t index) { return TFT_eSprite::printToSprite(x, y, index); }
    void drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) { TFT_eSPI::drawEllipse(x0, y0, rx, ry, color); }
    void fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) { TFT_eSPI::fillEllipse(x0, y0, rx, ry, color); }
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) { TFT_eSPI::drawRect(x, y, w, h, color); }
    void drawRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color) { TFT_eSPI::drawRoundRect(x0, y0, w, h, radius, color); }
    void fillRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color) { TFT_eSPI::fillRoundRect(x0, y0, w, h, radius, color); }
    void drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) { TFT_eSPI::drawTriangle(x0, y0, x1, y1, x2, y2, color); }
    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) { TFT_eSPI::fillTriangle(x0, y0, x1, y1, x2, y2, color); }
    void setTextColor(uint16_t color) { TFT_eSPI::setTextColor(color); }
    void setTextColor(uint16_t fgcolor, uint16_t bgcolor) { TFT_eSPI::setTextColor(fgcolor, bgcolor); }
    void setTextSize(uint8_t size) { TFT_eSPI::setTextSize(size); }
    void setTextWrap(boolean wrapX, boolean wrapY = false) { TFT_eSPI::setTextWrap(wrapX, wrapY); }
    void setTextDatum(uint8_t datum) { TFT_eSPI::setTextDatum(datum); }
    void setTextPadding(uint16_t x_width) { TFT_eSPI::setTextPadding(x_width); }
    int16_t drawNumber(long long_num, int32_t poX, int32_t poY, uint8_t font) { return TFT_eSPI::drawNumber(long_num, poX, poY, font); }
    int16_t drawNumber(long long_num, int32_t poX, int32_t poY) { return TFT_eSPI::drawNumber(long_num, poX, poY); }
    int16_t drawFloat(float floatNumber, uint8_t decimal, int32_t poX, int32_t poY, uint8_t font) { return TFT_eSPI::drawFloat(floatNumber, decimal, poX, poY, font); }
    int16_t drawFloat(float floatNumber, uint8_t decimal, int32_t poX, int32_t poY) { return TFT_eSPI::drawFloat(floatNumber, decimal, poX, poY); }
    int16_t drawString(const char *string, int32_t poX, int32_t poY, uint8_t font) { return TFT_eSPI::drawString(string, poX, poY, font); }
    int16_t drawString(const char *string, int32_t poX, int32_t poY) { return TFT_eSPI::drawString(string, poX, poY); }
    int16_t drawCentreString(const char *string, int32_t dX, int32_t poY, uint8_t font) { return TFT_eSPI::drawCentreString(string, dX, poY, font); }
    int16_t drawRightString(const char *string, int32_t dX, int32_t poY, uint8_t font) { return TFT_eSPI::drawRightString(string, dX, poY, font); }
    int16_t drawString(const String &string, int32_t poX, int32_t poY, uint8_t font) { return TFT_eSPI::drawString(string, poX, poY, font); }
    int16_t drawString(const String &string, int32_t poX, int32_t poY) { return TFT_eSPI::drawString(string, poX, poY); }
    int16_t drawCentreString(const String &string, int32_t dX, int32_t poY, uint8_t font) { return TFT_eSPI::drawCentreString(string, dX, poY, font); }
    int16_t drawRightString(const String &string, int32_t dX, int32_t poY, uint8_t font) { return TFT_eSPI::drawRightString(string, dX, poY, font); }
    uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining) { return TFT_eSPI::decodeUTF8(buf, index, remaining); };
    uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining, uint8_t *length) { return TFT_eSPI::decodeUTF8(buf, index, remaining, length); };


    // Parent functions - Print
    size_t printf(const char * format, ...)  __attribute__ ((format (printf, 2, 3)));
    size_t print(const __FlashStringHelper *x){return Print::print(x);}
    size_t print(const String &x){return Print::print(x);}
    size_t print(const char x[]){return Print::print(x);}
    size_t print(char x){return Print::print(x);}
    size_t print(unsigned char x, int y = DEC){return Print::print(x, y);}
    size_t print(int x, int y = DEC){return Print::print(x, y);}
    size_t print(unsigned int x, int y = DEC){return Print::print(x, y);}
    size_t print(long x, int y = DEC){return Print::print(x, y);}
    size_t print(unsigned long x, int y = DEC){return Print::print(x, y);}
    size_t print(double x, int y = 2){return Print::print(x, y);}
    size_t print(const Printable& x){return Print::print(x);}
    size_t print(struct tm * timeinfo, const char * format = NULL){return Print::print(timeinfo, format);}

    size_t println(const __FlashStringHelper *x){return Print::println(x);}
    size_t println(const String &x){return Print::println(x);}
    size_t println(const char x[]){return Print::println(x);}
    size_t println(char x){return Print::println(x);}
    size_t println(unsigned char x, int y = DEC){return Print::println(x, y);}
    size_t println(int x, int y = DEC){return Print::println(x, y);}
    size_t println(unsigned int x, int y = DEC){return Print::println(x, y);}
    size_t println(long x, int y = DEC){return Print::println(x, y);}
    size_t println(unsigned long x, int y = DEC){return Print::println(x, y);}
    size_t println(double x, int y = 2){return Print::println(x, y);}
    size_t println(const Printable& x){return Print::println(x);}
    size_t println(struct tm * timeinfo, const char * format = NULL){return Print::println(timeinfo, format);}
    size_t println(void){return Print::println();}

private:
    void *callocSprite(int16_t w, int16_t h, uint8_t frames);

private:
    typedef struct
    {
        uint8_t data;
        uint32_t pos;
        uint32_t len;
    }img_index_t;

private:
    int32_t _last_push_x, _last_push_y;
    M5EPD_Driver *_epd_driver;
    int32_t _bytewidth; // Bytes per line
    uint32_t _buffer_size;
};
#endif
