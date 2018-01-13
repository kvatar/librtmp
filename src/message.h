#ifndef MESSAGE_H
#define MESSAGE_H

#include <functional>
#include <memory>
#include <string>
namespace RTMP{
	
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
	Message() {}
    Message( CSID id, MessageTypeID tpid,Timestamp tm, std::string bd,MessageID streamid,std::string nm);

    //for debug
	uint32_t GetID() const;
    const std::string &GetName() const;
	//virtual void FunRunAfterSend(SendEngine &sengine);
private:
	CSID 		    _csid;
	Header 		    _header;
    std::string     _body;
    uint32_t        _timeSend;  //time when SendMessage, use for differentiate priority 
    std::string     _name;


	friend class SendEngine;
};

}


#endif
