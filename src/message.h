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
	enum PURPOSE {RECV,SEND,NONE};
public:
//数据依靠拷贝传进库，以后可以实现一个基于shared_ptr的版本接受动态开辟的指针
	Message() {}
    Message( CSID id, MessageTypeID tpid,Timestamp tm, std::string bd,MessageID streamid,std::string nm);
    Message(const Message &msg) = default;
    Message(Message &&msg) = default;
    Message &operator=(const Message &) = default;
    Message &operator=(Message &&) = default;
    virtual ~Message() = default;
    //for debug
	uint32_t GetID() const;
    const std::string &GetName() const;

    //for debug
    virtual void show();

	//virtual void FunRunAfterSend(SendEngine &sengine);
private:
	CSID 		    _csid;
	Header 		    _header;
    std::string     _body;
    uint32_t        _timeSend;  //time when SendMessage, use for differentiate priority 
    std::string     _name;
    
    //for debug
    PURPOSE         _purpose;
    int             _fmt;
    uint32_t        _netHeaderLength;

	friend class SendEngine;
	friend class RecvEngine;
};

}


#endif
