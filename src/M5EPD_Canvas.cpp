#include "M5EPD_Canvas.h"
#include <HTTPClient.h>

M5EPD_Canvas::M5EPD_Canvas(M5EPD_Driver *driver) : TFT_eSprite(NULL)
{
    _epd_driver = driver;
    _bpp = 4;
    _bytewidth = 0;
    _last_push_x = 0;
    _last_push_y = 0;
}

M5EPD_Canvas::M5EPD_Canvas() : TFT_eSprite(NULL)
{
}

M5EPD_Canvas::~M5EPD_Canvas()
{
    deleteCanvas();
}

void M5EPD_Canvas::operator=(const M5EPD_Canvas &src)
{
    deleteCanvas();
    createCanvas(src._dwidth, src._dheight);
    memcpy(_img8, src._img8, src._buffer_size);
}

void M5EPD_Canvas::setDriver(M5EPD_Driver *driver)
{
    _epd_driver = driver;
}

void M5EPD_Canvas::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    if (!_created)
        return;

    if ((x >= _iwidth) || (y >= _iheight))
        return;

    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }

    if ((x + w) > _iwidth)
        w = _iwidth - x;
    if ((y + h) > _iheight)
        h = _iheight - y;

    if ((w < 1) || (h < 1))
        return;

    uint8_t c = color << 4 | color;
    bool oddx = x & 0x01;
    bool oddsum = (x + w) & 0x01;
    if (oddx)
    {
        uint8_t *yp = _img8 + (y * _bytewidth + (x >> 1) + 1);
        if (oddsum)
        {
            int32_t len = (w >> 1) - 1;
            int32_t rx = x + w - 1;
            for (int i = y; i < y + h; i++)
            {
                drawPixel(x, i, color);
                drawPixel(rx, i, color);
                memset(yp, c, len);
                yp += _bytewidth;
            }
        }
        else
        {
            int32_t len = (w >> 1);
            for (int i = y; i < y + h; i++)
            {
                drawPixel(x, i, color);
                memset(yp, c, len);
                yp += _bytewidth;
            }
        }
    }
    else
    {
        uint8_t *yp = _img8 + (y * _bytewidth + (x >> 1));
        int32_t len = (w >> 1);
        if (oddsum)
        {
            int32_t rx = x + w - 1;
            for (int i = y; i < y + h; i++)
            {
                drawPixel(rx, i, color);
                memset(yp, c, len);
                yp += _bytewidth;
            }
        }
        else
        {
            for (int i = y; i < y + h; i++)
            {
                memset(yp, c, len);
                yp += _bytewidth;
            }
        }
    }
}


void M5EPD_Canvas::ReversePartColor(int32_t x, int32_t y, int32_t w, int32_t h)
{
    if (!_created)
        return;

    if ((x >= _iwidth) || (y >= _iheight))
        return;

    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }

    if ((x + w) > _iwidth)
        w = _iwidth - x;
    if ((y + h) > _iheight)
        h = _iheight - y;

    if ((w < 1) || (h < 1))
        return;

    for(int j = y; j < y + h; j++)
    {
        for(int i = x; i < x + w; i++)
        {
            drawPixel(i, j, 15 - readPixel(i, j));
        }
    }
}

/** @brief Allocate a memory area for the Sprite and return pointer
  * @param w frame width
  * @param h frame height
  * @param frames *No support for now.
  * @retval pointer to memory
  */
void *M5EPD_Canvas::callocSprite(int16_t w, int16_t h, uint8_t frames)
{
    // Add one extra "off screen" pixel to point out-of-bounds setWindow() coordinates
    // this means push/writeColor functions do not need additional bounds checks and
    // hence will run faster in normal circumstances.
    uint8_t *ptr8 = NULL;

    w = (w + 3) & 0xFFFC; // width should be the multiple of 4 bits to be compatible with epdpaint
    _iwidth = w;          // _iwidth is rounded up to be multiple of 8, so might not be = _dwidth
    _bitwidth = w;
    _bytewidth = w >> 1;

#if defined(ESP32) && defined(CONFIG_SPIRAM_SUPPORT)
    if (psramFound() && _usePsram)
        ptr8 = (uint8_t *)ps_calloc(_bytewidth * h + 1, sizeof(uint8_t));
    else
#endif
        ptr8 = (uint8_t *)calloc(_bytewidth * h + 1, sizeof(uint8_t));

    _buffer_size = _bytewidth * h + 1;
    // log_d("calloc = %d bytes", _bytewidth * h + 1);
    return ptr8;
}

