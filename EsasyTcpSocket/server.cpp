#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>


enum CMD
{
	CMD_LOGIN, 
	CMD_LOGOUT,
	CMD_ERROR
};

struct DataHeader
{
	short dataLenght; 
	short cmd;
};

struct Login
{
	char userName[32];
	char password[32];

};

struct LoginResult
{
	int result;
};

struct LogOut
{
	char usrName[32];
};

struct LogOutResult
{
	int result;
};



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

	while (true)
	{
	
		//5.接受客户端数据
		DataHeader head{ 0 };
		recv(cSock, (char*)& head, sizeof(DataHeader), 0);
		switch (head.cmd)
		{
		case CMD_LOGIN:
		{
		
			Login login = {};
			recv(cSock, (char*)& login, sizeof(Login), 0);
			std::cout << "登陆操作"  << "用户名:" << login.userName << "密码:" << login.password << std::endl;
			LoginResult loginResult = { 0 };
			if ( 0 == strcmp(login.userName, "caihui") && 0 == strcmp(login.password, "123456"))
			{
				std::cout << "登录成功" << std::endl;
				loginResult.result = 0;
			}
			else
			{
				std::cout << "登录失败" << std::endl;
				loginResult.result = -1;
			}
			send(cSock, (char *)&head, sizeof(DataHeader), 0);
			send(cSock, (char*)& loginResult, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT:
		{
			LogOut logOut = {};
			recv(cSock, (char*)& logOut, sizeof(LogOut), 0);
			LogOutResult logOutResult = { 0 };
			send(cSock, (const char*)& head, sizeof(DataHeader), 0);
			send(cSock, (const char*)&logOutResult , sizeof(LogOutResult), 0);
			break;
		}

		default:
		{
			head.cmd = CMD_ERROR;
			head.dataLenght = 0;
			send(cSock, (const char *)&head, sizeof(DataHeader), 0);
			break;
		}
		}
		 
		
		//6.处理请求
		
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