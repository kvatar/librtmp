#include <string>
#include "recvengine.h"
#include "log.h"
#include "basic.h"


RTMP::RecvEngine::RecvEngine(int sockfd)
    :_chunkSize(DEFAULT_RECV_CHUNK_SIZE),_sockfd(sockfd)
{
    _ChunkStreamVector.resize(10);
    LOG_INFO << "RecvEngine init over";
}

std::shared_ptr<message> RTMP::RecvEngine::RecvMessage()
{
    
}


bool RTMP::RecvEngine::RecvOneChunk()
{
    std::string mm;
    char *buffer = const_cast<char *>(mm.c_str());
    mm.resize(MAX_CHUNK_HEAD_SIZE + _chunkSize);
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


    Message::Header &_lastHeader = _ChunkStreamVector[csid]._lastHeader;
    Timestamp _lastTime = _ChunkStreamVector[csid]._lastTime;
    int _lastFmt = _ChunkStreamVector[csid]._lastFmt;
    

    Message::Header msgHeader;
    
    if(_isFirstChunk && fmt != 0)
    {
        LOG_WARN << "fmt != 0 && _isFirstChunk";
        return false;
    }
    

    bool Exist_Extended_Time = false;
    Timestamp tm = 0;

    if(fmt == 0)
    {
        ReadN(_sockfd,buffer,11);
        std::string::iterator it = buffer.begin();
        tm = DecodeInt24(it);
        it += 3;
        if(tm == 0xffffff)
            Exist_Extended_Time = true;
        else
            msgHeader._timestamp = tm;
        
        msgHeader._payloadlength = DecodeInt24(it);
        it += 3;

        msgHeader._typeid = *it;
        ++it;

        msgHeader._streamid = DecodeInt32(it);
    }
    else if(fmt == 1)
    {
        ReadN(_sockfd,buffer,7);
        std::string::iterator it = buffer.begin();
        tm = DecodeInt24(it);
        it += 3;
        if(tm == 0xffffff)
            Exist_Extended_Time = true;
        else
            msgHeader._timestamp = (tm + _lastHeader._timestamp) % 0xffffff;
        
        msgHeader._payloadlength = DecodeInt24(it);
        it += 3;

        msgHeader._typeid = *it;

        msgHeader._streamid = _lastHeader._streamid;
    }
    else if(fmt == 2)
    {
        ReadN(_sockfd,buffer,3); 
        std::string::iterator it = buffer.begin();
        tm = DecodeInt24(it);
        it += 3;
        if(tm == 0xffffff)
            Exist_Extended_Time = true;
        else
            msgHeader._timestamp = (tm + _lastHeader._timestamp) % 0xffffff;
        
        msgHeader._payloadlength = _lastHeader._payloadlength;

        msgHeader._typeid = _lastHeader._typeid;

        msgHeader._streamid = _lastHeader._streamid;
    }
    else if(fmt == 3)
    {
        tm = _lastTime;
        if(tm == 0xffffff)
            Exist_Extended_Time = true;
        else
        {
            if(_lastFmt == 0)
                msgHeader._timestamp = tm;
            else
                msgHeader._timestamp = (tm + _lastHeader._timestamp) % 0xffffff;
        }
        msgHeader._payloadlength = _lastHeader._payloadlength;

        msgHeader._typeid = _lastHeader._typeid;

        msgHeader._streamid = _lastHeader._streamid;
    
    }
    else
    {
        LOG_ERROR << "fmt > 3";
        return false;
    }
    if(Exist_Extended_Time)
    {
         ReadN(_sockfd,buffer,4);
         msgHeader._timestamp = DecodeInt32(buffer);
    } 
    _ChunkStreamVector[csid]._lastHeader = msg;
    _ChunkStreamVector[csid]._lastTime = tm;
    _ChunkStreamVector[csid]._lastFmt = fmt;

    msg->_body.resize(msg->_header._payloadlength);

    int remain = _lastMsg->_payloadlength - _readSz;
    int sz = _readSz < _chunkSize ? _readSz : _chunkSize;
    ReadN(_sockfd,const_cast<char *>(msg->_body.c_str()),msg->_header._payloadlength);

}









