#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//-- 用socket api建立简易tcp服务器
	//1. 建立一个socket 套接字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2. bind绑定用于接收客户度连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;  // inet_addr("192.168.1.10");
	if (SOCKET_ERROR == bind(sock, (sockaddr*)& _sin, sizeof(_sin)))
	{
		std::cout << "Error, 绑定用于接收客户端连接的网络端口失败" << std::endl;
		closesocket(sock);
	}
	else
	{
		std::cout << "绑定网络端口成功" << std::endl;
	}
	//3. listen监听网络端口
	if (SOCKET_ERROR == listen(sock, 5))
	{
		std::cout << "错误，监听网络端口失败" << std::endl;
		closesocket(sock);
	}
	else
	{
		std::cout << "监听网络端口成功" << std::endl;
	}
	//4. accept等待接收客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(sock, (sockaddr*)& clientAddr, &nAddrLen);
	if (INVALID_SOCKET == cSock)
	{
		std::cout << "错误，接收到无限客户端socket ..." << std::endl;
	}
	std::cout << "新客户端加入：IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;

	char recvBuf[128] = {};
	while (true)
	{
	
		//5.接受客户端数据
		int  nLen = recv(cSock, recvBuf, 128, 0);
		if (nLen <= 0)
		{
			std::cout << "客户端已经退出，任务结束" << std::endl;
			break;
		}
		std::cout << "收到命令:" << recvBuf << std::endl;
		
		//6.处理请求
		if (0 == strcmp(recvBuf, "getName"))
		{
			char msgBuf[] = "Hello, I'm Server";
			send(cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(recvBuf, "getAge"))
		{
			char msgBuf[] = "18岁";
			send(cSock, msgBuf, sizeof(msgBuf) + 1, 0);
		}
		else 
		{
			char msgBuff[] = "??????";
			send(cSock, msgBuff, sizeof(msgBuff) + 1, 0);
		}
		
	}
	//7. 关闭套接字closesocket
	closesocket(sock);
	//---------
	//清除windows socket环境

	WSACleanup();
	std::cout << "服务端已退出，任务结束" << std::endl;
	getchar();
	return 0;
}