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
		else
		{
			//5.������������������� 
			send(sock, cmdBuf, sizeof(cmdBuf) + 1, 0);
		}

		//6.���շ�������Ϣrecv
		char recvBuf[256] = {};
		int nLen = recv(sock, recvBuf, 256, 0);
		if (nLen > 0)
		{
			DataPackage* dp = (DataPackage *)recvBuf;
			std::cout << "���յ�����  ����:" << dp->name  << "    ����:"  << dp->age << std::endl;
		}

	}

	
	//7.�ر��׽���closesocket
	closesocket(sock);
	WSACleanup();
	std::cout << "�ͻ������˳����������" << std::endl;
	getchar();
	return 0;
}