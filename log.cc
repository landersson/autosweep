
#include "log.h"

namespace Log
{

static FILE*    log_fp = 0;
static unsigned log_level = INFO;

static bool   my_fp = false;

void setLevel(int level)
{
    log_level = level;
}

void setOutputStream(FILE *fp)
{
    if (my_fp && fp != 0)
    {
        fclose(fp);
    }   
    log_fp = fp;
    my_fp = true;
}

bool setOutputFile(const std::string& filename)
{
    log_fp = fopen(filename.c_str(), "w");
    if (log_fp == 0) return false;

    my_fp = true;
    return false;
}

void print(unsigned level, const char *format, ...)
{
    if (level > log_level)
        return;

    va_list ap;
    va_start(ap, format);
 
    vfprintf(log_fp, format, ap);

    va_end(ap);
}

#if 0
void info(const char *format, ...)
{
    if (level > log_level)
        return;

    va_list argptr;
    va_start(argptr, format);
    vprintln(LOG_INFO, format, argptr);
    va_end(argptr);
}



void verbose(const char *format, ...)
#endif

} // namespace log
