#include "message.h"
#include <iostream>

RTMP::Message::Message( CSID id, MessageTypeID tpid,Timestamp tm, std::string bd, MessageID streamid,std::string nm)
			: _csid(id),_body(bd),_timeSend(0),_name(nm),_purpose(NONE),_fmt(-1),_netHeaderLength(-1)
{
    _header._typeid = tpid;
    _header._payloadlength = bd.length();
    _header._timestamp = tm;
    _header._streamid = streamid;
}

uint32_t RTMP::Message::GetID() const
{
    return _timeSend;
}

const std::string &RTMP::Message::GetName() const
{
    return _name;
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

