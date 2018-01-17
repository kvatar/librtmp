#include "sendengine.h"
#include "basic.h"
#include <stdlib.h>
#include <memory>
#include <string.h>
#include <functional>
#include <algorithm>
using namespace RTMP;

#define DEFAULT_SEND_CHUNK_SIZE		128

RTMP::SendEngine::SendEngine(int sockfd)
	: _sockfd(sockfd),_timeForSend(0),_beginThread(false),
    _chunkSize(DEFAULT_SEND_CHUNK_SIZE),
    _sendEngineThread(std::bind(&RTMP::SendEngine::ThreadFun,this),"SendEngineThread")
{
	_ChunkStreamVector.resize(10);
    LOG_INFO << "SendEngine init over";
}

void RTMP::SendEngine::SendMessage(std::shared_ptr<Message> msg)
{
    LOG_INFO << "Name:" << msg->GetName() << " beginSend ";
    msg->_purpose = Message::SEND;
    if(_beginThread)
        SendMessageByThread(msg);
    else
        SendMessageSimple(msg);
}
void RTMP::SendEngine::BeginThread()
{
    _beginThread = true;
    _sendEngineThread.start();
}

void RTMP::SendEngine::SendMessageByThread(std::shared_ptr<Message> &msg)
{
    msg->_timeSend = _timeForSend++;
    LOG_INFO << "Name:" << msg->GetName() << " add in sendqueue " << "Get ID:" << msg->_timeSend;
    _messageQueue.push(msg);    
}

void RTMP::SendEngine::SendMessageSimple(std::shared_ptr<Message> &msg)
{
    while(!SendOneChunk(msg));
}

void RTMP::SendEngine::ThreadFun()
{
    while(1)
    {
        std::shared_ptr<Message> msg = _messageQueue.top();
        _messageQueue.pop();
        int csid = msg->_csid;
        int sz = _ChunkStreamVector.size();
        if(csid >= sz)
        {
            _ChunkStreamVector.resize(csid + 1);    //
        }
        bool sendover = SendOneChunk(msg);
        if(sendover == false)
        {
            _messageQueue.push(msg);
            LOG_INFO << "Name:" << msg->GetName() << " add in queue again " << "ID:" << msg->_timeSend;
        }
    }
}

bool RTMP::SendEngine::SendOneChunk(std::shared_ptr<Message> &msg)
{
    int csid = msg->_csid;
    std::shared_ptr<Message> &_lastMsg = _ChunkStreamVector[csid]._lastMsg;
    Timestamp &_lastTime = _lastMsg->_header._timestamp;
    using std::string;
    string mm;
    mm.resize(MAX_CHUNK_HEAD_SIZE + _chunkSize);
    string::iterator begin = mm.begin();
    string::iterator cur = begin;
    bool Exist_Extended_Time = false;
    //判断fmt the chunk type
    int fmt = 0;
    if(_lastMsg && (msg->_header._streamid == _lastMsg->_header._streamid))	//可能还有其他情况，如回放功能，暂时先不考虑
    {
        fmt++;
        if(msg->_header._payloadlength == _lastMsg->_header._payloadlength
            && msg->_header._typeid == _lastMsg->_header._typeid)
        {
            fmt++;
            if(msg->_header._timestamp == _lastMsg->_header._timestamp)   //don't support Timestamp delta equal 
                fmt++;
        }
    }
    //判断basic_header多长
    int BHeaderSz = 0;//error
    if(csid <= 65599)
        BHeaderSz = 1;
    else if(csid <= 319)
        BHeaderSz = 2;
    else
        BHeaderSz = 3;
    //设置basic_header第一个字节
    char bheader1 = fmt << 6;
    switch(BHeaderSz)
    {
    case 1:
        bheader1 |= csid;
        break;
    case 2:
        break;
    case 3:
        bheader1 |= 1;
        break;
    }
    *cur++ = bheader1;
    //设置其余字节
    if(BHeaderSz != 1)
    {
        int tmp = csid - 64;
        *cur++ = tmp & 0xff;
        if (BHeaderSz != 2)
            *cur++ = tmp >> 8;
    }
    //设置Message Header
    
    Timestamp tm = fmt == 0 ? msg->_header._timestamp : (msg->_header._timestamp - _lastTime);      //fix to a circulatory Timestamp
    if(fmt < 3)
    {
        if(tm >= 0xffffff)
        {
            Exist_Extended_Time = true;
            cur = EncodeingInt24(cur,mm.end(),0xffffff);
        }
        else
            cur = EncodeingInt24(cur,mm.end(),tm);
    }
    if(fmt < 2)
    {
        cur = EncodeingInt24(cur,mm.end(),msg->_header._payloadlength);
        *cur++ = msg->_header._typeid;
    }
    if(fmt < 1)
        cur = EncodeingInt32(cur,mm.end(),msg->_header._streamid);
    if(_lastMsg && Exist_Extended_Time == true)
        cur = EncodeingInt32(cur,mm.end(),tm);

    _lastMsg = msg;

    bool sendover = false;
    //body部分
    uint32_t &_sendSz = _ChunkStreamVector[csid]._sendSz;
    string::iterator bbegin = msg->_body.begin() + _sendSz;//
    int bodylen = msg->_body.length() - _sendSz;
    if(bodylen > _chunkSize)
    {
        cur = std::copy(bbegin,bbegin + _chunkSize,cur);   
        _sendSz += _chunkSize;
        sendover = false;
        LOG_INFO << "Name:" << msg->GetName() << " ID: "
            << msg->GetID() << " SendOneChunk " << _chunkSize <<
            "bytes";
    }
    else
    {
        cur = std::copy(bbegin,bbegin + bodylen,cur);
        sendover = true;
        _sendSz = 0;
        LOG_INFO << "Name:" << msg->GetName() << " ID: "
            << msg->GetID() << " Send " << bodylen << " bytes SendOver";
    }
    int length = cur - begin;
    //调用网络库发送数据
    WriteN(_sockfd,mm.c_str(),length);
    //
    //msg->FunRunAfterSend(*_sendEngine);
    //
    return sendover;
}






