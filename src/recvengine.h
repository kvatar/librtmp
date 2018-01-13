#ifndef RECVENGINE_H
#define RECVENGINE_H

#include "basic.h"
#include "message.h"
#include <memory>
#include <vector>
#include <queue>
namespace RTMP{

#define DEFAULT_RECV_CHUNK_SIZE		128

class RecvEngine
{
public:
    struct ChunkStream
    {
        Message::Header             _lastHeader;
        Timestamp                   _lastTime;
        bool                        _is;
        std::string                 _readBody;
    };
public:
    RecvEngine(int sockfd);
    std::shared_ptr<message> RecvMessage();

private:
   bool RecvOneChunk();

private:
    std::queue<shared_ptr<Message>> _ResQueue;
    std::vector<ChunkStream>        _ChunkStreamVector;
    int                             _chunkSize;
    int                             _sockfd;
    bool                            _isFirstChunk;
};

}
#endif
