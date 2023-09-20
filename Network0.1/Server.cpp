#include "Server.h"

Server::Server(uint16_t port)
{
	//initializing

	//socket create
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
		printf("Warning: Socket Error\n");

	sockaddr_in _address;
	_address.sin_family = AF_INET;
	_address.sin_addr.S_un.S_addr = INADDR_ANY;
	_address.sin_port = htons(port);

	//bind
	int result = bind(listenSocket, (SOCKADDR*)&_address, sizeof(_address));
	if (result == SOCKET_ERROR)
		printf("Warning: Bind Error\n");

	//listen
	result = listen(listenSocket, 1);
	if (result == SOCKET_ERROR)
		printf("Warning: Listening Error\n");
	else
	{
		printf("Listening...\n");
	}

	FD_ZERO(&MasterSet);
	FD_SET(listenSocket, &MasterSet);

	//initializing Success
	printf("Initializing Success\n");

	//--------------server running---------------
	fd_set tempSet;
	while (true)
	{
		tempSet = MasterSet;
		select(0, &tempSet, NULL, NULL, 0);
		int count = tempSet.fd_count;

		for (int i = 0; i < count; i++)
		{
			// Check listening socket
			if (FD_ISSET(listenSocket, &tempSet))
			{
				SOCKET comuSocket = accept(listenSocket, NULL, NULL);
				if (comuSocket == INVALID_SOCKET)
				{
					if (WSAGetLastError() == WSAESHUTDOWN)
						printf("Warning: ShutDown Error\n");
					else

						printf("Warning: Accept Error\n");
				}
				printf("New client connected\n");
				FD_SET(comuSocket, &MasterSet);
			}
			else
			{
				// Receive from Client
				int length = 0;
				int result = RecvDataTCP(tempSet.fd_array[i], (char*)&length, 1);

				if (result == SOCKET_ERROR)
				{
					if (result == 0)
						printf("Warning: ShutDown Error\n");
					else
						printf("Warning: Conncet Error\n");

					/*shutdown(tempSet.fd_array[i], SD_BOTH);
					closesocket(tempSet.fd_array[i]);
					FD_CLR(tempSet.fd_array[i], &MasterSet);
					FD_CLR(tempSet.fd_array[i], &tempSet);
					i = 0;*/
					return;
				}
				else if (length >= 256)
				{
					printf("Warning: Parameter Error\n");
				}
				else
				{
					//recieve data
					char* buffer = new char[length];
					memset(buffer, 0, length);
					result = RecvDataTCP(tempSet.fd_array[i], buffer, length);
					if (result == SOCKET_ERROR)
					{
						if (result == 0)
							printf("Warning: ShutDown Error\n");
						else
							printf("Warning: Conncet Error\n");
					}
					else if (buffer[0] == '$')
					{
						//command
						std::string userInput = buffer;
						
						char* alluser = NULL;
						int lengthnow = 2;

						if (userInput == "$register")
						{
							userlist.push_back(userlist.size());
							auto youareuser = std::to_string(userlist.size());
							result = SendDataTCP(tempSet.fd_array[i],(char*)&lengthnow, 1);
							result = SendDataTCP(tempSet.fd_array[i], &(youareuser[0]), lengthnow);
						}
						else if (userInput == "$getlist")
						{
							
							for (size_t i = 0; i < userlist.size(); i++)
							{
								alluser += userlist[i];
							}
							lengthnow = sizeof(*alluser)/sizeof(char);
							result = SendDataTCP(tempSet.fd_array[i], (char*)&lengthnow, 1);
							result = SendDataTCP(tempSet.fd_array[i], alluser, lengthnow);
						}
						else
						{
							alluser;
							result = SendDataTCP(tempSet.fd_array[i], (char*)&length, 1);
							result = SendDataTCP(tempSet.fd_array[i], buffer, length);
						}

					}
					else
					{
						printf("Message received: ");
						printf(buffer);
						printf("\n");
						// send something preventing block
						result = SendDataTCP(tempSet.fd_array[i], (char*)&length, 1);
						result = SendDataTCP(tempSet.fd_array[i], buffer, length);
						delete[] buffer;
					}
				}
			}
		}
	}
	stop();
}


void Server::stop()
{
	for (size_t i = 0; i < MasterSet.fd_count; i++)
	{
		shutdown(MasterSet.fd_array[i], SD_BOTH);
		closesocket(MasterSet.fd_array[i]);
	}
}