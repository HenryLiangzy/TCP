// TCP_client.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <stdlib.h>
#include <Windows.h>
#include <string.h>
#include <WS2tcpip.h>


DWORD WINAPI recive(LPVOID sock)
{
	SOCKET new_sock = *(SOCKET *)sock;
	char bufStr[1024];
	int result;

	do
	{
		result = recv(new_sock, bufStr, sizeof(bufStr), 0);
		if (result == SOCKET_ERROR) {
			fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
			WSACleanup();
			return -1;
		}

		else if (result == 0) {
			printf("Client closed connection\n");
			closesocket(new_sock);
			return -1;
		}
		printf("========================================\n%s \n", bufStr);



	} while (strncmp(bufStr, "Bye", 3) != 0);

	printf("Connection close\n");
	closesocket(new_sock);

	return 0;
}

DWORD WINAPI send(LPVOID socket)
{
	SOCKET sock = *(SOCKET *)socket;
	char bufStr[1024];
	int result;

	do
	{
		printf("Input the Message:");
		//scanf_s("%s", bufStr, 1024);
		fgets(bufStr, sizeof(bufStr), stdin);

		result = send(sock, bufStr, sizeof(bufStr), 0);

		if (result == SOCKET_ERROR) {
			printf("Disconnected\n");
			closesocket(sock);
			WSACleanup();
			return -1;
		}

		if (result == 0) {
			printf("server closed connection\n");
			closesocket(sock);
			WSACleanup();
			return -1;
		}
		memset(bufStr, '\0', sizeof(bufStr));


	} while (strncmp(bufStr, "Bye", 3) != 0);

	printf("Connection stop\n");
	closesocket(sock);

	return 0;
}

int main()
{
	WSAData wsaData;
	SOCKET sock;
	struct hostent *he;
	struct sockaddr_in server;
	char server_name[] = "localhost";
	int addr;
	
	printf("Strating WSA......");
	//WSAStart up start
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
	{
		fprintf(stderr, "WSAStartup failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Done\n");

	printf("Creating socket......");
	//create a socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Done\n");


	
	char str[64];
	
	memset(&server, 0, sizeof(server));
	char IP[64];
	printf("input the IPaddress:");
	scanf_s("%s", IP, sizeof(IP));
	//memcpy(&(server.sin_addr), he->h_addr_list[0], he->h_length);
	server.sin_addr.s_addr = inet_addr(IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(7070);

	printf("Connecting to the server ......");


	//connect to the server
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)))
	{
		printf("connecting fail\n");
		return -1;
	}
	printf("Done\n");

	printf("Connect to server %s, port %d\n", inet_ntoa(server.sin_addr), htons(server.sin_port));

	char declar[] = "This is message from client, recive and repeat\n";
	send(sock, declar, strlen(declar), 0);
	

	char bufStr[1024];
	recv(sock, bufStr, strlen(bufStr), 0);

	//used thread to recive
	HANDLE recive_thread = CreateThread(NULL, 0, recive, &sock, 0, NULL);
	HANDLE send_thread = CreateThread(NULL, 0, send, &sock, 0, NULL);

	WaitForSingleObject(recive_thread, INFINITE);
	WaitForSingleObject(send_thread, INFINITE);

	closesocket(sock);

	WSACleanup();

    return 0;
}

