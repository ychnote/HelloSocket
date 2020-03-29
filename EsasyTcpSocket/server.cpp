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

	char recvBuf[128] = {};
	while (true)
	{
	
		//5.���ܿͻ�������
		int  nLen = recv(cSock, recvBuf, 128, 0);
		if (nLen <= 0)
		{
			std::cout << "�ͻ����Ѿ��˳����������" << std::endl;
			break;
		}
		std::cout << "�յ�����:" << recvBuf << std::endl;
		
		//6.��������
		if (0 == strcmp(recvBuf, "getName"))
		{
			char msgBuf[] = "Hello, I'm Server";
			send(cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(recvBuf, "getAge"))
		{
			char msgBuf[] = "18��";
			send(cSock, msgBuf, sizeof(msgBuf) + 1, 0);
		}
		else 
		{
			char msgBuff[] = "??????";
			send(cSock, msgBuff, sizeof(msgBuff) + 1, 0);
		}
		
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