/** @brief Get pointer to memory
  * @param f *Not support for now.
  * @retval pointer to memory
  */
void *M5EPD_Canvas::frameBuffer(int8_t f)
{
    if (!_created)
        return NULL;

    return _img8;
}

/** @brief Push the sprite to panel at x, y
  * @param x pos x
  * @param y pos y
  * @param mode update mode
  */
void M5EPD_Canvas::pushCanvas(int32_t x, int32_t y, m5epd_update_mode_t mode)
{
    if (!_created)
        return;

    _last_push_x = x;
    _last_push_y = y;

    _epd_driver->WritePartGram4bpp(x, y, _iwidth, _iheight, _img8);
    _epd_driver->UpdateArea(x, y, _iwidth, _iheight, mode);
}

/** @brief Update spirte to panel without data transfer
  * @param mode update mode
  */
void M5EPD_Canvas::pushCanvas(m5epd_update_mode_t mode)
{
    if (!_created)
        return;
    _epd_driver->UpdateArea(_last_push_x, _last_push_y, _iwidth, _iheight, mode);
}

void M5EPD_Canvas::pushToCanvas(int32_t x, int32_t y, M5EPD_Canvas *canvas)
{
    if (!_created)
        return;

    canvas->pushImage(x, y, _iwidth, _iheight, _img8);
}

/** @brief Read color of a pixel at defined coordinates
  * @param x pos x
  * @param y pos y
  */
uint16_t M5EPD_Canvas::readPixel(int32_t x, int32_t y)
{
    if ((x < 0) || (x >= _iwidth) || (y < 0) || (y >= _iheight) || !_created)
        return 0;

    if (x & 0x1)
    {
        return _img8[y * _bytewidth + (x >> 1)] & 0x0F;
    }
    else
    {
        return _img8[y * _bytewidth + (x >> 1)] >> 4;
    }
}

/** @brief push 16 gray level image into a defined area of a sprite 
  * @param x pos x
  * @param y pos y
  * @param w pos wt
  * @param h pos h
  * @param data pos pointer to image data, two pixels per byte.
  */
void M5EPD_Canvas::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data)
{
    if ((x >= _iwidth) || (y >= _iheight) || (w == 0) || (h == 0) || !_created)
        return;
    if ((x + w < 0) || (y + h < 0))
        return;
    if (data == nullptr)
        return;

    // if(data[0] == 0x23 && data[1] == 0x33)
    // {
    //     uint32_t total_len = *((uint32_t*)(&data[2]));
    //     uint16_t index_len = *((uint16_t*)(&data[6]));
    //     img_index_t img_index[index_len];

    //     for(int i = 0; i < index_len; i++)
    //     {
    //         img_index[i].data = data[6 + i * 9];
    //         img_index[i].pos = *((uint32_t*)(&data[6 + i * 9 + 1]));
    //         img_index[i].len = *((uint32_t*)(&data[6 + i * 9 + 5]));
    //     }

    //     return;
    // }

    int32_t nbytes = (w >> 1) + (w & 1);
    int32_t i0 = 0, j0 = 0;
    for (int32_t i = y; i < y + h; i++)
    {
        j0 = 0;
        for (int32_t j = x; j < x + w; j++)
        {
            uint8_t c = (*(data + i0 * nbytes + (j0 >> 1)));
            c = (j0 & 0x01) ? c & 0x0F : c >> 4;

            int32_t index = i * _bytewidth + (j >> 1);
            if (j & 0x01)
            {
                _img8[index] &= 0xF0;
                _img8[index] |= c;
            }
            else
            {
                _img8[index] &= 0x0F;
                _img8[index] |= c << 4;
            }

            j0++;
        }
        i0++;
    }
}

/** @brief push 16 gray level image into a defined area of a sprite with transparent
  * @param x pos x
  * @param y pos y
  * @param w pos wt
  * @param h pos h
  * @param transp transparent color
  * @param data pos pointer to image data, two pixels per byte.
  */
void M5EPD_Canvas::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t transp, const uint8_t *data)
{
    if ((x >= _iwidth) || (y >= _iheight) || (w == 0) || (h == 0) || !_created)
        return;
    if ((x + w < 0) || (y + h < 0))
        return;
    if (data == nullptr)
        return;

    int32_t nbytes = (w >> 1) + (w & 1);
    int i0 = 0, j0 = 0;
    transp &= 0x0F;
    uint8_t transp2 = transp << 4;
    for (int32_t i = y; i < y + h; i++)
    {
        j0 = 0;
        for (int32_t j = x; j < x + w; j++)
        {
            uint8_t c = (*(data + i0 * nbytes + (j0 >> 1)));
            c = (j0 & 0x01) ? c & 0x0F : c >> 4;

            if (c == transp || c == transp2)
            {
                j0++;
                continue;
            }
            int32_t index = i * _bytewidth + (j >> 1);
            if (j & 0x01)
            {
                _img8[index] &= 0xF0;
                _img8[index] |= c;
            }
            else
            {
                _img8[index] &= 0x0F;
                _img8[index] |= c << 4;
            }

            j0++;
        }
        i0++;
    }
}

