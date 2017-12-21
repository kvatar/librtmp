#include <muduo/base/LogFile.h>
#include <muduo/base/Logging.h>
#include <unistd.h>
#include <memory>

static std::unique_ptr<muduo::LogFile> g_logFile;

void outputFunc(const char* msg, int len)
{
      g_logFile->append(msg, len);
}

void flushFunc()
{
      g_logFile->flush();
}

void LogInit(char *path)
{
    char name[256];
    strncpy(name, path, 256);
    g_logFile.reset(new muduo::LogFile(::basename(name), 200*1000));
    muduo::Logger::setOutput(outputFunc);
    muduo::Logger::setFlush(flushFunc);

}
