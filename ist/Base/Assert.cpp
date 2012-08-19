﻿#include "istPCH.h"
#include "ist/Base.h"
#include <stdarg.h>
#ifdef __ist_env_Windows__
    #include <tchar.h>
    #include <windows.h>
#endif // __ist_env_Windows__

#ifdef __ist_enable_assert__

namespace ist {


static const int DPRINTF_MES_LENGTH  = 4096;


namespace
{
    AssertHandler s_assert_handler = 0;
    PanicHandler s_panic_handler = 0;
}

#ifdef __ist_enable_file_log__
namespace
{
    FILE *s_logfile = NULL;
}

void CloseLogFile()
{
    if(s_logfile) {
        fclose(s_logfile);
    }
}

void InitLogFile()
{
    if(!s_logfile) {
        s_logfile = fopen("log.txt", "w");
        if(s_logfile) {
            atexit(CloseLogFile);
        }
    }
}

void WriteLogFile(const char *text)
{
    InitLogFile();
    if(s_logfile) {
        fprintf(s_logfile, text);
        fflush(s_logfile);
    }
}

void WriteLogFile(const wchar_t *text)
{
    InitLogFile();
    if(s_logfile) {
        fwprintf(s_logfile, text);
        fflush(s_logfile);
    }
}
#else // __ist_enable_file_log__

#define WriteLogFile(...)

#endif // __ist_enable_file_log__


void SetAssertHandler(AssertHandler handler) { s_assert_handler = handler; }
void SetPanicHandler(PanicHandler handler)   { s_panic_handler = handler; }


void DebugPrint(const char* file, int line, const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    DebugPrintV(file, line, fmt, vl);
    va_end(vl);
}

void DebugPrintV(const char* file, int line, const char* fmt, va_list vl)
{
#ifdef __ist_env_Windows__
    char buf[DPRINTF_MES_LENGTH];
    //istsprintf(buf, "%s:%d - ", file, line);
    //::OutputDebugStringA(buf);
    //WriteLogFile(buf);
    istvsprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
    WriteLogFile(buf);
#else // __ist_env_Windows__
    vprintf(fmt, vl);
    fflush(stdout);
#endif // __ist_env_Windows__
}

void DebugPrint(const char* file, int line, const wchar_t* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    DebugPrintV(file, line, fmt, vl);
    va_end(vl);

}
void DebugPrintV(const char* file, int line, const wchar_t* fmt, va_list vl)
{
#ifdef __ist_env_Windows__
    wchar_t wbuf[DPRINTF_MES_LENGTH];
    //char buf[DPRINTF_MES_LENGTH];
    //istsprintf(buf, "%s:%d - ", file, line);
    //::OutputDebugStringA(buf);
    //WriteLogFile(buf);
    istvsprintf(wbuf, fmt, vl);
    ::OutputDebugStringW(wbuf);
    WriteLogFile(wbuf);
#else // __ist_env_Windows__
    vwprintf(fmt, vl);
    fflush(stdout);
#endif // __ist_env_Windows__
}


int DebugAssert(const char* file, int line, const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    int result = DebugAssertV(file, line, fmt, vl);
    va_end(vl);
    return result;
}

int DebugAssertV(const char* file, int line, const char* fmt, va_list vl)
{
#ifdef __ist_env_Windows__
    char buf[DPRINTF_MES_LENGTH];
    istsprintf(buf, "assertion failed %s:%d - ", file, line);
    ::OutputDebugStringA(buf);
    WriteLogFile(buf);
    istvsprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
    WriteLogFile(buf);
    DebugBreak();
#else // __ist_env_Windows__
    printf("assertion failed %s:%d - ", file, line);
    vprintf(fmt, vl);
    fflush(stdout);
    fgets(buf, _countof(buf), stdin);
    return s_assert_handler ? s_assert_handler() : 0;
#endif // __ist_env_Windows__
    return 0;
}

int DebugAssert(const char* file, int line, const wchar_t* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    int result = DebugAssertV(file, line, fmt, vl);
    va_end(vl);
    return result;
}

int DebugAssertV(const char* file, int line, const wchar_t* fmt, va_list vl)
{
#ifdef __ist_env_Windows__
    char buf[DPRINTF_MES_LENGTH];
    wchar_t wbuf[DPRINTF_MES_LENGTH];
    istsprintf(buf, "assertion failed %s:%d - ", file, line);
    ::OutputDebugStringA(buf);
    WriteLogFile(buf);
    istvsnwprintf(wbuf, _countof(wbuf), fmt, vl);
    ::OutputDebugStringW(wbuf);
    WriteLogFile(wbuf);
    DebugBreak();
#else // __ist_env_Windows__
    printf("assertion failed %s:%d - ", file, line);
    vprintf(fmt, vl);
    fflush(stdout);
    fgets(buf, _countof(buf), stdin);
    return s_assert_handler ? s_assert_handler() : 0;
#endif // __ist_env_Windows__
    return 0;
}


} // namespace ist

#endif // __ist_enable_assert__
