#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include "stdint.h"
#include <iostream>

class Client
{
private:
	SOCKET ClientSocket;

	int SendDataTCP(SOCKET sock, const char* msg, uint16_t length)
	{
		int result;
		int bytesSent = 0;

		while (bytesSent < length)
		{
			result = send(sock, (const char*)msg + bytesSent, length - bytesSent, 0);

			if (result <= 0)
				return result;

			bytesSent += result;
		}

		return bytesSent;
	}

	int RecvDataTCP(SOCKET sock, char* buffer, int length)
	{
		int total = 0;
		do
		{
			int result = recv(sock, buffer + total, length - total, 0);
			if (result < 1)
				return result;
			else
				total += result;

		} while (total < length);

		return total;
	}

public:
	Client(std::string address, uint16_t port);
	void stop();

};