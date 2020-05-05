#pragma once
#ifndef _EAYSTCP_CLIENT_
#define _EAYSTCP_CLIENT_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include<Windows.h>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib");
#else 
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET) (~0)
#define SOCKET_ERROR (-1)

#endif
#include <stdio.h>
#include "MessageHeader.hpp"
#include <iostream>

using namespace std;

class EaysTcpClient
{

	SOCKET _sock;

public:
	EaysTcpClient()
	{
		_sock = INVALID_SOCKET;
	}

	virtual ~EaysTcpClient()
	{
		close();
	}


	//��ʼ��socket
	int initSocket()
	{

		//����win sock 2.����  
#ifdef _WIN32 
		WORD ver = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(ver, &data);
#endif
		if (INVALID_SOCKET != _sock) 
		{
			close();
			cout << "�ر���֮ǰ������" << endl;
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			cout << "���󣬽���socket ʧ��" << endl;
			return -1;
		}
		else
		{
			cout << "����socket�ɹ�" << endl;
			return 0;
		}


	}

	//���ӷ�����
	int connects(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock) 
		{
			initSocket();
		}
		//2���ӷ����� connect
		sockaddr_in sin = {};
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
#ifdef _WIN32
		sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else 
		sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr *)&sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			cout << "�������ӷ�����ʧ�ܡ���" << endl;
		}
		else
		{
			cout << "���ӷ������ɹ�" << endl;
		}
		return ret;
	}


	//�ر�socket
	void close()
	{
		if (_sock != INVALID_SOCKET) {
#ifdef _WIN32
			closesocket(_sock);
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}

	}


	bool onRun()
	{
		if (isRun())
		{
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);
			timeval t = { 1, 0 };
			int ret = select(_sock + 1, &fdReads, nullptr, nullptr, &t);
			if (ret < 0)
			{
				printf("select �������");
				return false;
			}

			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);
				if (-1 == recvData(_sock))
				{
					cout << "select �������" << endl;
					return false;
				}
			}

			return true;
		}
		return false;
	}

	//��������
	int recvData(SOCKET sockt)
	{
		char szRecv[1024] = {};
		int nLen = recv(sockt, szRecv, sizeof(DataHeader), 0);
		DataHeader* head = (DataHeader*)szRecv;

		if (nLen <= 0)
		{
			cout << "��������Ͽ����ӣ� ������� \n" << endl;
			return -1;
		}

		recv(sockt, szRecv + sizeof(DataHeader), head->dataLenght - sizeof(DataHeader), 0);

		onNetMsg(head);

		

	}
	
	//��Ӧ��������
	void onNetMsg( DataHeader* header)
	{

		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			
			LoginResult* loginResult = (LoginResult*)header;
			cout << "�յ���������Ϣ: CMD_LOGIN_RESULT: ���ݳ���:" << loginResult->dataLenght << " �����" << loginResult->result << endl;
			break;
		}
		case CMD_LOGOUT_RESULT:
		{
			LogOutResult* logOutResult = (LogOutResult*)header;
			cout << "�յ���������Ϣ��CMD_LOGOUT_RESULT: ���ݳ��ȣ�" << header->dataLenght << "�����" << logOutResult->result << endl;
			break;
		}
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* newUser = (NewUserJoin*)header;
			cout << "�յ���������Ϣ: CMD_NEW_USER_JOIN  ���ݳ���:" << header->dataLenght << "���:" << newUser->result << endl;
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

	int sendData(DataHeader * head)
	{
		if (isRun() && head) 
		{
		 return	send(_sock, (const char*)head, head->dataLenght, 0);
		}

		return SOCKET_ERROR;
	}

	//�Ƿ�����
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}


private: 

};
#endif