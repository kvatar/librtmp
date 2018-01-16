#include "recvengine.h"
#include "log.h"
#include "basic.h"
using namespace std;

using namespace RTMP;

#define DEFAULT_RECV_CHUNK_SIZE		128

RTMP::RecvEngine::RecvEngine(int sockfd,int queueMaxSZ)
    :_chunkSize(DEFAULT_RECV_CHUNK_SIZE),_sockfd(sockfd),
    _beginThread(false),_ResQueue(queueMaxSZ),
    _recvEngineThread(std::bind(&RTMP::RecvEngine::ThreadFun,this),"RecvEngineThread") 
{
    _ChunkStreamVector.resize(10);
    LOG_INFO << "RecvEngine init over";
}

std::shared_ptr<Message> RTMP::RecvEngine::RecvMessage()
{
    if(_beginThread)
        return RecvMessageByThread();
    else
        return RecvMessageSimple();
}

void RTMP::RecvEngine::BeginThread()
{
    _beginThread = true;
    _recvEngineThread.start();
}

std::shared_ptr<Message> RTMP::RecvEngine::RecvMessageSimple()
{
    std::shared_ptr<Message> msg;
    while(!(msg = RecvOneChunk()));
    return msg;
}

std::shared_ptr<Message> RTMP::RecvEngine::RecvMessageByThread()
{
    return _ResQueue.take();
}

void RTMP::RecvEngine::ThreadFun()
{
    while(1)
    {
        std::shared_ptr<Message> msg;
        while(!(msg = RecvOneChunk()));
        _ResQueue.put(msg);
    }
}



std::shared_ptr<Message> RTMP::RecvEngine::RecvOneChunk()
{
    std::vector<char> mm;
    mm.resize(MAX_CHUNK_HEAD_SIZE + _chunkSize);
    char *buffer = &*mm.begin();
    ReadN(_sockfd,buffer,1);
    int fmt = (buffer[0] & 0xc0) >> 6;
    int csid = (buffer[0] & 0x3f);
    
    if(csid == 0)
    {
        ReadN(_sockfd,buffer,1);
        csid = buffer[0];
        csid += 64;
    }
    else if(csid == 1)
    {
        ReadN(_sockfd,buffer,2);
        int tmp = (buffer[0] << 8) + buffer[0];
        csid = tmp + 64;
    }

    switch(fmt)
    {
    case 0:
         ReadN(_sockfd,buffer,11);
         break;
    case 1:
         ReadN(_sockfd,buffer,7);
         break;
    case 2:
         ReadN(_sockfd,buffer,3);
         break;
    case 3:
         break;
    }
    
    std::shared_ptr<Message> &_lastMsg = _ChunkStreamVector[csid]._lastMsg;
    Timestamp &_lastChunkTime = _ChunkStreamVector[csid]._lastChunkTime;
    Timestamp &_lastMessageTime = _ChunkStreamVector[csid]._lastMessageTime;
    bool &_isAbsOfLastChunkTime = _ChunkStreamVector[csid]._isAbsOfLastChunkTime;
    uint32_t &_readSz = _ChunkStreamVector[csid]._readSz;
    if(!_lastMsg)
    {
        if(fmt != 0)
        {
            LOG_ERROR << "first chunk fmt != 0";
            return false;
        }
        _lastMsg.reset(new Message);
        _lastMsg->_csid = csid;
    }
    else
        assert(_lastMsg->_csid == csid);
    bool isAbsTimestamp = (fmt == 0) || (fmt == 3 && _isAbsOfLastChunkTime);
    bool Exist_Extended_Time = false;
    bool isFirstChunkOfMessage = _readSz == 0;
    std::vector<char>::iterator it = mm.begin();
    Timestamp tm = 0;

    if(fmt == 3)
        tm = _lastChunkTime;
    if(fmt <= 2)
    {
        tm = DecodeInt24(it);
        it += 3;
    }
    if(fmt <= 1)
    {
        _lastMsg->_header._payloadlength = DecodeInt24(it);
        it += 3;
        _lastMsg->_header._typeid = *it;
        ++it;
    }
    if(fmt == 0)
        _lastMsg->_header._streamid = DecodeInt32(it);
    _lastChunkTime = tm;
    if(tm == 0xffffff)
    {
         ReadN(_sockfd,buffer,4);
         tm = DecodeInt32(buffer);
    }
    if(isAbsTimestamp)
        _lastMsg->_header._timestamp = tm;
    else
        _lastMsg->_header._timestamp = _lastMessageTime + tm;   //fix for circulatory Timestamp
    _isAbsOfLastChunkTime = isAbsTimestamp;

    if(isFirstChunkOfMessage)
        _lastMsg->_body.reserve(_lastMsg->_header._payloadlength);

    std::string::iterator bbegin = _lastMsg->_body.begin() + _readSz;
    int bodylen = _lastMsg->_header._payloadlength - _readSz;
    int rdsz = bodylen < _chunkSize ? bodylen : _chunkSize;
    bool isover = (bodylen <= _chunkSize);
    std::vector<char> bodybuff;      //fix to no copy
    bodybuff.resize(rdsz);
    ReadN(_sockfd,&*bodybuff.begin(),rdsz);
    std::copy(bodybuff.begin(),bodybuff.begin() + rdsz,back_inserter(_lastMsg->_body));
    _readSz += rdsz;

    if(isover)
    {
        _readSz = 0;
        _lastMessageTime = _lastMsg->_header._timestamp;
    }
    LOG_INFO << "RecvOneChunk : " << rdsz << "bytes,isover:" << isover;
    
    if(isover)
        return std::shared_ptr<Message>(new Message(std::move(*_lastMsg)));//fix to provide special construction to only move body
    else
        return std::shared_ptr<Message>(new Message) ;
}









