#ifndef _stdio_fl_h_
#define _stdio_fl_h_

#include "Config.h"

#include <stdio.h>

#if OS_ALVA == OS_Windows
#define SafeParam(param, size) param, size
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define SafeParam(param, size) param
#endif

#if OS_ALVA == OS_Windows
#define fopen_fl(pfile, path, mode)   fopen_s(pfile, path, mode)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define fopen_fl(pfile, path, mode)   (*pfile) = fopen(path, mode)
#endif

#if OS_ALVA == OS_Windows
#define freopen_fl(newstreamptr, filename, mode, stream)   freopen_s(newstreamptr, filename, mode, stream)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define freopen_fl(newstreamptr, filename, mode, stream)   (*newstreamptr)= freopen(filename, mode, stream)
#endif

#if OS_ALVA == OS_Windows
#define tmpfile_fl(stream)   tmpfile_s(stream)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define tmpfile_fl(stream)   (*stream)=tmpfile()
#endif

#if OS_ALVA == OS_Windows
#define tmpnam_fl(ptr, sizeInChars)   tmpnam_s(ptr, sizeInChars)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define tmpnam_fl(ptr, sizeInChars)   tmpnam(ptr)
#endif

#if OS_ALVA == OS_Windows
#define fread_fl(buffer, buffersize, elementsize, count, pfile)   fread_s(buffer, buffersize, elementsize, count, pfile)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define fread_fl(buffer, buffersize, elementsize, count, pfile)   fread(buffer, buffersize, count, pfile)
#endif

#if OS_ALVA == OS_Windows
#define gets_fl(buffer, count)   gets_s(buffer, count)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define gets_fl(buffer, count)   gets(buffer)
#endif

#if OS_ALVA == OS_Windows
#define  sprintf_fl(temp, size, ...)   sprintf_s(temp,  size, __VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define  sprintf_fl(temp, size, ...)   sprintf(temp, __VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define   swprintf_fl(temp, size, ...)    swprintf_s(temp,  size, __VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define   swprintf_fl(temp, size, ...)    swprintf(temp, __VA_ARGS__)
#endif
 
#if OS_ALVA == OS_Windows
#define sscanf_fl(str, ...)   sscanf_s(str, __VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define sscanf_fl(str, ...)	  sscanf(str, __VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define swscanf_fl(str, ...)  swscanf_s(str, __VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define swscanf_fl(str, ...)  swscanf(str, __VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define fwscanf_fl(pfile, ...)   fwscanf_s(pfile, __VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define fwscanf_fl(pfile, ...)   fwscanf(pfile, __VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define fscanf_fl(pfile, ...)  fscanf_s(pfile, __VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define fscanf_fl(pfile, ...)  fscanf(pfile, __VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define fprintf_fl(pfile, ...)  fprintf_s(pfile, __VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define fprintf_fl(pfile, ...)  fprintf(pfile, __VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define fwprintf_fl(pfile, ...)  fwprintf_s(pfile, __VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define fwprintf_fl(pfile, ...)  fwprintf(pfile, __VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define printf_fl(...)   printf_s(__VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define printf_fl(...)   printf(__VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define wprintf_fl(...)  wprintf_s(__VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define wprintf_fl(...)  wprintf(__VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define scanf_fl(...)  scanf_s(__VA_ARGS__)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define scanf_fl(...)  scanf(__VA_ARGS__)
#endif

#if OS_ALVA == OS_Windows
#define wscanf_fl(...)  wscanf_s(__VA_ARGS__ )
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define wscanf_fl(...)  wscanf(__VA_ARGS__)
#endif
////////////////////////////////////////////////////////////////////////////
#if OS_ALVA == OS_Windows
#define vprintf_fl(format, vlist)   vprintf_s(format, vlist)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define vprintf_fl(format, vlist)   vprintf(format, vlist)
#endif

#if OS_ALVA == OS_Windows
#define vwprintf_fl(format, vlist)   vwprintf_s(format, vlist)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define vwprintf_fl(format, vlist)   vwprintf(format, vlist)
#endif

#if OS_ALVA == OS_Windows
#define vfprintf_fl(stream, format, vlist)   vfprintf_s(stream, format, vlist)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define vfprintf_fl(stream, format, vlist)   vfprintf(stream,format, vlist)
#endif

#if OS_ALVA == OS_Windows
#define vfwprintf_fl(stream, format, vlist)   vfwprintf_s(stream, format, vlist)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define vfwprintf_fl(stream, format, vlist)   vfwprintf(stream,format, vlist)
#endif

#if OS_ALVA == OS_Windows
#define vswprintf_fl(buffer, bufsz, format, vlist)   vswprintf_s(buffer, bufsz, format, vlist)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define vswprintf_fl(buffer, bufsz, format, vlist)   vswprintf(buffer, bufsz, format, vlist)
#endif

#if OS_ALVA == OS_Windows
#define vsprintf_fl(buffer, bufsz, format, vlist)   vsprintf_s(buffer, bufsz, format, vlist)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define vsprintf_fl(buffer, bufsz, format, vlist)   vsprintf(buffer, format, vlist)
#endif

#if OS_ALVA == OS_Windows
#define vsnprintf_fl(buffer, bufsz,count, format, vlist)   vsnprintf_s(buffer, bufsz, count, format, vlist)
#endif
#if OS_ALVA == OS_Android || OS_ALVA == OS_iOS
#define vsnprintf_fl(buffer, bufsz,count, format, vlist)   vsnprintf(buffer, bufsz, format, vlist)
#endif

#endif