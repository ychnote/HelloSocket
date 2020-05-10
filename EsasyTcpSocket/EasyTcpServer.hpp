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
class EasyTcpServer
{
private:
	SOCKET _sock;
	vector<SOCKET> g_clients;
	
public:
	
	EasyTcpServer()
	{
	}

	virtual ~EasyTcpServer()
	{
		closes();
	}

	//��ʼ��socket
	void initSocket()
	{
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat); 

		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (INVALID_SOCKET == _sock)
		{
			cout << "���󣬽���socket ʧ�ܡ�����\n" << endl;
		}
		else
		{
			cout << "����Socket=<%d> �ɹ�...\n" << _sock << endl;
		}
	}
	//�󶨶˿ں� 

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
			std::cout << "Error, �����ڽ��տͻ������ӵ�����˿�ʧ��" << std::endl;
			closesocket(_sock);
		}
		else
		{
			std::cout << "������˿ڳɹ�" << std::endl;
		}
		return ret;
	}

	//�����˿ں� 
	int listens(int n)
	{
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret)
		{
			cout << "���󣬼�������˿�ʧ��" << endl;
		}
		else
		{
			cout << "��������˿ڳɹ�" << endl;
		}

		return ret;
	}

	//���տͻ�������
	int accepts()
	{
		sockaddr_in clientAddr = {};
		SOCKET _cScok = INVALID_SOCKET;
		int nAddrLen = sizeof(clientAddr); 
		_cScok = accept(_sock, (sockaddr*)& clientAddr, &nAddrLen);
		if (INVALID_SOCKET == _cScok)
		{
			cout << "���󣬽��ܵ���Ч�ͻ���SOKCET..." << endl;
		}
		else
		{
			std::cout << "�¿ͻ��˼��룺IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				NewUserJoin newUser;
				newUser.socket = g_clients[n];
				send(g_clients[n], (const char*)&newUser, sizeof(NewUserJoin), 0);
			}
			g_clients.push_back(_cScok);
		}
		return _cScok;
	}

    //�ر�Socket
	void closes()
	{
		if (_sock != INVALID_SOCKET) {
			closesocket(_sock);
			WSACleanup();
			_sock = INVALID_SOCKET;
		}
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			closesocket(g_clients[n]);
		}

	}

	//����������Ϣ
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
				FD_SET(g_clients[n], &fdRead);
			}

			timeval t = { 1, 0 };
			int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
			if (ret < 0)
			{
				closes();
				std::cout << "select �������" << std::endl;
				return;
			}

			if (FD_ISSET(_sock, &fdRead))
			{
				FD_CLR(_sock, &fdRead);
				//4. accept�ȴ����տͻ�������
				sockaddr_in clientAddr = {};
				int nAddrLen = sizeof(sockaddr_in);
				SOCKET cSock = INVALID_SOCKET;
				cSock = accept(_sock, (sockaddr*)& clientAddr, &nAddrLen);
				if (INVALID_SOCKET == cSock)
				{
					std::cout << "���󣬽��յ����޿ͻ���socket ..." << std::endl;
				}
				std::cout << "�¿ͻ��˼��룺IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
				NewUserJoin newUser;
				newUser.socket = cSock;
				sendDataAll(&newUser);
				g_clients.push_back(cSock);
			}
			//5.���ܿͻ�������
			for (size_t n = 0; n < fdRead.fd_count; n++)
			{
				if (-1 == RecvData(fdRead.fd_array[n]))
				{
					auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
					if (iter != g_clients.end()) {
						g_clients.erase(iter);
					}
				}
			}

			cout << "����ʱ�䴦��	����ҵ��" << endl;
		}

		//6.��������
	}


	//�Ƿ�����
	bool isRun()
    {
		return _sock != INVALID_SOCKET;
	}
	//�������ݴ��� ����ճ��  ��ְ� 

	int RecvData(SOCKET sockt)
	{
		char szRecv[1024] = {};
		int nLen = recv(sockt, szRecv, sizeof(DataHeader), 0);
		DataHeader* head = (DataHeader*)szRecv;

		if (nLen <= 0)
		{
			cout << "�ͻ������˳����������" << endl;
			return -1;
		}

		recv(sockt, szRecv + sizeof(DataHeader), head->dataLenght - sizeof(DataHeader), 0);
		onNetMsg(sockt, head);

		return 0;

	}

	//��Ӧ������Ϣ 
	void  onNetMsg(SOCKET sock ,DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{

			
			Login* login = (Login*)header;
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
			sendData(sock, &loginResult);
			break;
		}
		case CMD_LOGOUT:
		{
			LogOut* logOut = (LogOut*)header;
			std::cout << "�ǳ�����" << "�û���:" << logOut->usrName << "����:" << logOut->cmd << "���ݳ���:" << logOut->dataLenght << std::endl;
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

	//�������� 
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
				send(g_clients[n], (const char*)& header, header->dataLenght, 0);
			}
		}
	}

};


#endif