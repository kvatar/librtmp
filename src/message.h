#ifndef MESSAGE_H
#define MESSAGE_H


#include <functional>
#include <memory>

namespace RTMP{

typedef struct Buffer
{
	std::shared_ptr<char> begin;
	int length;
}Buffer;
	
typedef uint32_t 		ChunkStreamID;
typedef ChunkStreamID 	CSID;


class SendEngine;

//Message header 
typedef uint8_t			MessageTypeID;
typedef uint32_t		PayloadLength;
typedef uint32_t 		Timestamp;
typedef uint32_t		MessageID;



class Message
{
public:
	struct Header
	{
		MessageTypeID _typeid;
		PayloadLength _payloadlength;
		Timestamp	  _timestamp;
		MessageID	  _streamid;
	};
	
public:
//数据依靠拷贝传进库，以后可以实现一个基于shared_ptr的版本接受动态开辟的指针
	Message( CSID id, MessageTypeID tpid,Timestamp tm, Buffer bd,MessageID streamid);
	
	//virtual void FunRunAfterSend(SendEngine &sengine);
private:
	CSID 		_csid;
	Header 		_header;
	Buffer 		_body;
	
	friend class SendEngine;
	friend class SendChunkTool;
};


};


#endif
