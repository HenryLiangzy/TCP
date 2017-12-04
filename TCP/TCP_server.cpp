// TCP.cpp: 定义控制台应用程序的入口点。
//



#include "stdafx.h"
#include <WinSock2.h>
#include <string.h>
#include <Windows.h>
#include <stdlib.h>


//recive thread
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
			//fprintf(stderr, "send() failed with error %d\n", WSAGetLastError());
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


int main(void)
{
	WSAData wsaData;
	//HANDLE tid[3];


	printf("Starting WSA......");
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
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		fprintf(stderr, "socket() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Done\n");

	//bind the socket to port
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(7070);

	printf("Binding port to the socket......");
	//bind the port to socket
	if (bind(sock, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
		fprintf(stderr, "bind() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	printf("Done\n");

	//listen the connection
	if (listen(sock, 5) == SOCKET_ERROR) {
		fprintf(stderr, "listen() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	printf("Listening the connection......");

	//accept the connection
	int addrlen;
	int new_socket;
	struct sockaddr_in connection;
	addrlen = sizeof(connection);
	new_socket = accept(sock, (struct sockaddr *)&connection, &addrlen);
	printf("Get connection\n");
	if (new_socket == INVALID_SOCKET) {
		fprintf(stderr, "accept() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	//show the connection from
	printf("detected connection from %s, port %d\n", inet_ntoa(connection.sin_addr), htons(connection.sin_port));


	
	//receving data
	char bufStr[1024];
	int result = recv(new_socket, bufStr, sizeof(bufStr), 0);

	if (result == SOCKET_ERROR) {
		fprintf(stderr, "recv() failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	else if (result == 0) {
		printf("Client closed connection\n");
		closesocket(new_socket);
		return -1;
	}

	//sending the definition
	char declar[] = "This is message from Server, recive and repeat\n";
	send(new_socket, declar, strlen(declar), 0);

	
	//used thread to recive
	HANDLE recive_thread = CreateThread(NULL, 0, recive, &new_socket, 0, NULL);
	HANDLE send_thread = CreateThread(NULL, 0, send, &new_socket, 0, NULL);

	WaitForSingleObject(recive_thread, INFINITE);
	WaitForSingleObject(send_thread, INFINITE);

	closesocket(new_socket);
	WSACleanup();

    return 0;
}

