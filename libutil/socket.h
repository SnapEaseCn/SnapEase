#ifndef SOCKET_H
#define SOCKET_H

#include <WinSock2.h>
#include <string>
#include <map>

#include "util.h"
#include "public.h"

enum TypeSocket { BlockingSocket, NonBlockingSocket };

#pragma comment(lib,"Ws2_32.lib")

class COMMON_EXPORT SocketClient
{
public:
	SocketClient(const std::string & host, int port);
	virtual ~SocketClient();
	SocketClient & operator=(SocketClient &);

	bool Connect();
	void Close();

	/*成功返回写入的字节数
	 * 失败返回SOCKET_ERROR：通过函数得到相应的错误码，做出相应处理*/
	int SendBytes(char* buff, int len);
	int ReceiveBytes(char* buff, int len);

protected:
	SOCKET	s_;
	int	* refCounter_;

private:
	static void Start();
	static void End();
	static int nofSockets_;

	std::string	m_host;
	int		m_port;
};

class COMMON_EXPORT SocketServer
{
public:
	SocketServer(int port, int connections = 1, TypeSocket type = BlockingSocket);
	~SocketServer();

	int ReceiveBytes(std::string ip, char* buff, int len);

	/*成功返回写入的字节数
	 * 失败返回SOCKET_ERROR：通过函数得到相应的错误码，做出相应处理*/
	int SendBytes(std::string ip, char* buff, int len);

	std::string Accept();
	int  GetClientNum(){	return(clients.size());}
	bool CloseClient(std::string ip);

private:
	SOCKET			m_SockServer;
	int				m_port;

	// 最大允许连接的客户端数量
	int		m_ConnectNum;
	std::map<std::string, SOCKET>	clients;
};

#endif