#ifndef MESSAGE_H
#define MESSAGE_H

#include <functional>
#include <memory>
#include <string>
namespace RTMP{

#define MESSAGE_TYPE_SET_CHUNK_SIZE     0x01


typedef uint32_t 		ChunkStreamID;
typedef ChunkStreamID 	CSID;

class SendEngine;

class MsgBodyTag_Base
{
public:
    MsgBodyTag_Base(int typeid);
    MsgBodyTag_Base(const MsgBodyTag_Base &) = default;
    MsgBodyTag_Base(MsgBodyTag_Base &&) = default;
    MsgBodyTag_Base &operator=(const MsgBodyTag_Base &) = default;
    MsgBodyTag_Base &operator=(MsgBodyTag_Base &&) = default;
    virtual MsgBodyTag_Base *clone() const = 0;
    virtual ~MsgBodyTag_Base() = 0;
public:
    int _typeid;
};

class MsgBodyTag_SetChunkSize : public MsgBodyTag_Base
{
public:
    MsgBodyTag_SetChunkSize(const string &body);
    MsgBodyTag_SetChunkSize(const MsgBodyTag_SetChunkSize &) = default;
    MsgBodyTag_SetChunkSize(MsgBodyTag_SetChunkSize &&) = default;
    MsgBodyTag_SetChunkSize &operator=(const MsgBodyTag_SetChunkSize &) = default;
    MsgBodyTag_SetChunkSize &operator=(MsgBodyTag_SetChunkSize &&) = default;
    virtual MsgBodyTag_SetChunkSize *clone() const;
    virtual ~MsgBodyTag_SetChunkSize();
public:
    uint32_t _chunkSz;
};

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
	Message() {}
    Message(CSID id, MessageTypeID tpid,Timestamp tm, std::string bd,MessageID streamid,std::string nm);
    Message(const Message &msg);
    Message(Message &&msg) = default;
    Message &operator=(const Message &) = default;
    Message &operator=(Message &&) = default;
    ~Message() = default;
    //for debug
	uint32_t GetID() const;
    const std::string &GetName() const;

    void UpdataMsgBodyTag();
    //for debug
    void show();

	//virtual void FunRunAfterSend(SendEngine &sengine);
private:
	CSID 		    _csid;
	Header 		    _header;
    std::string     _body;
    uint32_t        _timeSend;  //time when SendMessage, use for differentiate priority 
    std::string     _name;
    std::unique_ptr<MsgBodyTag_Base> _bodyTagPtr;
    
    //for debug
    PURPOSE         _purpose;
    int             _fmt;
    uint32_t        _netHeaderLength;

	friend class SendEngine;
	friend class RecvEngine;
};

}


#endif
