#pragma warning(disable : 4996)
#include "Client.h"

//loopback
//address 127.0.0.1
//port 31337

Client::Client( std::string address, uint16_t port )
{
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
		printf("Warning: Socket Error\n");


	const char* addressC = address.c_str();
	sockaddr_in _address;
	_address.sin_family = AF_INET;
	_address.sin_addr.S_un.S_addr = inet_addr(addressC);
	if (_address.sin_addr.S_un.S_addr == -1)
		printf("Warning: Address Error\n");
	_address.sin_port = htons(port);

	printf("Initializing Success\n");

	int result;
	result = connect(ClientSocket, (SOCKADDR*)&_address, sizeof(_address));
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAESHUTDOWN)
		{
			printf("Warning: ShutDown Error\n");
		}
		printf("Warning: Conncet Error \n");
	}

	printf("Connect Success\n");
	
	while (true)
	{
		//user Input
		printf("Message to send: ");
		std::string clientInput;
		std::cin >> clientInput;

		// Send length
		int sendLength = clientInput.size()+1;
		int sendResult = SendDataTCP(ClientSocket, (char*)&sendLength, 1);
		if (sendResult == SOCKET_ERROR || sendResult == 0)
			printf("Warning: Conncet Error\n");
		else if (sendResult >= 256)
			printf("Warning: Parameter Error\n");

		//Send Data
		sendResult = SendDataTCP(ClientSocket, &(clientInput[0]), sendLength);
		if (sendResult == SOCKET_ERROR || sendResult == 0)
		{
			printf("Warning: Conncet Error\n");
		}

		// Receive length
		int recvLength = 0;
		int recvResult = RecvDataTCP(ClientSocket, (char*)&recvLength, 1);

		if (recvResult == SOCKET_ERROR || recvResult == 0)
			printf("Warning: Conncet Error\n");
		else if (recvLength >= 256)
			printf("Warning: Parameter Error\n");
		else
		{
			char* buffer = new char[recvLength];
			memset(buffer, 0, recvLength);
			recvResult = RecvDataTCP(ClientSocket, buffer, recvLength);
			if (recvResult == SOCKET_ERROR || recvResult == 0)
			{
				printf("Warning: Conncet Error\n");
			}
			else if (buffer[0] == '$')
			{
				std::string userInput = buffer;
				if (userInput == "$exit")
					return;
			}
			else
			{
				printf("Message received: ");
				printf(buffer);
				printf("\n");
				delete[] buffer;
			}
		}
	}

	stop();
}


void Client::stop()
{
	shutdown(ClientSocket, SD_BOTH);
	closesocket(ClientSocket);
}