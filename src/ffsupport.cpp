#include "ffsupport.h"

fileclass_t fileclass;
fs::FS &fontFS = SD;

void ffsupport_setffs(fs::FS &ffs)
{
    fontFS = ffs;
}

fileclass_t* ffsupport_fopen(const char *Filename, const char *mode)
{
    fileclass._fstream = fontFS.open(Filename, mode);
    return &fileclass;
}

void ffsupport_fclose(fileclass_t *stream)
{
    stream->_fstream.close();
}

size_t ffsupport_fread(void *ptr, size_t size, size_t nmemb, fileclass_t *stream)
{
    return stream->_fstream.read((uint8_t*)ptr, size * nmemb);
}

int  ffsupport_fseek(fileclass_t *stream, long int offset, int whence)
{
    return stream->_fstream.seek(offset, (SeekMode)whence);
}

long int ffsupport_ftell(fileclass_t *stream)
{
    return stream->_fstream.position();
}
