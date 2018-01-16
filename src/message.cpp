#include "message.h"
#include <iostream>

RTMP::Message::Message( CSID id, MessageTypeID tpid,Timestamp tm, std::string bd, MessageID streamid,std::string nm)
			: _csid(id),_body(bd),_timeSend(0),_name(nm)
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
    std::cout << "name : " << GetName() << std::endl;
    std::cout << "csid : " << _csid << std::endl;
    std::cout << "timestamp : " << _header._timestamp << std::endl;
    std::cout << "payloadlength : " << _header._payloadlength << std::endl;
    std::cout << "typeid : " << static_cast<uint32_t>(_header._typeid) << std::endl;
    std::cout << "stream id : " << _header._streamid << std::endl;
    std::cout << "==================Base show over=================="<<std::endl;
}

