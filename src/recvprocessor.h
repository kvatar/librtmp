#ifndef RECVPROCESSOR_H
#define RECVPROCESSOR_H

#include "message.h"
#include <memory>
#include "recvengine.h"

namespace RTMP
{

class RecvProcessor
{
public:
    RecvProcessor(int sockfd);
    RecvProcessor(const RecvProcessor &) = delete;
    RecvProcessor(RecvProcessor &&) = delete;
    RecvProcessor &operator=(const RecvProcessor &) = delete;
    RecvProcessor &operator=(RecvProcessor &&) = delete;
    ~RecvProcessor() = default;

    void Run();
private:   
    void process(std::shared_ptr<Message> &msg);
    void process(MsgBodyTag_Base *pBodyTag);
    void process(MsgBodyTag_SetChunkSize *pBodyTag);
private:
    std::unique_ptr<RecvEngine> _recvEnginePtr;

};

}
#endif