/** @brief Fill the whole sprite with defined colour
  * @param color fill color
  */
void M5EPD_Canvas::fillCanvas(uint32_t color)
{
    if (!_created)
        return;

    color &= 0x0F;
    uint8_t c = color << 4 | color;
    memset(_img8, c, _bytewidth * _dheight + 1);
}

/** @brief push a single pixel at an arbitrary position
  * @param x pos x
  * @param y pos y
  * @param color pixel color
  */
void M5EPD_Canvas::drawPixel(int32_t x, int32_t y, uint32_t color)
{
    // Range checking
    if ((x < 0) || (y < 0) || !_created)
        return;
    if ((x >= _iwidth) || (y >= _iheight))
        return;

    color &= 0x0F;

    int32_t index = y * _bytewidth + (x >> 1);

    if (x & 0x1)
    {
        _img8[index] &= 0xF0;
        _img8[index] |= color;
    }
    else
    {
        _img8[index] &= 0x0F;
        _img8[index] |= color << 4;
    }
}

/** @brief Re-implement printf
  */
size_t M5EPD_Canvas::printf(const char *format, ...)
{
    char loc_buf[64];
    char *temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if (len < 0)
    {
        va_end(arg);
        return 0;
    };
    if (len >= sizeof(loc_buf))
    {
        temp = (char *)malloc(len + 1);
        if (temp == NULL)
        {
            va_end(arg);
            return 0;
        }
        len = vsnprintf(temp, len + 1, format, arg);
    }
    va_end(arg);
    len = Print::write((uint8_t *)temp, len);
    if (temp != loc_buf)
    {
        free(temp);
    }
    return len;
}

/** @brief push a single pixel at an arbitrary position
  * @param string qrcode content
  * @param x pos x
  * @param y pos y
  * @param width qrcode size
  * @param version qrcode version
  */
void M5EPD_Canvas::qrcode(const char *string, uint16_t x, uint16_t y, uint16_t width, uint8_t version)
{

    // Create the QR code
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(version)];
    qrcode_initText(&qrcode, qrcodeData, version, 0, string);

    // Top quiet zone
    uint8_t thickness = width / qrcode.size;
    uint16_t lineLength = qrcode.size * thickness;
    uint8_t xOffset = x + (width - lineLength) / 2;
    uint8_t yOffset = y + (width - lineLength) / 2;
    fillRect(x, y, width, width, 0);

    for (uint8_t y = 0; y < qrcode.size; y++)
    {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            uint8_t q = qrcode_getModule(&qrcode, x, y);
            if (q)
                fillRect(x * thickness + xOffset, y * thickness + yOffset, thickness, thickness, 15);
        }
    }
}

/** @brief push a single pixel at an arbitrary position
  * @param string qrcode content
  * @param x pos x
  * @param y pos y
  * @param width qrcode size
  * @param version qrcode version
  */
void M5EPD_Canvas::qrcode(const String &string, uint16_t x, uint16_t y, uint16_t width, uint8_t version)
{
    int16_t len = string.length() + 2;
    char buffer[len];
    string.toCharArray(buffer, len);
    qrcode(buffer, x, y, width, version);
}

void M5EPD_Canvas::fillCenterSquare(int32_t cx, int32_t cy, uint16_t w, uint8_t color)
{
    uint16_t halfw = w >> 1;
    int32_t ystart = cy - halfw;
    int32_t yend = ystart + w;
    int32_t xstart = cx - halfw;
    int32_t xend = xstart + w;
    for (int32_t yy = ystart; yy < yend; yy++)
    {
        for (int32_t xx = xstart; xx < xend; xx++)
        {
            drawPixel(yy, xx, color);
        }
    }
}

void M5EPD_Canvas::drawFastVLine(int32_t x, int32_t y, int32_t h, uint8_t thickness, uint8_t color)
{

    if ((x < 0) || (x >= _iwidth) || (y >= _iheight) || !_created)
        return;

    if (y < 0)
    {
        h += y;
        y = 0;
    }

    if ((y + h) > _iheight)
        h = _iheight - y;

    if (h < 1)
        return;

    int32_t xstart = x - (thickness >> 1);
    int32_t xend = xstart + thickness;

    while (h--)
    {
        for (int32_t xx = xstart; xx < xend; xx++)
        {
            drawPixel(xx, y, color);
        }
        y++;
    }
}

