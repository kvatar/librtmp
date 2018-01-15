#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>    
#include <sys/types.h>    
#include <stdint.h>
#include <sys/socket.h>  
#include <arpa/inet.h> 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/times.h>
#include "./src/amf_librtmp.h"   
#include "./src/sendengine.h"
#include "./src/log.h"


#define MAX_CHUNK_HEADER 18
#define CHUNK_TYPE_LARGE 0

static char *ip = "10.20.6.90";

using namespace RTMP;
//usr for connect
#define SAVC(x)	static const AVal av_##x = AVC(#x)
#define AVC2(str)	{str,str == NULL ? 0 : strlen(str)}
#define SAVC2(x)	const AVal av2_##x = AVC2(x)

SAVC(app);
SAVC(connect);
SAVC(flashVer);
SAVC(swfUrl);
SAVC(pageUrl);
SAVC(tcUrl);
SAVC(fpad);
SAVC(capabilities);
SAVC(audioCodecs);
SAVC(videoCodecs);
SAVC(videoFunction);
SAVC(objectEncoding);
SAVC(secureToken);
SAVC(secureTokenResponse);
SAVC(type);
SAVC(nonprivate);



int getsocket()
{
	struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr)); //把一段内存区的内容全部设置为0
    
    //创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }
 
    //设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if (inet_aton(ip, &server_addr.sin_addr) == 0) //服务器的IP地址来自程序的参数
    {
        printf("Server IP Address Error! \n");
        exit(1);
    }
 
    server_addr.sin_port = htons(1935);
    socklen_t server_addr_length = sizeof(server_addr);
    // 向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if (connect(client_socket, (struct sockaddr*) &server_addr,
            server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", ip);
        exit(1);
    }
   
    return client_socket;
}

void _EncodeInt24(char *output, int nVal)
{
	output[2] = nVal & 0xff;
	output[1] = nVal >> 8;
	output[0] = nVal >> 16;
}

void _EncodeInt32LE(char *output, int nVal)
{
  output[0] = nVal;
  nVal >>= 8;
  output[1] = nVal;
  nVal >>= 8;
  output[2] = nVal;
  nVal >>= 8;
  output[3] = nVal;
}


//得到chunk头
int GetChunkHeader(char *ptr,int csid,int timestamp,int bodysz,uint8_t msgtypeid,int msgstreamid)//ptr长度应大于MAX_CHUNK_HEADER字节,
{
	//basic header 
	//暂时只实现chunk_type = large
	*ptr = (CHUNK_TYPE_LARGE << 6);
	*ptr |= csid;
	ptr++;
	//message header
	_EncodeInt24(ptr,timestamp);
	ptr += 3;
	_EncodeInt24(ptr,bodysz);
	ptr += 3;
	*ptr++ = msgtypeid;
	_EncodeInt32LE(ptr,msgstreamid);
	//Extended Time Stamp 
	
	return 12;
}

void OpenAndSaveToFile(char *file,char *mm,int len)
{
	int filefd = open(file,O_WRONLY|O_CREAT|O_EXCL,0600);
	if(filefd == -1 && EEXIST == errno)
	{
		if(0 != remove(file))
			printf("SaveFile remove error\n");
		filefd = open(file,O_WRONLY|O_CREAT,0600);
	}
	if(-1 == write(filefd,mm,len))
		printf("SaveFile write error\n");
	close(filefd);
}

static int clk_tck = 0;


uint32_t _RTMP_GetTime()
{
  struct tms t;
  if (!clk_tck) clk_tck = sysconf(_SC_CLK_TCK);
  return times(&t) * 1000 / clk_tck;
}


