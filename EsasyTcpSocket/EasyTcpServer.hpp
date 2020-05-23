#ifndef _EASY_TCP_SERVER 
#define _EASY_TCP_SERVER
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<vector>
#include "MessageHeader.hpp"


using namespace std;

//缓冲区最小单元大小
constexpr static int RECV_BUFFER_SIZE = 10240;;
class ClientSocket
{
public:
	ClientSocket(SOCKET sockfd = INVALID_SOCKET)
	{
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, sizeof(_szMsgBuf));
		_lastPos = 0;
	}

	SOCKET sockfd()
	{
		return _sockfd;
	}

	char* msgBuf()
	{
		return _szMsgBuf;
	}

	int getLastPos()
	{
		return _lastPos;
	}

	void setLastPos(int lastPos)
	{
		_lastPos = lastPos;
	}

private:
	SOCKET _sockfd;  
	//第二缓冲区消息缓冲区 
	char _szMsgBuf[RECV_BUFFER_SIZE * 10];
	int _lastPos;
};



class EasyTcpServer
{
private:
	SOCKET _sock;
	vector<ClientSocket *> g_clients;
	
public:
	
	EasyTcpServer()
	{
	}

	virtual ~EasyTcpServer()
	{
		closes();
	}

	//初始化socket
	void initSocket()
	{
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat); 

		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (INVALID_SOCKET == _sock)
		{
			cout << "错误，建立socket 失败。。。\n" << endl;
		}
		else
		{
			cout << "建立Socket=<%d> 成功...\n" << _sock << endl;
		}
	}
	//绑定端口号 

	int binds(const char * ip, unsigned short port)
	{
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
		if (ip)
		{
			_sin.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			_sin.sin_addr.S_un.S_addr = INADDR_ANY;  // inet_addr("192.168.1.10");
		}

		int ret = bind(_sock, (sockaddr*)& _sin, sizeof(_sin));

		if (SOCKET_ERROR == ret)
		{
			std::cout << "Error, 绑定用于接收客户端连接的网络端口失败" << std::endl;
			closesocket(_sock);
		}
		else
		{
			std::cout << "绑定网络端口成功" << std::endl;
		}
		return ret;
	}

	//监听端口号 
	int listens(int n)
	{
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret)
		{
			cout << "错误，监听网络端口失败" << endl;
		}
		else
		{
			cout << "监听网络端口成功" << endl;
		}

		return ret;
	}

	//接收客户端连接
	int accepts()
	{
		sockaddr_in clientAddr = {};
		SOCKET _cScok = INVALID_SOCKET;
		int nAddrLen = sizeof(clientAddr); 
		_cScok = accept(_sock, (sockaddr*)& clientAddr, &nAddrLen);
		if (INVALID_SOCKET == _cScok)
		{
			cout << "错误，接受到无效客户端SOKCET..." << endl;
		}
		else
		{
			std::cout << "新客户端加入：IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				NewUserJoin newUser;
				newUser.socket = _cScok;
				send(g_clients[n]->sockfd(), (const char*)&newUser, sizeof(NewUserJoin), 0);
			}
			g_clients.push_back(new ClientSocket(_cScok));
		}
		return _cScok;
	}

    //关闭Socket
	void closes()
	{
		if (_sock != INVALID_SOCKET) {
			closesocket(_sock);
			WSACleanup();
			_sock = INVALID_SOCKET;
		}
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			closesocket(g_clients[n]->sockfd());
			delete g_clients[n];
		}
		g_clients.clear();

	}

	//处理网络消息
	void onRun()
	{
		if (isRun()) {
			fd_set fdRead;
			fd_set fdWrite;
			fd_set fdExp;
			FD_ZERO(&fdRead);
			FD_ZERO(&fdWrite);
			FD_ZERO(&fdExp);
			FD_SET(_sock, &fdRead);
			FD_SET(_sock, &fdWrite);
			FD_SET(_sock, &fdExp);

			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				FD_SET(g_clients[n]->sockfd(), &fdRead);
			}

			timeval t = { 1, 0 };
			int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
			if (ret < 0)
			{
				closes();
				std::cout << "select 任务结束" << std::endl;
				return;
			}

			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				//4. accept等待接收客户端连
				accepts();
			}
			//5.接受客户端数据
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(g_clients[n]->sockfd(), &fdRead))
				{
					if (-1 == RecvData(g_clients[n]))
					{
						auto iter = g_clients.begin() + n;;
						if (iter != g_clients.end())
						{
							delete* iter;
							g_clients.erase(iter);
						}
					}
				}
			}

			cout << "空闲时间处理	其它业务" << endl;
		}

		//6.处理请求
	}


	//是否工作中
	bool isRun()
    {
		return _sock != INVALID_SOCKET;
	}
	//接收数据处理 处理粘包  拆分包 

	int RecvData(ClientSocket* pClient)
	{
		char szRecv[1024] = {};
		int nLen = recv(pClient->sockfd() , szRecv, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			cout << "客户端已退出，任务结束" << endl;
			return -1;
		}

		//将收取的消息拷贝到消息缓冲区
		memcpy(pClient->msgBuf() + pClient->getLastPos(), szRecv, nLen);
	   //消息缓冲区的数据尾部位置后移
		pClient->setLastPos(pClient->getLastPos() + nLen);

		//判断消息缓冲区的数据长度大于消息头DataHeader 长度
		while (pClient->getLastPos() >= sizeof(DataHeader))
		{
			//这时就可以知道当前消息的长度
			DataHeader* header = (DataHeader*)pClient->msgBuf();
			//判断消息缓冲区的数据长度大于消息长度
			if (pClient->getLastPos() >= header->dataLenght)
			{
				//消息缓冲区剩余产未处理数据的长度
				int nSize = pClient->getLastPos() - header->dataLenght;
				onNetMsg(pClient->sockfd(), header);
				memcpy(pClient->msgBuf(), pClient->msgBuf() + header->dataLenght, nSize);
				pClient->setLastPos(nSize);
			}
			else
			{
				break;
			}
		}

		return 0;

	}

	//响应网络消息 
	void  onNetMsg(SOCKET sock ,DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{

			
			Login* login = (Login*)header;
			std::cout << "登陆操作" << "用户名:" << login->userName << "密码:" << login->password << "命令:" << login->cmd << "数据长度:" << login->dataLenght << std::endl;
			LoginResult loginResult = { };
			if (0 == strcmp(login->userName, "caihui") && 0 == strcmp(login->password, "123456"))
			{
				std::cout << "登录成功" << std::endl;
				loginResult.result = 0;
			}
			else
			{
				std::cout << "登录失败" << std::endl;
				loginResult.result = -1;
			}
			sendData(sock, &loginResult);
			break;
		}
		case CMD_LOGOUT:
		{
			LogOut* logOut = (LogOut*)header;
			std::cout << "登出操作" << "用户名:" << logOut->usrName << "命令:" << logOut->cmd << "数据长度:" << logOut->dataLenght << std::endl;
			LogOutResult logOutResult = { };
			sendData(sock, &logOutResult);
			break;
		}

		default:
		{
			header->cmd = CMD_ERROR;
			header->dataLenght = 0;
			sendData(sock, header);
			break;
		}
		}
	}

	//发送数据 
	int sendData(SOCKET sock, DataHeader * header)
	{ 
		if (isRun() && header)
		{
			return send(sock, (const char*)header, header->dataLenght, 0);
		}
		return SOCKET_ERROR;
	}
	
	void sendDataAll(DataHeader* header)
	{
		if (isRun() && header)
		{
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				send(g_clients[n]->sockfd(), (const char*)& header, header->dataLenght, 0);
			}
		}
	}

};


#endif