void M5EPD_Canvas::drawFastHLine(int32_t x, int32_t y, int32_t w, uint8_t thickness, uint8_t color)
{

    if ((y < 0) || (x >= _iwidth) || (y >= _iheight) || !_created)
        return;

    if (x < 0)
    {
        w += x;
        x = 0;
    }

    if ((x + w) > _iwidth)
        w = _iwidth - x;

    if (w < 1)
        return;

    int32_t ystart = y - (thickness >> 1);
    int32_t yend = ystart + thickness;

    while (w--)
    {
        for (int32_t yy = ystart; yy < yend; yy++)
        {
            drawPixel(x, yy, color);
        }
        x++;
    }
}

void M5EPD_Canvas::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t thickness, uint8_t color)
{
    boolean steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        swap_coord(x0, y0);
        swap_coord(x1, y1);
    }

    if (x0 > x1)
    {
        swap_coord(x0, x1);
        swap_coord(y0, y1);
    }

    int32_t dx = x1 - x0, dy = abs(y1 - y0);
    int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

    if (y0 < y1)
        ystep = 1;

    // uint8_t half_thickness = thickness >> 1;

    // Split into steep and not steep for FastH/V separation
    if (steep)
    {
        for (; x0 <= x1; x0++)
        {
            dlen++;
            err -= dy;
            if (err < 0)
            {
                err += dx;
                drawFastVLine(y0, xs, dlen, thickness, color);
                dlen = 0;
                y0 += ystep;
                xs = x0 + 1;
            }
        }
        if (dlen)
        {
            drawFastVLine(y0, xs, dlen, thickness, color);
        }
    }
    else
    {
        for (; x0 <= x1; x0++)
        {
            dlen++;
            err -= dy;
            if (err < 0)
            {
                err += dx;
                drawFastHLine(xs, y0, dlen, thickness, color);
                dlen = 0;
                y0 += ystep;
                xs = x0 + 1;
            }
        }
        if (dlen)
        {
            drawFastHLine(xs, y0, dlen, thickness, color);
        }
    }
}

void M5EPD_Canvas::drawFreetypeBitmap(int32_t cx, int32_t cy, uint16_t bw, uint16_t bh, uint8_t *bitmap)
{
    uint32_t pos = 0;
    uint16_t bg = textbgcolor;
    uint16_t fg = textcolor;
    bool _is_textbk_transp = fg == bg;
    for (int y = 0; y < bh; y++)
    {
        for (int x = 0; x < bw; x++)
        {
            if (_is_textbk_transp)
                bg = readPixel(cx + x, cy + y);
            if (pos & 0x1)
            {
                drawPixel(cx + x, cy + y, alphaBlend16(bitmap[pos >> 1] & 0x0F, fg, bg));
            }
            else
            {
                drawPixel(cx + x, cy + y, alphaBlend16(bitmap[pos >> 1] >> 4, fg, bg));
            }
            pos++;
        }
    }
}

// /** @brief Anti-aliasing gradient color calculation, only supports 2bit.
//   * @param alpha alpha, 0 ~ 3
//   * @param fgc front color
//   * @param bgc back color
//   */
// uint16_t M5EPD_Canvas::alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc)
// {
//     if (alpha == 3)
//     {
//         return fgc;
//     }
//     else if (alpha == 0)
//     {
//         return bgc;
//     }
//     if (bgc == fgc)
//     {
//         return fgc;
//     }

//     uint8_t step = abs(fgc - bgc) >> 2;

//     if (fgc > bgc)
//     {
//         return bgc + alpha * step;
//     }

//     if (bgc > fgc)
//     {
//         return fgc + alpha * step;
//     }

//     return fgc;
// }

const uint8_t alphamap[16] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};

uint16_t M5EPD_Canvas::alphaBlend16(uint8_t alpha, uint8_t fgc, uint8_t bgc)
{
    if (alpha == 15)
    {
        return fgc;
    }
    else if (alpha == 0)
    {
        return bgc;
    }
    if (bgc == fgc)
    {
        return fgc;
    }

    uint8_t step = abs(alphamap[fgc] - alphamap[bgc]) >> 4;

    if (fgc > bgc)
    {
        return bgc + ((alpha * step) >> 4);
    }

    if (bgc > fgc)
    {
        return bgc - ((alpha * step) >> 4);
        // return fgc + (((15 - alpha) * step) >> 4);
    }

    return fgc;
}

