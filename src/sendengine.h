#ifndef SENDENGINE_H
#define SENDENGINE_H

#include "PriorityBlockingQueue.h"
#include "message.h"
#include <atomic>
#include <muduo/base/Thread.h>
namespace RTMP{

#define DEFAULT_SEND_CHUNK_SIZE		128

class SendEngine			//发送引擎
{
public:
    struct ChunkStream			//负责一个chunk stream 的发送
    {
        std::shared_ptr<Message>    _lastMsg;
        uint32_t                    _sendSz;
    };

    class CompareForMessage
    {
    private:
        bool Later(uint32_t t1,uint32_t t2)
        {
            Timestamp v1 = (t1 + 0xffffffff - t2) % 0xffffffff;
            Timestamp v2 = (t2 + 0xffffffff - t1) % 0xffffffff;
            return v1 < v2;
        }

    public:
        //msg1 priority < msg2 priority     return true;
        bool operator()(const std::shared_ptr<Message> &msg1,const std::shared_ptr<Message> &msg2)
        {
            if(msg1->_header._streamid != msg2->_header._streamid)
            {
                return msg1->_header._streamid > msg2->_header._streamid;
            }
            else
            {
                return Later(msg1->_timeSend,msg2->_timeSend);
            }
        }      
    };
public:
	SendEngine(int sockfd,int chunksz = DEFAULT_SEND_CHUNK_SIZE);
	//~SendEngine();
	void SendMessage(std::shared_ptr<Message> msg);	//别的线程调用的函数，应加个队列
    void BeginThread();
private:
	void ThreadFun();

    void SendMessageSimple(std::shared_ptr<Message> &msg);
    void AddMessageToQueue(std::shared_ptr<Message> &msg);
    bool SendOneChunk(std::shared_ptr<Message> &msg);
private:
    PriorityBlockingQueue<std::shared_ptr<Message>,CompareForMessage> _messageQueue;
	std::vector<ChunkStream>    _ChunkStreamVector;
	int                         _chunkSize;
	int                         _sockfd;
    std::atomic_uint            _timeForSend;
    muduo::Thread               _sendEngineThread;
    bool                        _beginThread;
};

};
#endif
