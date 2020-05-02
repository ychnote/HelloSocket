#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<vector>


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



vector<SOCKET> g_clients;

int processor(SOCKET sockt) 
{
	char szRecv[1024] = {};
	int nLen = recv(sockt, szRecv, sizeof(DataHeader), 0);
	DataHeader* head = (DataHeader*)szRecv;

	if (nLen <= 0)
	{
		cout << "�ͻ������˳����������" << endl;
		return -1;
	}

	switch (head->cmd)
	{
	case CMD_LOGIN:
	{

		recv(sockt, szRecv + sizeof(DataHeader), head->dataLenght - sizeof(DataHeader), 0);
		Login* login = (Login*)szRecv;
		std::cout << "��½����" << "�û���:" << login->userName << "����:" << login->password << "����:" << login->cmd << "���ݳ���:" << login->dataLenght << std::endl;
		LoginResult loginResult = { };
		if (0 == strcmp(login->userName, "caihui") && 0 == strcmp(login->password, "123456"))
		{
			std::cout << "��¼�ɹ�" << std::endl;
			loginResult.result = 0;
		}
		else
		{
			std::cout << "��¼ʧ��" << std::endl;
			loginResult.result = -1;
		}
		send(sockt, (char*)& loginResult, sizeof(LoginResult), 0);
		break;
	}
	case CMD_LOGOUT:
	{

		recv(sockt, szRecv + sizeof(DataHeader), head->dataLenght - sizeof(DataHeader), 0);
		LogOut* logOut = (LogOut*)szRecv;
		std::cout << "�ǳ�����" << "�û���:" << logOut->usrName << "����:" << logOut->cmd << "���ݳ���:" << logOut->dataLenght << std::endl;
		LogOutResult logOutResult = { };
		send(sockt, (const char*)& logOutResult, sizeof(LogOutResult), 0);
		break;
	}

	default:
	{
		head->cmd = CMD_ERROR;
		head->dataLenght = 0;
		send(sockt, (const char*)& head, sizeof(DataHeader), 0);
		break;
	}
	}

}

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

	while (true)
	{
	
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		FD_SET(sock, &fdRead);
		FD_SET(sock, &fdWrite);
		FD_SET(sock, &fdExp);

		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}

		timeval t = {1, 0};
		int ret =  select(sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			std::cout << "select �������" << std::endl;
			break;
		}

		if (FD_ISSET(sock, &fdRead))
		{
			FD_CLR(sock, &fdRead);
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
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				NewUserJoin newUser;
				newUser.socket = g_clients[n];
				send(g_clients[n], (const char*)& newUser, sizeof(NewUserJoin), 0);
			}
			g_clients.push_back(cSock);
		}
		//5.���ܿͻ�������
		for (size_t n = 0 ;  n < fdRead.fd_count;  n++)
		{
			if (-1 == processor(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end()) {
					g_clients.erase(iter);
				}
			}
		}
	
		cout << "����ʱ�䴦��	����ҵ��" << endl;
		
		//6.��������
		
	}

	for (size_t n = g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
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