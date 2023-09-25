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
		/*printf("What is your port number: ");
		int port;
		std::cin >> port;*/
		printf("Client amount : ");
		int amount;
		std::cin >> amount;

		Server server(31337, amount); //hardcode port
	}
		
	
	//Client
	if (thisDevice == 2)
	{
		/*printf("What is your IP number: ");
		std::string IP ="";
		std::cin >> IP;
		printf("What is your port number: ");
		int port;
		std::cin >> port;*/
		Client client("127.0.0.1", 31337);
	}
		
	return WSACleanup();
}
