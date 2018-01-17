#include "basic.h"
#include <iostream>
#include <stdio.h>

namespace RTMP
{

unsigned int DecodeInt24(char * it)
{
    unsigned char * c = reinterpret_cast<unsigned char *>(it);
    unsigned int val;
    val = (c[0] << 16) | (c[1] << 8) | c[2];
    return val;
}

unsigned int DecodeInt32(char * it)
{
    unsigned char * c = reinterpret_cast<unsigned char *>(it);
    unsigned int val;
    val = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
    return val;
}

bool WriteN(int sockfd, const char * buffer, int n)
{
	const char *ptr = buffer;
	while (n > 0)
	{
		int nBytes;
		nBytes = send(sockfd, ptr, n, 0);
        std::cout << "WriteN : " << nBytes << " bytes" <<std::endl;
		if (nBytes < 0)
		{
            std::cout << "WriteN : send return < 0" << std::endl;
			break;
		}
		if (nBytes == 0)
        {
            std::cout << "WriteN : send return == 0" << std::endl;
			break;
        }
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
        std::cout << "ReadN : " << nBytes << " bytes" <<std::endl;
		if (nBytes < 0)
		{
            std::cout << "ReadN : recv return < 0" << std::endl;
			break;
		}
		if (nBytes == 0)
        {
            std::cout << "ReadN : recv return == 0" << std::endl;
			break;
        }
		n -= nBytes;
		ptr += nBytes;
    }
    
	return n == 0;
}



}
