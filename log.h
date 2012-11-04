// extremely simple logging facilities

#ifndef _LOG_H
#define _LOG_H

#include <string>
#include <cstdarg>
#include <cstdio>

namespace Log
{

const unsigned ALWAYS   = 0;
const unsigned ERROR    = 1;
const unsigned WARNING  = 2;
const unsigned INFO     = 3;
const unsigned VERBOSE  = 4;
const unsigned DEBUG    = 5;


void setLevel(int level);
void setOutputStream(FILE *fp);
bool setOutputFile(const std::string& filename);
void print(unsigned level, const char *format, ...);

void verbose(const char *format, ...);

}

#endif
