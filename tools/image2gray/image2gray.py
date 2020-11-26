import os
from PIL import Image
import re
import copy
import struct
import shutil

GRAYSCALE_MAP = [0 for i in range(256)]

x = -1
for i in range(0, 256):
    if i % 16 == 0:
        x += 1
    GRAYSCALE_MAP[i] = 15 - x

def walkFile(file):
    result = []
    for root, dirs, files in os.walk(file):

        for f in files:
            x = os.path.join(root, f)
            if '.png' in x or '.bmp' in x or '.jpg' in x:
                try:
                    result.append((x, re.search('/(\w+)\.', x)[1].lower()))
                except:
                    print('err', x)
    return result

def ExtractBuffer(data):
    if data[0] != 0x23 or data[1] != 0x33:
        return
    
    total_len = struct.unpack('<1I', data[2:6])[0]
    index_len = struct.unpack('<1H', data[6:8])[0]
    index = [] # data[0] pos[1] len[2]
    for i in range(index_len):
        index.append(struct.unpack('<1B2I', data[8 + i * 9 : 8 + i * 9 + 9]))

    cursor = 2 + 4 + 2 + index_len * 9
    idx = 0
    buffer = bytes()
    i = 0
    while 1:
        if i == index[idx][1]:
            buffer += bytes([index[idx][0] for _ in range(index[idx][2])])
            i += index[idx][2]

            idx += 1
            if idx == len(index):
                buffer += (data[cursor: cursor + total_len - i])
                break
        else:
            length = index[idx][1] - i
            buffer += (data[cursor: cursor + length])
            cursor += length

            i = index[idx][1]

    return buffer

def getBitmap(img):
    buffer = []
    byte = 0
    img = img.convert('L')
    for y in range(img.height):
        for x in range(0, img.width, 2):
            byte = GRAYSCALE_MAP[img.getpixel((x, y))] << 4
            if x + 1 < img.width:
                byte |= GRAYSCALE_MAP[img.getpixel((x + 1, y))]
            buffer.append(byte)

    return bytes(buffer)

png_files = walkFile("./")
png_files = walkFile("./")

dstf = open('ImageResource.h', 'w')
dstf.write('#ifndef IMAGERESOURCE_H\n#define IMAGERESOURCE_H\n\n')
dstf.write('// ----- ImageResource Overview -----\n')
dstf.write('// Name, Width x Height\n')

png_files.sort()

for f in png_files:
    img = Image.open(f[0])
    dstf.write('// ImageResource_%s_%dx%d\n' %(f[1], img.width, img.height))
dstf.write('\n\n')

for f in png_files:
    print(f[0])
    img = Image.open(f[0])
    buffer = getBitmap(img)
    dstf.write('const unsigned char ImageResource_%s_%dx%d[%d] = {\n\t' %(f[1], img.width, img.height, len(buffer)))
    cnt = 0
    for byte in buffer:
        dstf.write('0x%02X, ' %byte)
        cnt += 1
        if cnt == 15:
            cnt = 0
            dstf.write('\n\t')
    dstf.write('};\n\n')

dstf.write('\n\n#endif\n\n')
dstf.close()

shutil.copy('./ImageResource.h', r'C:\Users\tongy\Dropbox\PlatformIO\M5Paper_FactoryTest\src\resources')