void M5EPD_Canvas::ReverseColor(void)
{
    for (int i = 0; i < _buffer_size; i++)
    {
        _img8[i] = 0xFF - _img8[i];
    }
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(fs::File &f)
{
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

uint32_t read32(fs::File &f)
{
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
}

const uint8_t kGrayScaleMap[] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Bodmers BMP image rendering function
bool M5EPD_Canvas::drawBmpFile(fs::FS &fs, String path, uint16_t x, uint16_t y)
{
    return drawBmpFile(fs, path.c_str(), x, y);
}

bool M5EPD_Canvas::drawBmpFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y)
{
    if ((x >= width()) || (y >= height()))
        return 0;

    // Open requested file on SD card
    File bmpFS = fs.open(path, "r");

    if (!bmpFS)
    {
        log_e("File not found");
        return 0;
    }

    uint32_t seekOffset;
    uint16_t w, h, row, col;
    uint8_t r, g, b;

    uint32_t startTime = millis();

    if (read16(bmpFS) == 0x4D42)
    {
        read32(bmpFS);
        read32(bmpFS);
        seekOffset = read32(bmpFS);
        read32(bmpFS);
        w = read32(bmpFS);
        h = read32(bmpFS);

        if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
        {
            y += h - 1;

            bmpFS.seek(seekOffset);

            uint16_t padding = (4 - ((w * 3) & 3)) & 3;
            uint8_t lineBuffer[w * 3 + padding];

            for (row = 0; row < h; row++)
            {
                bmpFS.read(lineBuffer, sizeof(lineBuffer));
                uint8_t *bptr = lineBuffer;
                // Convert 24 to 16 bit colours
                for (col = 0; col < w; col++)
                {
                    b = *bptr++;
                    g = *bptr++;
                    r = *bptr++;
                    drawPixel(x + col, y, kGrayScaleMap[(r * 38 + g * 75 + b * 15) >> 7]);
                }

                // Push the pixel row to screen, pushImage will crop the line if needed
                // y is decremented as the BMP image is drawn bottom up
                y--;
            }
            log_d("Loaded in %lu ms", millis() - startTime);
        }
        else
        {
            log_e("BMP format not recognized.");
            return 0;
        }
            
    }
    bmpFS.close();
    return 1;
}

/***************************************************
  This library is written to be compatible with Adafruit's ILI9341
  library and automatically detects the display type on ESP_WROVER_KITs
  Earlier WROVERs had ILI9341, while newer releases have ST7789V

  MIT license, all text above must be included in any redistribution
 ****************************************************/

/*
 * JPEG
 */

#include "rom/tjpgd.h"

#define jpgColor(c)                                  \
    (((uint16_t)(((uint8_t *)(c))[0] & 0xF8) << 8) | \
     ((uint16_t)(((uint8_t *)(c))[1] & 0xFC) << 3) | \
     ((((uint8_t *)(c))[2] & 0xF8) >> 3))

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_ERROR
const char *jd_errors[] = {"Succeeded",
                           "Interrupted by output function",
                           "Device error or wrong termination of input stream",
                           "Insufficient memory pool for the image",
                           "Insufficient stream input buffer",
                           "Parameter error",
                           "Data format error",
                           "Right format but not supported",
                           "Not supported JPEG standard"};
#endif

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t maxWidth;
    uint16_t maxHeight;
    uint16_t offX;
    uint16_t offY;
    jpeg_div_t scale;
    const void *src;
    size_t len;
    size_t index;
    M5EPD_Canvas *canvas;
    uint16_t outWidth;
    uint16_t outHeight;
} jpg_file_decoder_t;

static uint32_t jpgReadFile(JDEC *decoder, uint8_t *buf, uint32_t len)
{
    jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
    File *file = (File *)jpeg->src;
    if (buf)
    {
        return file->read(buf, len);
    }
    else
    {
        file->seek(len, SeekCur);
    }
    return len;
}

static uint32_t jpgRead(JDEC *decoder, uint8_t *buf, uint32_t len)
{
    jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
    if (buf)
    {
        memcpy(buf, (const uint8_t *)jpeg->src + jpeg->index, len);
    }
    jpeg->index += len;
    return len;
}

