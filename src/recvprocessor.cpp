#include "recvprocessor.h"
#include <iostream>
using namespace RTMP;

RcvProcessor::RecvProcessor(int sockfd) 
    : _recvEnginePtr (sockfd) {}


void RcvProcessor::Run()
{
    //add Thread
    std::shared_ptr<Message> msg =  _recvEnginePtr->RecvMessage();
    process(msg);
}

void RecvProcessor::process(std::shared_ptr<Message> &msg)
{
    process(msg->_bodyTagPtr.get());
}

void RecvProcessor::process(MsgBodyTag_Base *pBodyTag)
{
    std::cout << "Message type processing is not yet supported.Message type ID : " << pBodyTag->_typeid;
}
void RecvProcessor::process(MsgBodyTag_SetChunkSize *pBodyTag)
{
    
}


