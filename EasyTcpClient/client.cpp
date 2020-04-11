#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>


struct DataPackage
{
	int age;
	char name[32];

};

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//--------------------
	//--用socket API建立简易TCP客户端
	//1.建立一个socket 
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock)
	{
		std::cout << "ERRPR，建立套接字失败" << std::endl;
	}
	else
	{
		std::cout << "建立socket 成功" << std::endl;
	}
	//2.连接服务器connect
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("192.168.3.3");
	int ret = connect(sock, (sockaddr*)& sin, sizeof(sin));
	if (SOCKET_ERROR == ret)
	{
		std::cout << "错误，连接失败" << std::endl;
	}
	else
	{
		std::cout << "连接服务器成功" << std::endl;
	}

	
	while (true)
	{
		//3.输入请求命令
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;

		//4.处理请求命令
		if (0 == strcmp(cmdBuf, "exit"))
		{
			std::cout << "收到退出命令" << std::endl;
			break;
		}
		else
		{
			//5.向服务器发送请求命令 
			send(sock, cmdBuf, sizeof(cmdBuf) + 1, 0);
		}

		//6.接收服务器信息recv
		char recvBuf[256] = {};
		int nLen = recv(sock, recvBuf, 256, 0);
		if (nLen > 0)
		{
			DataPackage* dp = (DataPackage *)recvBuf;
			std::cout << "接收到数据  名字:" << dp->name  << "    年龄:"  << dp->age << std::endl;
		}

	}

	
	//7.关闭套接字closesocket
	closesocket(sock);
	WSACleanup();
	std::cout << "客户端已退出，任务结束" << std::endl;
	getchar();
	return 0;
}