static uint32_t jpgWrite(JDEC *decoder, void *bitmap, JRECT *rect)
{
    jpg_file_decoder_t *jpeg = (jpg_file_decoder_t *)decoder->device;
    uint16_t x = rect->left;
    uint16_t y = rect->top;
    uint16_t w = rect->right + 1 - x;
    uint16_t h = rect->bottom + 1 - y;
    uint16_t oL = 0, oR = 0;
    uint8_t *data = (uint8_t *)bitmap;

    if (rect->right < jpeg->offX)
    {
        return 1;
    }
    if (rect->left >= (jpeg->offX + jpeg->outWidth))
    {
        return 1;
    }
    if (rect->bottom < jpeg->offY)
    {
        return 1;
    }
    if (rect->top >= (jpeg->offY + jpeg->outHeight))
    {
        return 1;
    }
    if (rect->top < jpeg->offY)
    {
        uint16_t linesToSkip = jpeg->offY - rect->top;
        data += linesToSkip * w * 3;
        h -= linesToSkip;
        y += linesToSkip;
    }
    if (rect->bottom >= (jpeg->offY + jpeg->outHeight))
    {
        uint16_t linesToSkip = (rect->bottom + 1) - (jpeg->offY + jpeg->outHeight);
        h -= linesToSkip;
    }
    if (rect->left < jpeg->offX)
    {
        oL = jpeg->offX - rect->left;
    }
    if (rect->right >= (jpeg->offX + jpeg->outWidth))
    {
        oR = (rect->right + 1) - (jpeg->offX + jpeg->outWidth);
    }

    uint16_t line;

    uint16_t canvas_x = x - jpeg->offX + jpeg->x + oL;
    uint16_t canvas_y = y - jpeg->offY + jpeg->y;
    uint16_t canvas_w = w - (oL + oR);
    uint16_t canvas_h = h;
    while (h--)
    {
        data += 3 * oL;
        line = canvas_w;
        while (line--)
        {
            jpeg->canvas->drawPixel(canvas_x + (canvas_w - line), canvas_y + (canvas_h - h), kGrayScaleMap[(data[0] * 38 + data[1] * 75 + data[2] * 15) >> 7]);
            data += 3;
        }
        data += 3 * oR;
    }
    return 1;
}

static bool jpgDecode(jpg_file_decoder_t *jpeg,
                      uint32_t (*reader)(JDEC *, uint8_t *, uint32_t))
{
    static uint8_t work[3100];
    JDEC decoder;

    JRESULT jres = jd_prepare(&decoder, reader, work, 3100, jpeg);
    if (jres != JDR_OK)
    {
        log_e("jd_prepare failed! %s", jd_errors[jres]);
        return false;
    }

    uint16_t jpgWidth = decoder.width / (1 << (uint8_t)(jpeg->scale));
    uint16_t jpgHeight = decoder.height / (1 << (uint8_t)(jpeg->scale));

    if (jpeg->offX >= jpgWidth || jpeg->offY >= jpgHeight)
    {
        log_e("Offset Outside of JPEG size");
        return false;
    }

    size_t jpgMaxWidth = jpgWidth - jpeg->offX;
    size_t jpgMaxHeight = jpgHeight - jpeg->offY;

    jpeg->outWidth =
        (jpgMaxWidth > jpeg->maxWidth) ? jpeg->maxWidth : jpgMaxWidth;
    jpeg->outHeight =
        (jpgMaxHeight > jpeg->maxHeight) ? jpeg->maxHeight : jpgMaxHeight;

    jres = jd_decomp(&decoder, jpgWrite, (uint8_t)jpeg->scale);
    if (jres != JDR_OK)
    {
        log_e("jd_decomp failed! %s", jd_errors[jres]);
        return false;
    }

    return true;
}

bool M5EPD_Canvas::drawJpg(const uint8_t *jpg_data, size_t jpg_len, uint16_t x,
                           uint16_t y, uint16_t maxWidth, uint16_t maxHeight,
                           uint16_t offX, uint16_t offY, jpeg_div_t scale)
{
    if ((x + maxWidth) > width() || (y + maxHeight) > height())
    {
        log_e("Bad dimensions given");
        return 0;
    }

    jpg_file_decoder_t jpeg;

    if (!maxWidth)
    {
        maxWidth = width() - x;
    }
    if (!maxHeight)
    {
        maxHeight = height() - y;
    }

    jpeg.src = jpg_data;
    jpeg.len = jpg_len;
    jpeg.index = 0;
    jpeg.x = x;
    jpeg.y = y;
    jpeg.maxWidth = maxWidth;
    jpeg.maxHeight = maxHeight;
    jpeg.offX = offX;
    jpeg.offY = offY;
    jpeg.scale = scale;
    jpeg.canvas = this;

    return jpgDecode(&jpeg, jpgRead);
}

