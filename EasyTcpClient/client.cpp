#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>


enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
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



int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//--------------------
	//--��socket API��������TCP�ͻ���
	//1.����һ��socket 
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock)
	{
		std::cout << "ERRPR�������׽���ʧ��" << std::endl;
	}
	else
	{
		std::cout << "����socket �ɹ�" << std::endl;
	}
	//2.���ӷ�����connect
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("192.168.3.3");
	int ret = connect(sock, (sockaddr*)& sin, sizeof(sin));
	if (SOCKET_ERROR == ret)
	{
		std::cout << "��������ʧ��" << std::endl;
	}
	else
	{
		std::cout << "���ӷ������ɹ�" << std::endl;
	}

	
	while (true)
	{
		//3.������������
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;

		//4.������������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			std::cout << "�յ��˳�����" << std::endl;
			break;
		}
		else if(0 == strcmp(cmdBuf, "Login"))
		{
			char userName[32] = {};
			char password[32] = {};
			std::cout << "�������û���:" << std::endl;
			std::cin >> userName;
			std::cout << "��������:" << std::endl;
			std::cin >> password;;
			Login log = {};
			strcpy_s(log.userName, userName);
			strcpy_s(log.password, password);
			send(sock, (const char*)&log, sizeof(Login), 0);

			LoginResult result = {};
			recv(sock, (char *)&result, sizeof(LoginResult), 0);
			if (result.result == 0)
			{
				std::cout << "��¼�ɹ�" << std::endl;
				std::cin >> cmdBuf;
				if (0 == strcmp(cmdBuf, "LogOut")) 
				{
					LogOut out = {};
					strcpy_s(out.usrName, userName);
					send(sock, (const char *) &out, sizeof(LogOut), 0);
					LogOutResult logOutResult = {};
					recv(sock, (char *)&logOutResult, sizeof(LogOutResult), 0);
					if (0 == logOutResult.result)
					{
						std::cout << "userName:" << userName << "�ǳ��ɹ�" << std::endl;
					}
					else 
					{
						std::cout << "�ǳ�ʧ��" << std::endl;
					}
				}
			}
			else 
			{
				std::cout << "��¼ʧ��" << std::endl;
			}



		}

		//6.���շ�������Ϣrecv
	

	}

	
	//7.�ر��׽���closesocket
	closesocket(sock);
	WSACleanup();
	std::cout << "�ͻ������˳����������" << std::endl;
	getchar();
	return 0;
}