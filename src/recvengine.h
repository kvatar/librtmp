#ifndef RECVENGINE_H
#define RECVENGINE_H

#include "basic.h"
#include "message.h"
#include <memory>
#include <vector>
#include <muduo/base/BoundedBlockingQueue.h>
#include <muduo/base/Thread.h>
namespace RTMP{


class RecvEngine
{
public:
    struct ChunkStream
    {
        std::shared_ptr<Message>        _lastMsg;
        Timestamp                       _lastChunkTime;
        Timestamp                       _lastMessageTime;
        bool                            _isAbsOfLastChunkTime;
        uint32_t                        _readSz;
        ChunkStream() : _lastChunkTime(0),_lastMessageTime(0) {}
    };
public:
    RecvEngine(int sockfd,int queueMazSZ = 20);
    RecvEngine(const RecvEngine &) = delete;
    RecvEngine(RecvEngine &&) = delete;
    RecvEngine &operator=(const RecvEngine &) = delete;
    RecvEngine &operator=(RecvEngine &&) = delete;
    ~RecvEngine() = default;

    std::shared_ptr<Message> RecvMessage();
    void BeginThread();
private:
    std::shared_ptr<Message> RecvOneChunk();
    std::shared_ptr<Message> RecvMessageSimple();
    std::shared_ptr<Message> RecvMessageByThread();

    void ThreadFun();

private:
    muduo::BoundedBlockingQueue<std::shared_ptr<Message>> _ResQueue;  //fix to support Multi-thread
    std::vector<ChunkStream>                        _ChunkStreamVector;
    int                                             _chunkSize;
    int                                             _sockfd;
    muduo::Thread                                   _recvEngineThread;
    bool                                            _beginThread;
};

}
#endif
