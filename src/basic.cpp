#include "basic.h"

namespace RTMP
{

bool WriteN(int sockfd, const char * buffer, int n)
{
	const char *ptr = buffer;
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
