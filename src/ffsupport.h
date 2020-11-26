#ifndef __FFSUPPORT_H
#define __FFSUPPORT_H

#include "SD.h"
#include "SPIFFS.h"

#define FT_FILE     fileclass_t
#define ft_fclose   ffsupport_fclose
#define ft_fopen    ffsupport_fopen
#define ft_fread    ffsupport_fread
#define ft_fseek    ffsupport_fseek
#define ft_ftell    ffsupport_ftell
#define ft_sprintf  sprintf


typedef struct
{
    File _fstream;
}fileclass_t;

void ffsupport_setffs(fs::FS &ffs);
fileclass_t* ffsupport_fopen(const char *Filename, const char *mode);
void  ffsupport_fclose(fileclass_t *stream);
size_t  ffsupport_fread(void *ptr, size_t size, size_t nmemb, fileclass_t *stream);
int  ffsupport_fseek(fileclass_t *stream, long int offset, int whence);
long int  ffsupport_ftell(fileclass_t *stream);

#endif