int _ReadN(int sockfd, std::string buffer, int n)
{
    if(buffer.length() < n)
    {
        LOG_ERROR << "ReadN Size Overflow";
        return 0;
    }
	char *ptr = const_cast<char *>(buffer.c_str());
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

int main(int argc, char* argv[])
{
    RTMP::LogInit(argv[0]);

	int sockfd = getsocket();
	printf("sockfd connect success : fd = %d\n",sockfd);
	
	//握手阶段
	{
		char *sendbuff = (char *)malloc(1536 + 1);
		char recvbuff[1537];
		memset(recvbuff,0,1537);
		memset(sendbuff,0,1537);
		char *psend = sendbuff;
		*psend++ = 0x03;		//C0
		//C1的时间变为全1
		int uptime = htonl(_RTMP_GetTime());
		memcpy(psend, &uptime, 4);
		psend += 4;
		memset(psend,2,1537 - 5);	//C1的随机值变为全2
		//send C0 + C1
		send(sockfd,sendbuff,1537,0);
		//recv S0 + S1 + S2
		recv(sockfd,recvbuff,1537,0);		//接受S0 + S1
		send(sockfd,recvbuff + 1,1536,0);	//发送C2
		recv(sockfd,recvbuff,1536,0);		//接受S2
		free(sendbuff);
		printf("handshake success\n");
	}
	LOG_INFO << "connect complete";
	
	//connect
	{
		char *ptr = (char *)malloc(1024);
		char *pend = ptr + 1024;
		char *p = ptr;
		//command object
		p = AMF_EncodeString(p, pend, &av_connect);
		p = AMF_EncodeNumber(p, pend, 1);
		
		char *app = "123";
		char *flashVer = NULL;//"FMSc/1.0";
		char *swfUrl = NULL;
		char *tcUrl = "rtmp://10.20.6.90:1935/123";
		double audioCodecs = 3191;
		double videoCodecs = 252;
		char *pageUrl = NULL;
		*p++ = AMF_OBJECT;
		
		SAVC2(app);
		SAVC2(flashVer);
		SAVC2(swfUrl);
		SAVC2(tcUrl);
		SAVC2(pageUrl);
		
		if (app)	
			p = AMF_EncodeNamedString(p, pend, &av_app, &av2_app);
		//?type librtmp这有个type协议上没有?//
		if (tcUrl)
			p = AMF_EncodeNamedString(p, pend, &av_tcUrl, &av2_tcUrl);
		p = AMF_EncodeNamedBoolean(p, pend, &av_fpad, FALSE);
		p = AMF_EncodeNamedNumber(p, pend, &av_capabilities, 15.0);//协议上没有
		if (audioCodecs != -1)
			p = AMF_EncodeNamedNumber(p, pend, &av_audioCodecs, audioCodecs);
		if (videoCodecs != -1)
			p = AMF_EncodeNamedNumber(p, pend, &av_videoCodecs, videoCodecs);
		if (flashVer)
			p = AMF_EncodeNamedString(p, pend, &av_flashVer, &av2_flashVer);
		if (swfUrl)
			p = AMF_EncodeNamedString(p, pend, &av_swfUrl, &av2_swfUrl);
		p = AMF_EncodeNamedNumber(p, pend, &av_videoFunction, 1.0);//协议上没有
		if (pageUrl)
			p = AMF_EncodeNamedString(p, pend, &av_pageUrl, &av2_pageUrl);
		//p = AMF_EncodeNamedNumber(p, pend, &av_objectEncoding, 0);
		if (p + 3 >= pend)
			return FALSE;
		*p++ = 0;
		*p++ = 0;			/* end of object - 0x00 0x00 0x09 */
		*p++ = AMF_OBJECT_END;
		//协议下面还有个“可选的用户变量”。不知道有什么用
		
		std::shared_ptr<Message> msg(new Message(3,20,0,std::string(ptr,p - ptr),0,"connect"));
	    SendEngine SEngine(sockfd);
        //SEngine.BeginThread();
        SEngine.SendMessage(msg);

        std::string recvbuff;
        recvbuff.resize(1024);

		_ReadN(sockfd,recvbuff,16);


		
	}
	
	
	return 0;	
}























	
