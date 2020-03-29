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
	//--用socket API建立简易TCP客户端
	//1.建立一个socket 
	//2.连接服务器connect
	//3.接收服务器信息recv
	//4.关闭套接字closesocket
	//-- 用socket api建立简易tcp服务器
	//1. 建立一个socket 
	//2. bind绑定用于接收客户度连接的网络端口
	//3. listen监听网络端口
	//4. accept等待接收客户端连接
	//5. send向客户端发送一条数据
	//6. 关闭套接字closesocket
	//---------
	//清除windows socket环境

	WSACleanup();

	return 0;
}