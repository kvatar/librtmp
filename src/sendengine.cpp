#include "sendengine.h"
#include <stdlib.h>
#include <memory>
#include <iostream>//
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <exception>

using namespace RTMP;

namespace RTMP{

#define MAX_CHUNK_HEAD_SIZE		18

char *TransInt24(char *output, char *outend, int nVal)
{
	if (output+3 > outend)
		return NULL;

	output[2] = nVal & 0xff;
	output[1] = nVal >> 8;
	output[0] = nVal >> 16;
	return output+3;
}

char *TransInt32(char *output, char *outend, int nVal)
{
	if(output + 4 > outend)
	    return NULL;
	output[0] = nVal;
	nVal >>= 8;
	output[1] = nVal;
	nVal >>= 8;
	output[2] = nVal;
	nVal >>= 8;
	output[3] = nVal;
	return output + 4;
}


static int WriteN(int sockfd, const char *buffer, int n)
{
	const char *ptr = buffer;
	while (n > 0)
	{
		int nBytes;
		nBytes = send(sockfd, buffer, n, 0);

		if (nBytes < 0)
		{
			n = 1;
			break;
		}

		if (nBytes == 0)
			break;

		n -= nBytes;
		ptr += nBytes;
    }

	return n == 0;
}
}

RTMP::SendChunkTool::SendChunkTool(CSID csid,SendEngine *sengine)
		: _csid(csid),_lastMsg(NULL),_sockfd(sengine->_sockfd),_sendEngine(sengine) {}

bool RTMP::SendChunkTool::AddMessage(std::shared_ptr<Message> &msg)
{
	_sendQueue.emplace(Recorder(msg));
	return true;
}

bool RTMP::SendChunkTool::Send()
{
	if(_sendQueue.empty())
		return false;
	Recorder &rec = _sendQueue.front();
	std::shared_ptr<Message> &msg = rec._msg;
	char begin[MAX_CHUNK_HEAD_SIZE + 128/*_sendEngine->_chunkSize*/];
	char *cur = begin;
	char *mmend = begin + MAX_CHUNK_HEAD_SIZE + 128/*_sendEngine->_chunkSize*/;
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
			if((msg->_header._timestamp == _lastMsg->_header._timestamp)
				|| (_lastTime == (msg->_header._timestamp + 0xffffff - _lastMsg->_header._timestamp)%0xffffff))
			fmt++;
		}
	}
	//判断basic_header多长
	int BHeaderSz = 0;//error
	if(_csid <= 65599)
		BHeaderSz = 1;
	else if(_csid <= 319)
	    BHeaderSz = 2;
	else
	    BHeaderSz = 3;
	//设置basic_header第一个字节
	char bheader1 = fmt << 6;
	switch(BHeaderSz)
    {
    case 1:
		bheader1 |= _csid;
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
		int tmp = _csid - 64;
		*cur++ = tmp & 0xff;
		if (BHeaderSz != 2)
			*cur++ = tmp >> 8;
    }
	//设置Message Header
	if(fmt < 3)
	{
		if(msg->_header._timestamp > 0xffffff)
		{
			Exist_Extended_Time = true;
			_lastTime = 0xffffff;
			cur = TransInt24(cur,mmend,_lastTime);
		}
		else
		{
			if(fmt == 0)
			{
				_lastTime = msg->_header._timestamp;
				cur = TransInt24(cur,mmend,_lastTime);
			}
			else
			{
				_lastTime = ((msg->_header._timestamp + 0xffffff - _lastMsg->_header._timestamp) % 0xffffff);
				cur = TransInt24(cur,mmend,_lastTime);
			}

		}
	}
	if(fmt < 2)
	{
		cur = TransInt24(cur,mmend,msg->_header._payloadlength);
		*cur++ = msg->_header._typeid;
	}
	if(fmt < 1)
	{
		cur = TransInt32(cur,mmend,msg->_header._streamid);
	}
	if(Exist_Extended_Time == true)
	{
		cur = TransInt32(cur,mmend,msg->_header._timestamp);
	}

	_lastMsg = msg;

	//body部分
	char *bbegin = msg->_body.begin.get() + rec._haveRead;
	int msglen = msg->_body.length - rec._haveRead;
	int chunksize = 128;//_sendEngine->_chunkSize;
	if(msglen > chunksize)
	{
		memcpy(cur,bbegin,chunksize);
		cur += chunksize;
		rec._haveRead += chunksize;
	}
	else
	{
		memcpy(cur,bbegin,msglen);
		cur += msglen;
		_sendQueue.pop();
	}
	int length = cur - begin;
	//调用网络库发送数据
	WriteN(_sockfd,begin,length);

	//
	//msg->FunRunAfterSend(*_sendEngine);

	return true;
}


RTMP::SendEngine::SendEngine(int sockfd,int chunksz)
	: _sockfd(sockfd),_chunkSize(chunksz)
{
	Tool_vector.reserve(10);
}

//
void RTMP::SendEngine::SendMessage(std::shared_ptr<Message> &msg)
{
	int csid = msg->_csid;
	SendChunkTool *tool = NULL;
	int sz = Tool_vector.size();
	if(csid >= sz)
	{
	    for(int i = sz;i <= csid;++i)
		{
            Tool_vector.push_back(SendChunkTool(i,this));
	    }
    }
    tool = &Tool_vector.at(csid);

	tool->AddMessage(msg);
}

void RTMP::SendEngine::Run()
{

}










