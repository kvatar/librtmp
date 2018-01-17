#ifndef BASIC_H
#define BASIC_H

#include <string>
#include "log.h"
#include <sys/types.h>
#include <sys/socket.h>
namespace RTMP{

#define MAX_CHUNK_HEAD_SIZE		18



template <typename Iterator>    //iterator 1byte
Iterator EncodeingInt24(Iterator output, Iterator outend, int nVal)
{
	if (output + 3 > outend)
    {
        LOG_ERROR << "Memory access overbounds";
		return output;
    }

	output[2] = nVal & 0xff;
	output[1] = nVal >> 8;
	output[0] = nVal >> 16;
	return output+3;
}

template <typename Iterator>
Iterator EncodeingInt32(Iterator output, Iterator outend, int nVal)
{
	if(output + 4 > outend)
    {
        LOG_ERROR << "Memory access overbounds";
	    return output;
    }
	output[0] = nVal;
	nVal >>= 8;
	output[1] = nVal;
	nVal >>= 8;
	output[2] = nVal;
	nVal >>= 8;
	output[3] = nVal;
	return output + 4;
}

unsigned int DecodeInt24(char * it);

unsigned int DecodeInt32(char * it);

bool WriteN(int sockfd, const char * buffer, int n);

//for fix add a cached
bool ReadN(int sockfd, char * buffer, int n);


}


#endif
