#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>


int main()
{
	WORD ver = MAKEWORD(2,2);
	WSADATA data;
	WSAStartup(ver, &data);
	//--------------------
	//--��socket API��������TCP�ͻ���
	//1.����һ��socket 
	//2.���ӷ�����connect
	//3.���շ�������Ϣrecv
	//4.�ر��׽���closesocket
	//-- ��socket api��������tcp������
	//1. ����һ��socket 
	//2. bind�����ڽ��տͻ������ӵ�����˿�
	//3. listen��������˿�
	//4. accept�ȴ����տͻ�������
	//5. send��ͻ��˷���һ������
	//6. �ر��׽���closesocket
	//---------
	//���windows socket����

	WSACleanup();

	return 0;
}