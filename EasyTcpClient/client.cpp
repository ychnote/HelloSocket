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
		else if(0 == strcmp(cmdBuf, "Login"))
		{
			char userName[32] = {};
			char password[32] = {};
			std::cout << "请输入用户名:" << std::endl;
			std::cin >> userName;
			std::cout << "请输入密:" << std::endl;
			std::cin >> password;
			DataHeader header = {};
			header.cmd = CMD_LOGIN;
			header.dataLenght = sizeof(Login);
			send(sock, (const char *)&header, sizeof(DataHeader), 0);
			Login log = {};
			strcpy_s(log.userName, userName);
			strcpy_s(log.password, password);
			send(sock, (const char*)&log, sizeof(Login), 0);

			recv(sock, (char*)&header, sizeof(DataHeader), 0);

			LoginResult result = {};
			recv(sock, (char *)&result, sizeof(LoginResult), 0);
			if (result.result == 0)
			{
				std::cout << "登录成功" << std::endl;
				std::cin >> cmdBuf;
				if (0 == strcmp(cmdBuf, "LogOut")) 
				{
					header.cmd = CMD_LOGOUT;
					header.dataLenght = sizeof(LogOut);
					send(sock, (const char *)&header, sizeof(DataHeader), 0);
					LogOut out = {};
					strcpy_s(out.usrName, userName);
					send(sock, (const char *) &out, sizeof(LogOut), 0);
					
					recv(sock, (char *) &header, sizeof(DataHeader), 0);
					LogOutResult logOutResult = {};
					recv(sock, (char *)&logOutResult, sizeof(LogOutResult), 0);
					if (0 == logOutResult.result)
					{
						std::cout << "userName:" << userName << "登出成功" << std::endl;
					}
					else 
					{
						std::cout << "登出失败" << std::endl;
					}
				}
			}
			else 
			{
				std::cout << "登录失败" << std::endl;
			}



		}

		//6.接收服务器信息recv
	

	}

	
	//7.关闭套接字closesocket
	closesocket(sock);
	WSACleanup();
	std::cout << "客户端已退出，任务结束" << std::endl;
	getchar();
	return 0;
}