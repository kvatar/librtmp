#ifndef RECVENGINE_H
#define RECVENGINE_H

#include "basic.h"
#include "message.h"
#include <memory>
#include <vector>
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

    void ModifyChunkSize(uint32_t chunksz);
    std::shared_ptr<Message> RecvMessage();
private:
    std::shared_ptr<Message> RecvOneChunk();

private:
    std::vector<ChunkStream>                        _ChunkStreamVector;
    int                                             _chunkSize;
    int                                             _sockfd;
};

}
#endif
