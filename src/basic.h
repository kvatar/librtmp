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

template <typename Iterator>
int DecodeInt24(Iterator it)
{
    Iterator c = it;
    int val;
    val = (c[0] << 16) | (c[1] << 8) | c[2];
    return val;
}

template <typename Iterator>
int DecodeInt32(Iterator it)
{
    Iterator c = it;
    int val;
    val = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
    return val;
}

bool WriteN(int sockfd, const char * buffer, int n)
{
	const char *ptr = buffer.c_str();
	while (n > 0)
	{
		int nBytes;
		nBytes = send(sockfd, ptr, n, 0);
		if (nBytes < 0)
		{
            LOG_WARN << "send return < 0";
			break;
		}
		if (nBytes == 0)
			break;

		n -= nBytes;
		ptr += nBytes;
    }

	return n == 0;
}

//for fix add a cached
bool ReadN(int sockfd, char * buffer, int n)
{
	char *ptr = buffer;
	while (n > 0)
	{
		int nBytes;
		nBytes = recv(sockfd, ptr, n, 0);
		if (nBytes < 0)
		{
            LOG_WARN << "send return < 0";
			break;
		}
		if (nBytes == 0)
			break;

		n -= nBytes;
		ptr += nBytes;
    }
    
	return n == 0;
}



}


#endif
