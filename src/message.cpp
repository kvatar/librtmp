#include "message.h"
#include <iostream>
#include "basic.h" 

MsgBodyTag_Base::MsgBodyTag_Base(int typeid) : _typeid(typeid) {}


RTMP::Message::Message( CSID id, MessageTypeID tpid,Timestamp tm, std::string bd, MessageID streamid,std::string nm)
			: _csid(id),_body(bd),_timeSend(0),_name(nm),_purpose(NONE),_fmt(-1),_netHeaderLength(-1)
{
    _header._typeid = tpid;
    _header._payloadlength = bd.length();
    _header._timestamp = tm;
    _header._streamid = streamid;
}

MsgBodyTag_SetChunkSize::MsgBodyTag_SetChunkSize(const string &body)
    : MsgBodyTag_Base(MESSAGE_TYPE_SET_CHUNK_SIZE)
{
    _chunkSz = DecodeInt32(body.c_str());
}
Message::Message(const Message &msg)
    :_csid(msg._csid),_header(msg._header),_body(msg._body),_timeSend(msg._timeSend),
    _name(msg._name),_purpose(msg._purpose),_fmt(msg._fmt),_netHeaderLength(msg._netHeaderLength),
    _bodyTagPtr(msg.clone()) {}

uint32_t RTMP::Message::GetID() const
{
    return _timeSend;
}

const std::string &RTMP::Message::GetName() const
{
    return _name;
}


void RTMP::Message::UpdataMsgBodyTag()
{
    switch(_header._typeid)
    {
    case MESSAGE_TYPE_SET_CHUNK_SIZE:
        _bodyTagPtr = new MsgBodyTag_SetChunkSize(_body);
        break;
    default:
        std::cout << "RTMP::Message::UpdataMsgBodyTag() : Message type is not supported." << std:;endl;
        break;

    }
}


void RTMP::Message::show()
{
    std::cout << "===================Message show==================="<<std::endl;
    std::cout << "purpose : " << (_purpose == RECV ? "Recv" : 
                (_purpose == SEND ? "Send" : "NONE")) << std::endl;
    std::cout << "fmt : " << _fmt << std::endl;
    std::cout << "name : " << GetName() << std::endl;
    std::cout << "csid : " << _csid << std::endl;
    std::cout << "timestamp : " << _header._timestamp << std::endl;
    std::cout << "payload length : " << _header._payloadlength << std::endl;
    std::cout << "typeid : " << static_cast<uint32_t>(_header._typeid) << std::endl;
    std::cout << "stream id : " << _header._streamid << std::endl;
    std::cout << "net header length : " << _netHeaderLength << std::endl;
    std::cout << "total length : " << _netHeaderLength + _header._payloadlength << std::endl;
    std::cout << "==================Base show over=================="<<std::endl;
}













