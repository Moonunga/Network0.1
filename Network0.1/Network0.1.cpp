#include"Client.h"
#include"Server.h"

int main()
{
	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);

	int thisDevice;
	do
	{
		printf("This device is a :\n");
		printf("1: Server\n");
		printf("2: Client\n");
		printf("address 127.0.0.1 port 31337\n");
		std::cin >> thisDevice;
	} while (thisDevice != 1 && thisDevice != 2);

	//Server
	if (thisDevice == 1)
	{
		printf("What is your port number: \n");
		int port;
		std::cin >> port;
		Server server(port); //hardcode port
	}
		
	
	//Client
	if (thisDevice == 2)
	{
		printf("What is your IP number: \n");
		std::string IP ="";
		std::cin >> IP;
		printf("What is your port number: \n");
		int port;
		std::cin >> port;
		Client client(IP, port);
	}
		
	return WSACleanup();
}
