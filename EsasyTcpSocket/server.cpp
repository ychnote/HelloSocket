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
	//-- ��socket api��������tcp������
	//1. ����һ��socket �׽���
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//2. bind�����ڽ��տͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;  // inet_addr("192.168.1.10");
	if (SOCKET_ERROR == bind(sock, (sockaddr*)& _sin, sizeof(_sin)))
	{
		std::cout << "Error, �����ڽ��տͻ������ӵ�����˿�ʧ��" << std::endl;
		closesocket(sock);
	}
	else
	{
		std::cout << "������˿ڳɹ�" << std::endl;
	}
	//3. listen��������˿�
	if (SOCKET_ERROR == listen(sock, 5))
	{
		std::cout << "���󣬼�������˿�ʧ��" << std::endl;
		closesocket(sock);
	}
	else
	{
		std::cout << "��������˿ڳɹ�" << std::endl;
	}
	//4. accept�ȴ����տͻ�������
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET cSock = INVALID_SOCKET;
	cSock = accept(sock, (sockaddr*)& clientAddr, &nAddrLen);
	if (INVALID_SOCKET == cSock)
	{
		std::cout << "���󣬽��յ����޿ͻ���socket ..." << std::endl;
	}
	std::cout << "�¿ͻ��˼��룺IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;

	while (true)
	{
	
		//5.���ܿͻ�������
		DataHeader head{ };
		recv(cSock, (char*)& head, sizeof(DataHeader), 0);
		switch (head.cmd)
		{
		case CMD_LOGIN:
		{
		
			Login login = {};
			recv(cSock, (char*)& login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
			std::cout << "��½����"  << "�û���:" << login.userName << "����:" << login.password << "����:" << login.cmd << "���ݳ���:"  << login.dataLenght << std::endl;
			LoginResult loginResult = { };
			if ( 0 == strcmp(login.userName, "caihui") && 0 == strcmp(login.password, "123456"))
			{
				std::cout << "��¼�ɹ�" << std::endl;
				loginResult.result = 0;
			}
			else
			{
				std::cout << "��¼ʧ��" << std::endl;
				loginResult.result = -1;
			}
			send(cSock, (char*)& loginResult, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT:
		{
			LogOut logOut = {};
			recv(cSock, (char*)& logOut + sizeof(DataHeader), sizeof(LogOut) - sizeof(DataHeader), 0);
			std::cout << "�ǳ�����" << "�û���:" << logOut.usrName << "����:" << logOut.cmd << "���ݳ���:" << logOut.dataLenght << std::endl;
			LogOutResult logOutResult = { };
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
		 
		
		//6.��������
		
	}
	//7. �ر��׽���closesocket
	closesocket(sock);
	//---------
	//���windows socket����

	WSACleanup();
	std::cout << "��������˳����������" << std::endl;
	getchar();
	return 0;
}