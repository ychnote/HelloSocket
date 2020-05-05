#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<thread>
#include <cstdlib>
#include "EasyTcpClient.hpp"

using namespace std;


void cmdThread(EaysTcpClient* client)
{
	while (true) {
		char cmdBuf[256] = {};
		cout << "请输出命令:";
		cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit"))
		{
			client->close();
			cout << "退出" << endl;
			return;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{

			Login login;
			strcpy_s(login.userName, "caihui");
			strcpy_s(login.password, "123456");
			int result = client->sendData(&login);
			cout << "发送结果:" << result << endl;
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			LogOut logout;
			strcpy_s(logout.usrName, "caihui");
			client->sendData(&logout);
		}
		else
		{
			cout << "不支持的命令" << endl;
		}
	}
}

int main()
{

	EaysTcpClient client;
	client.initSocket();
	client.connects("192.168.3.3", 4567);
	std::thread t1(cmdThread, &client);
	t1.detach();

	while (client.isRun())
	{
		client.onRun();
	}

	client.close();

	return 0;
}