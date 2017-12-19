#ifndef SENDENGINE_H
#define SENDENGINE_H

#include <vector>
#include <queue>
#include "message.h"
using namespace RTMP;

namespace RTMP{

#define DEFAULT_CHUNK_SIZE		128


class SendEngine;

class SendChunkTool			//负责一个chunk stream 的发送
{
public:
	struct Recorder			//记录message在传输时的信息
	{
		Recorder(std::shared_ptr<Message> &msg) : _msg(msg),_haveRead(0) {}
		std::shared_ptr<Message> _msg;
		int _haveRead;
	};
	
public:
	//fortest
	SendChunkTool(CSID csid,int sockfd) : _csid(csid),_sockfd(sockfd),_sendEngine(NULL) {}
	void ChangeCSID(CSID id)
	{
		_csid = id; 
	}
public:
	SendChunkTool(CSID csid,SendEngine *sengine);
	bool Send();			//发送一个chunk
	bool AddMessage(std::shared_ptr<Message> &msg);
private:
	CSID _csid;
	std::shared_ptr<Message> _lastMsg;
	Timestamp _lastTime;
	std::queue<Recorder> _sendQueue;
	int _sockfd;
	SendEngine *_sendEngine;
};

class SendEngine			//发送引擎
{
public:
	SendEngine(int sockfd,int chunksz = DEFAULT_CHUNK_SIZE);
	//~SendEngine();
	void SendMessage(std::shared_ptr<Message> &msg);	//别的线程调用的函数，应加个队列
	void Run();
private:
	std::vector<SendChunkTool> Tool_vector;
	int _chunkSize;
	int _sockfd;
	
	friend class SendChunkTool;
};

};

#endif