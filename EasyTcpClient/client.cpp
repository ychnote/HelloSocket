#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<thread>


using namespace std;

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};


struct DataHeader
{
	short dataLenght;
	short cmd;
};

struct Login : public DataHeader
{
	Login()
	{
		dataLenght = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char password[32];

};

struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLenght = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin : public DataHeader
{
	int socket;
	NewUserJoin()
	{
		dataLenght = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		result = 0;
		socket = 0;
	}
	int result;
};

struct LogOut : public DataHeader
{
	LogOut()
	{
		dataLenght = sizeof(LogOut);
		cmd = CMD_LOGOUT;
	}
	char usrName[32];
};

struct LogOutResult : public DataHeader
{
	LogOutResult()
	{
		dataLenght = sizeof(LogOutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

int processor(SOCKET sockt)
{
	char szRecv[1024] = {};
	int nLen = recv(sockt, szRecv, sizeof(DataHeader), 0);
	DataHeader* head = (DataHeader*)szRecv;

	if (nLen <= 0)
	{
		cout << "与服务器断开连接， 任务结束 \n" << endl;
		return -1;
	}

	switch (head->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(sockt, szRecv + sizeof(DataHeader), head->dataLenght - sizeof(DataHeader), 0);
		LoginResult* loginResult = (LoginResult *)szRecv;
		cout << "收到服务器消息: CMD_LOGIN_RESULT: 数据长度:" << loginResult->dataLenght << " 结果：" << loginResult->result << endl;
		break;
	}
	case CMD_LOGOUT_RESULT:
	{
		recv(sockt, szRecv + sizeof(DataHeader), head->dataLenght - sizeof(DataHeader), 0);
		LogOutResult* logOutResult = (LogOutResult *)szRecv;
		cout << "收到服务器消息：CMD_LOGOUT_RESULT: 数据长度：" << head->dataLenght << "结果：" << logOutResult->result << endl;
		break;
	}
	case CMD_NEW_USER_JOIN:
	{
		recv(sockt, szRecv + sizeof(DataHeader), head->dataLenght - sizeof(DataHeader), 0 );
		NewUserJoin* newUser = (NewUserJoin*)szRecv;
		cout << "收到服务器消息: CMD_NEW_USER_JOIN  数据长度:" << head->dataLenght << "结果:" << newUser->result << endl;
		break;
	}
	default:
	{
		//head->cmd = CMD_ERROR;
		//head->dataLenght = 0;
		//send(sockt, (const char*)& head, sizeof(DataHeader), 0);
		break;
	}
	}

}


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
		std::cout << "ERRPR，建立套接字失败"  << std::endl;
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
		fd_set fdReads; 
		FD_ZERO(&fdReads);
		FD_SET(sock, &fdReads);
		timeval t = {0, 0};
		int ret = select(sock + 1, &fdReads, nullptr, nullptr, &t);
		if (ret < 0)
		{
			printf("select 任务结束");
		}

		cout << "ret" << ret << endl;

		if (FD_ISSET(sock, &fdReads))
		{
			FD_CLR(sock, &fdReads);
			if (-1 == processor(sock))
			{
				cout << "select 任务结束" << endl;
				break;
			}
		}
		cout << "空闲时间处理其它业务" << endl;
		Login login; 
		strcpy_s(login.userName, "caihui");
		strcpy_s(login.password, "123456");
		send(sock, (const char *)&login, sizeof(Login), 0);
		Sleep(1000);

	}

	
	//7.关闭套接字closesocket
	closesocket(sock);
	WSACleanup();
	std::cout << "客户端已退出，任务结束" << std::endl;
	getchar();
	return 0;
}