bool M5EPD_Canvas::drawJpgFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y,
                           uint16_t maxWidth, uint16_t maxHeight, uint16_t offX,
                           uint16_t offY, jpeg_div_t scale)
{
    if ((x + maxWidth) > width() || (y + maxHeight) > height())
    {
        log_e("Bad dimensions given");
        return 0;
    }

    File file = fs.open(path);
    if (!file)
    {
        log_e("Failed to open file for reading");
        return 0;
    }

    jpg_file_decoder_t jpeg;

    if (!maxWidth)
    {
        maxWidth = width() - x;
    }
    if (!maxHeight)
    {
        maxHeight = height() - y;
    }

    jpeg.src = &file;
    jpeg.len = file.size();
    jpeg.index = 0;
    jpeg.x = x;
    jpeg.y = y;
    jpeg.maxWidth = maxWidth;
    jpeg.maxHeight = maxHeight;
    jpeg.offX = offX;
    jpeg.offY = offY;
    jpeg.scale = scale;
    jpeg.canvas = this;

    bool ret = jpgDecode(&jpeg, jpgReadFile);

    file.close();
    return ret;
}

bool drawJpgFile(fs::FS &fs, String path, uint16_t x, uint16_t y,
                            uint16_t maxWidth, uint16_t maxHeight, uint16_t offX,
                            uint16_t offY, jpeg_div_t scale)
{
    return drawJpgFile(fs, path.c_str(), x, y, maxWidth, maxHeight, offX, offY, scale);
}

bool M5EPD_Canvas::drawJpgUrl(String url, uint16_t x,
                           uint16_t y, uint16_t maxWidth, uint16_t maxHeight,
                           uint16_t offX, uint16_t offY, jpeg_div_t scale)
{
    if ((x + maxWidth) > width() || (y + maxHeight) > height())
    {
        log_e("Bad dimensions given");
        return 0;
    }

    jpg_file_decoder_t jpeg;

    if (!maxWidth)
    {
        maxWidth = width() - x;
    }
    if (!maxHeight)
    {
        maxHeight = height() - y;
    }

    HTTPClient http;

    if (WiFi.status() != WL_CONNECTED)
    {
        log_e("Not connected");
        return 0;
    }

    http.begin(url);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK)
    {
        log_e("HTTP ERROR: %d\n", httpCode);
        http.end();
        return 0;
    }

    size_t size = http.getSize();

    log_d("jpg size = %d Bytes", size);

    WiFiClient *stream = http.getStreamPtr();
    uint8_t *p = (uint8_t*)ps_malloc(size);
    if(p == NULL)
    {
        log_e("Memory overflow.");
        return 0;
    }

    log_d("downloading...");
    size_t offset = 0;
    while (http.connected())
    {
        size_t len = stream->available();
        if (!len)
        {
            delay(1);
            continue;
        }

        stream->readBytes(p + offset, len);
        offset += len;
        log_d("%d / %d", offset, size);
        if(offset == size)
        {
            break;
        }
    }

    jpeg.src = p;
    jpeg.len = size;
    jpeg.index = 0;
    jpeg.x = x;
    jpeg.y = y;
    jpeg.maxWidth = maxWidth;
    jpeg.maxHeight = maxHeight;
    jpeg.offX = offX;
    jpeg.offY = offY;
    jpeg.scale = scale;
    jpeg.canvas = this;

    http.end();

    log_d("Drawing...");
    int ret = jpgDecode(&jpeg, jpgRead);
    log_d("done");
    free(p);
    return ret;
}

/*
 * PNG
 */

#include "utility/pngle.h"

typedef struct _png_draw_params
{
    uint16_t x;
    uint16_t y;
    uint16_t maxWidth;
    uint16_t maxHeight;
    uint16_t offX;
    uint16_t offY;
    double scale;
    uint8_t alphaThreshold;
    M5EPD_Canvas *canvas;
} png_file_decoder_t;

static void pngle_draw_callback(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4])
{
    png_file_decoder_t *p = (png_file_decoder_t *)pngle_get_user_data(pngle);

    if (x < p->offX || y < p->offY)
        return;
    x -= p->offX;
    y -= p->offY;

    // An interlaced file with alpha channel causes disaster, so use 1 here for simplicity
    w = 1;
    h = 1;

    if (p->scale != 1.0)
    {
        x = (uint32_t)ceil(x * p->scale);
        y = (uint32_t)ceil(y * p->scale);
        w = (uint32_t)ceil(w * p->scale);
        h = (uint32_t)ceil(h * p->scale);
    }

    if (x >= p->maxWidth || y >= p->maxHeight)
        return;
    if (x + w >= p->maxWidth)
        w = p->maxWidth - x;
    if (y + h >= p->maxHeight)
        h = p->maxHeight - y;

    x += p->x;
    y += p->y;

    if (rgba[3] >= p->alphaThreshold)
    {
        p->canvas->fillRect(x, y, w, h, kGrayScaleMap[(rgba[0] * 38 + rgba[1] * 75 + rgba[2] * 15) >> 7]);
    }
}

