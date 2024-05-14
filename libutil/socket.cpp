#include "socket.h"
#include <iostream>

using namespace std;

int SocketClient::nofSockets_ = 0;

SocketClient::SocketClient(const std::string & host, int port)
{
	m_host = host;
	m_port = port;
	Start();

	/* UDP: use SOCK_DGRAM instead of SOCK_STREAM */
	s_ = socket(AF_INET, SOCK_STREAM, 0);

	if (s_ == INVALID_SOCKET)
	{
		printf("INVALID_SOCKET");
	}
	int nTimeout = 3000; /* 3秒 */
	/* 设置发送超时 */
	setsockopt(s_, SOL_SOCKET, SO_SNDTIMEO, (char *)&nTimeout, sizeof(int));
	/* 设置接收超时 */
	setsockopt(s_, SOL_SOCKET, SO_RCVTIMEO, (char *)&nTimeout, sizeof(int));

	refCounter_ = new int(1);
}

void SocketClient::Start()
{
	if (!nofSockets_)
	{
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 0), &info))
		{
			throw "Could not start WSA";
		}
	}
	++nofSockets_;
}


void SocketClient::End()
{
	WSACleanup();
}

SocketClient::~SocketClient()
{
	if (!--(*refCounter_))
	{
		Close();
		delete refCounter_;
	}

	--nofSockets_;
	if (!nofSockets_)
		End();
}


SocketClient & SocketClient::operator=(SocketClient & o)
{
	(*o.refCounter_)++;

	refCounter_ = o.refCounter_;
	s_ = o.s_;
	nofSockets_++;

	return(*this);
}


void SocketClient::Close()
{
	closesocket(s_);
}

int SocketClient::ReceiveBytes(char* buff, int len)
{
	return(recv(s_, buff, len, 0));
}


int SocketClient::SendBytes(char* buff, int len)
{
	return(send(s_, buff, len, 0));
}

bool SocketClient::Connect()
{
	hostent *he;
	if ((he = gethostbyname(m_host.c_str())) == 0)
	{
		return(false);
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_port);
	addr.sin_addr = *((in_addr *)he->h_addr);
	memset(&(addr.sin_zero), 0, 8);

	if (::connect(s_, (sockaddr *)&addr, sizeof(sockaddr)))
	{
		std::string err = strerror(WSAGetLastError());
		return(false);
	}
	return(true);
}


SocketServer::SocketServer(int port, int num, TypeSocket type)
{
	WORD	myVersionRequest;
	WSADATA wsaData;                      /* 包含系统所支持的WinStock版本信息 */
	myVersionRequest = MAKEWORD(1, 1);    /* 初始化版本 */
	int err = WSAStartup(myVersionRequest, &wsaData);
	if (!err)
	{
		printf("open socket success.\n");
	}
	else
	{
		printf("open socket fail.\n");
	}
	m_port = port;
	m_ConnectNum = num;

	/* 创建了可识别套接字 */
	m_SockServer = socket(AF_INET, SOCK_STREAM, 0);

	//设置成非阻塞模式
	unsigned long mode = 1;
	err = ioctlsocket(m_SockServer, FIONBIO, (unsigned long *)&mode);

	int	nTimeout = 3000; /* 3秒 */
	/* 设置发送超时 */
	setsockopt(m_SockServer, SOL_SOCKET, SO_SNDTIMEO, (char *)&nTimeout, sizeof(int));
	/* 设置接收超时 */
	setsockopt(m_SockServer, SOL_SOCKET, SO_RCVTIMEO, (char *)&nTimeout, sizeof(int));
	/* 需要绑定的参数 */
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	/* ip地址 */
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	/* 绑定端口 */
	addr.sin_port = htons(m_port);
	/* 将套接字绑定到指定的网络地址 */
	bind(m_SockServer, (SOCKADDR *)&addr, sizeof(SOCKADDR));
	/* 第二个参数代表能够接收的最多的连接数 */
	listen(m_SockServer, m_ConnectNum);                                       
}


SocketServer::~SocketServer()
{
	if (clients.size() > 0)
	{
		clients.clear();
	}

	WSACleanup();
	closesocket(m_SockServer);
}


std::string SocketServer::Accept()
{
	SOCKET	_sock;
	sockaddr_in	client;

	int	len = sizeof(sockaddr);
	_sock = accept(m_SockServer, (sockaddr *)&client, &len);
	std::string	info = inet_ntoa(client.sin_addr);
	if (!info.empty() && info != "204.204.204.204" && clients.find(info) == clients.end())
	{
		clients[info] = _sock;
	}
	else
		info.clear();
	return info;
}


int SocketServer::ReceiveBytes(std::string ip, char* buff, int len)
{
	if (clients.find(ip) != clients.end())
	{
		return(recv(clients[ip], buff, len, 0));
	}
	return(0);
}

/*成功返回写入的字节数
 * 失败返回SOCKET_ERROR：通过函数得到相应的错误码，做出相应处理*/
int SocketServer::SendBytes(std::string ip, char* buff, int len)
{
	if (clients.find(ip) != clients.end())
	{
		return(send(clients[ip], buff, len, 0));
	}
	return(0);
}

bool SocketServer::CloseClient(std::string ip)
{
	if (clients.find(ip) == clients.end())
		return false;
	else
	{
		closesocket(clients[ip]);
		clients.erase(ip);
		return true;
	}
}