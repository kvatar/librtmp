#include "log.h"

static std::unique_ptr<muduo::LogFile> g_logFile;


void RTMP::outputFunc(const char* msg, int len)
{
      g_logFile->append(msg, len);
}

void RTMP::flushFunc()
{
      g_logFile->flush();
}

void RTMP::LogInit(char *path)
{
    char name[256];
    strncpy(name, path, 256);
    g_logFile.reset(new muduo::LogFile(::basename(name), 200*1000));
    muduo::Logger::setOutput(RTMP::outputFunc);
    muduo::Logger::setFlush(RTMP::flushFunc);
    LOG_TRACE << "LogInit finish";
}


