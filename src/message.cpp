#include "message.h"

RTMP::Message::Message( CSID id, MessageTypeID tpid,Timestamp tm, std::string bd, MessageID streamid,std::string nm)
			: _csid(id),_body(bd),_sendSz(0),_timeSend(0),_name(nm)
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
