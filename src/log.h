#ifndef RTMP_LOG_H
#define RTMP_LOG_H

#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h>
#include <unistd.h>
#include <memory>

namespace RTMP{


void outputFunc(const char* msg, int len);

void flushFunc();

void LogInit(char *path);
}

#endif
