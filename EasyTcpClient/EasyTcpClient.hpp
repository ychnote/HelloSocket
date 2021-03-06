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


	//初始化socket
	int initSocket()
	{

		//启动win sock 2.环境  
#ifdef _WIN32 
		WORD ver = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(ver, &data);
#endif
		if (INVALID_SOCKET != _sock) 
		{
			close();
			cout << "关闭了之前的连接" << endl;
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock)
		{
			cout << "错误，建立socket 失败" << endl;
			return -1;
		}
		else
		{
			cout << "建立socket成功" << endl;
			return 0;
		}


	}

	//连接服务器
	int connects(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock) 
		{
			initSocket();
		}
		//2连接服务器 connect
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
			cout << "错误，连接服务器失败。。" << endl;
		}
		else
		{
			cout << "连接服务器成功" << endl;
		}
		return ret;
	}


	//关闭socket
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
				printf("select 任务结束");
				return false;
			}

			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);
				if (-1 == recvData(_sock))
				{
					cout << "select 任务结束" << endl;
					return false;
				}
			}

			return true;
		}
		return false;
	}

	//缓冲区最小单元大小
constexpr static int RECV_BUFFER_SIZE =  10240;;

// 接受缓冲区
	char _szRecv[RECV_BUFFER_SIZE] = {};
	

	//第二缓冲区消息缓冲区 
	char _szMsgBuf[RECV_BUFFER_SIZE * 10] = {}; 

	int _lastPos = 0;

	//接收数据
	int recvData(SOCKET sockt)
	{
		int nLen = recv(sockt, _szRecv, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			cout << "与服务器断开连接， 任务结束 \n" << endl;
			return -1;
		}

		//将收取的数据拷贝到消息缓冲区
		memcpy(_szMsgBuf + _lastPos, _szRecv, nLen);
		//消息缓冲区的数据尾部位置后移
		_lastPos += nLen;

		//判断小消息缓冲区的长度大于消息头DataHeader的长度
		//这时就可以知道当前消息的长度 
		while (_lastPos > sizeof(DataHeader))
		{
			//这时就可以知道当前消息的长度
			DataHeader* header = (DataHeader*)_szMsgBuf;
			//判断消息缓冲区的数据长度大于消息长度
			if (_lastPos > header->dataLenght)
			{
				//剩余未处理消息缓冲区的长度 
				int nSize = _lastPos - header->dataLenght;
				//处理网络数据
				onNetMsg(header);
				//将消息缓冲区剩余未处理的数据前移
				memcpy(_szMsgBuf, _szMsgBuf + header->dataLenght, nSize);
				//消息缓冲区的数据尾部位置前移
				_lastPos = nSize;
			}
			else
			{
				//剩余数据不够一条完整的消息
				break;
			}
		}
	}
	
	//响应网络数据
	void onNetMsg( DataHeader* header)
	{

		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			
			LoginResult* loginResult = (LoginResult*)header;
			cout << "收到服务器消息: CMD_LOGIN_RESULT: 数据长度:" << loginResult->dataLenght << " 结果：" << loginResult->result << endl;
			break;
		}
		case CMD_LOGOUT_RESULT:
		{
			LogOutResult* logOutResult = (LogOutResult*)header;
			cout << "收到服务器消息：CMD_LOGOUT_RESULT: 数据长度：" << header->dataLenght << "结果：" << logOutResult->result << endl;
			break;
		}
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* newUser = (NewUserJoin*)header;
			cout << "收到服务器消息: CMD_NEW_USER_JOIN  数据长度:" << header->dataLenght << "结果:" << newUser->result << endl;
			break;
		}
		case CMD_ERROR:
		{
			cout << "接受到错误消息" << endl;
			break;
		}
		default:
		{
			//head->cmd = CMD_ERROR;
			//head->dataLenght = 0;
			//send(sockt, (const char*)& head, sizeof(DataHeader), 0);
			cout << "无效命令 " << endl;
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

	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}


private: 

};
#endif