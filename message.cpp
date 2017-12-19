#include "message.h"

RTMP::Message::Message( CSID id, MessageTypeID tpid,Timestamp tm, Buffer bd, MessageID streamid)
			: _csid(id),_body(bd)
{
    _header._typeid = tpid;
    _header._payloadlength = bd.length;
    _header._timestamp = tm;
    _header._streamid = streamid;

}


