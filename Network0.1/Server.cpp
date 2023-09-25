#include "Server.h"

Server::Server(uint16_t port, int amount)
{
	//initializing
	// log file
	std::ofstream file;
	file.open("log.txt");
	// UDP socket create

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
		file << "Listening...\n";
	}

	FD_ZERO(&MasterSet);
	FD_SET(listenSocket, &MasterSet);

	//initializing Success
	printf("Initializing Success\n");
	file << "Initializing Success\n";
	//--------------server running---------------
	fd_set tempSet;
	while (true)
	{
		tempSet = MasterSet;
		int res = select(0, &tempSet, NULL, NULL, NULL);
		int count = tempSet.fd_count;

		if (FD_ISSET(listenSocket, &tempSet))
		{
			SOCKET comuSocket = accept(listenSocket, NULL, NULL);
			if (comuSocket == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAESHUTDOWN)
					printf("Warning: ShutDown Error\n");
				else
					printf("Warning: Accept Error\n");//here

				//shutdown(listenSocket, SD_BOTH);
				//closesocket(listenSocket);
				FD_CLR(listenSocket, &tempSet);
				printf("Client disconnected\n");
				file << "Client disconnected\n";
			}
			else
			{
				printf("New client connected\n");
				file << "New client connected\n";
				FD_SET(comuSocket, &MasterSet);
			}
		}

		for (int i = 0; i < count; i++)
		{
			// Check listening socket
			if (tempSet.fd_array[i] == listenSocket)
				continue;

			// Receive Length
			int length = 0;
			int result = RecvDataTCP(tempSet.fd_array[i], (char*)&length, 1);

			if (result == SOCKET_ERROR)
			{
				if (result == 0)
					printf("Warning: ShutDown Error\n");
				else
					printf("Warning: Receiving Error\n");

				shutdown(MasterSet.fd_array[i], SD_BOTH);
				closesocket(MasterSet.fd_array[i]);
				FD_CLR(tempSet.fd_array[i], &MasterSet);
			
				printf("Client forced quit");
				continue;
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
						printf("Warning: Receiving Error\n");
				}
				else if (buffer[0] == '$')
				{
					//command function
					std::string userInput = buffer;

					if (userInput == "$register")
					{
						if (userlist.size() < amount)
						{

							auto youareuser = std::to_string(userlist.size());
							userlist.push_back(userlist.size());

							std::string userprompt = "you are Client" +youareuser;
							auto lengthnow = userprompt.size() + 1;

							result = SendDataTCP(tempSet.fd_array[i], (char*)&lengthnow, 1);
							result = SendDataTCP(tempSet.fd_array[i], &(userprompt[0]), lengthnow);
							std::cout << "Client" << userlist.size() << "has register\n";
							file << "Client" << userlist.size() << "has register\n";
						}
						else
						{
							std::string sorry = "sorry clients are full\n";
							auto lengthnow = sorry.size() + 1;
							result = SendDataTCP(tempSet.fd_array[i], (char*)&lengthnow, 1);
							result = SendDataTCP(tempSet.fd_array[i], &(sorry[0]), lengthnow);
						}

					}
					else if (userInput == "$getlist")
					{
						std::string Slist = "\n";
						for (size_t i = 0; i < userlist.size(); i++)
						{
							Slist += ("Client" + std::to_string(userlist[i]) + " is in the server\n");
						}
						auto lengthnow = Slist.size() + 1;
						result = SendDataTCP(tempSet.fd_array[i], (char*)&lengthnow, 1);
						result = SendDataTCP(tempSet.fd_array[i], &(Slist[0]), lengthnow);
					}
					else if (userInput == "$exit")
					{
						shutdown(MasterSet.fd_array[i], SD_BOTH);
						closesocket(MasterSet.fd_array[i]);
						FD_CLR(tempSet.fd_array[i], &MasterSet);
						FD_CLR(tempSet.fd_array[i], &tempSet);
						result = SendDataTCP(tempSet.fd_array[i], (char*)&length, 1);
						result = SendDataTCP(tempSet.fd_array[i], buffer, length);
						break;
					}
				}
				else
				{
					printf("Message received: ");
					printf(buffer);
					printf("\n");
					file <<"Message received: "<< buffer<<"\n";
					// send something preventing block
					result = SendDataTCP(tempSet.fd_array[i], (char*)&length, 1);
					result = SendDataTCP(tempSet.fd_array[i], buffer, length);
					delete[] buffer;
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