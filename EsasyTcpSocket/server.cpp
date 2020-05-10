#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "EasyTcpServer.hpp"

int main()
{
	EasyTcpServer ser;
	ser.initSocket();
	ser.binds(nullptr, 4567);
	ser.listens(5);
	while (ser.isRun())
	{
		ser.onRun();
	}

	ser.closes();

	
	return 0;
}