import sys

try:
    filename = sys.argv[1]
except:
    exit('Please specify file path. Like "ttf2bin ./font.ttf" or "python ttf2bin.py ./font.ttf"')

try:
    f = open(sys.argv[1], 'rb')
except:
    exit('Error to open file %s.' %sys.argv[1])

data = f.read()
f.close()

d = open('binaryttf.h', 'w')
d.write('#ifndef _BINARYTTF_H_\n')
d.write('#define _BINARYTTF_H_\n\n')
d.write('const unsigned char binaryttf[%d] = { \n' %len(data))

cnt = 0
for n in data:
    d.write('0x%02X, ' %(n & 0xFF))
    cnt += 1
    if cnt == 10:
        cnt = 0
        d.write('\n')

d.write('};\n\n')
d.write('#endif')
d.close()