bool M5EPD_Canvas::drawPngFile(fs::FS &fs, const char *path, uint16_t x, uint16_t y,
                               uint16_t maxWidth, uint16_t maxHeight, uint16_t offX,
                               uint16_t offY, double scale, uint8_t alphaThreshold)
{
    File file = fs.open(path);
    if (!file)
    {
        log_e("Failed to open file for reading");
        return 0;
    }

    pngle_t *pngle = pngle_new();

    png_file_decoder_t png;

    if (!maxWidth)
    {
        maxWidth = width() - x;
    }
    if (!maxHeight)
    {
        maxHeight = height() - y;
    }

    png.x = x;
    png.y = y;
    png.maxWidth = maxWidth;
    png.maxHeight = maxHeight;
    png.offX = offX;
    png.offY = offY;
    png.scale = scale;
    png.alphaThreshold = alphaThreshold;
    png.canvas = this;

    pngle_set_user_data(pngle, &png);
    pngle_set_draw_callback(pngle, pngle_draw_callback);

    // Feed data to pngle
    uint8_t buf[1024];
    int remain = 0;
    int len;
    while ((len = file.read(buf + remain, sizeof(buf) - remain)) > 0)
    {
        int fed = pngle_feed(pngle, buf, remain + len);
        if (fed < 0)
        {
            log_e("[pngle error] %s", pngle_error(pngle));
            pngle_destroy(pngle);
            return 0;
        }

        remain = remain + len - fed;
        if (remain > 0)
            memmove(buf, buf + fed, remain);
    }

    pngle_destroy(pngle);
    file.close();
    return 1;
}

// Please note that drawing png will consume a lot of time
bool drawPngFile(fs::FS &fs, String path, uint16_t x, uint16_t y,
                               uint16_t maxWidth, uint16_t maxHeight, uint16_t offX,
                               uint16_t offY, double scale, uint8_t alphaThreshold)
{
    return drawPngFile(fs, path.c_str(), x, y, maxWidth, maxHeight, offX, offY, scale, alphaThreshold);
}

// Please note that drawing png will consume a lot of time
bool M5EPD_Canvas::drawPngUrl(const char *url, uint16_t x, uint16_t y,
                              uint16_t maxWidth, uint16_t maxHeight, uint16_t offX,
                              uint16_t offY, double scale, uint8_t alphaThreshold)
{
    HTTPClient http;

    if (WiFi.status() != WL_CONNECTED)
    {
        log_e("Not connected");
        return 0;
    }

    http.begin(url);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK)
    {
        log_e("HTTP ERROR: %d\n", httpCode);
        http.end();
        return 0;
    }

    WiFiClient *stream = http.getStreamPtr();

    pngle_t *pngle = pngle_new();

    png_file_decoder_t png;

    if (!maxWidth)
    {
        maxWidth = width() - x;
    }
    if (!maxHeight)
    {
        maxHeight = height() - y;
    }

    png.x = x;
    png.y = y;
    png.maxWidth = maxWidth;
    png.maxHeight = maxHeight;
    png.offX = offX;
    png.offY = offY;
    png.scale = scale;
    png.alphaThreshold = alphaThreshold;
    png.canvas = this;

    pngle_set_user_data(pngle, &png);
    pngle_set_draw_callback(pngle, pngle_draw_callback);

    // Feed data to pngle
    uint8_t buf[1024];
    int remain = 0;
    int len;
    while (http.connected())
    {
        size_t size = stream->available();
        if (!size)
        {
            delay(1);
            continue;
        }

        if (size > sizeof(buf) - remain)
            size = sizeof(buf) - remain;
        if ((len = stream->readBytes(buf + remain, size)) > 0)
        {
            int fed = pngle_feed(pngle, buf, remain + len);
            if (fed < 0)
            {
                log_e("[pngle error] %s", pngle_error(pngle));
                pngle_destroy(pngle);
                http.end();
                return 0;
            }

            remain = remain + len - fed;
            if (remain > 0)
                memmove(buf, buf + fed, remain);
        }
    }

    pngle_destroy(pngle);
    http.end();
    return 1;
}


