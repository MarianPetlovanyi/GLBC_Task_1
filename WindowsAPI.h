#undef UNICODE
#define _WIN32_WINNT
#define _WIN32_WINNT 0x501
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#include <iostream>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define MAX 80
const char* SOCK_IP = "127.0.0.1";
class ServerConnection {
private:
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    int portSock;
public:
    void setSock() {
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;
    }

    ServerConnection(int port) {
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            exit(1);
        }
        createSock(port);
        setSock();

    }

    ~ServerConnection() {

    }

    void createSock(int port) {
        memset(&hints, 0, sizeof(hints));
        std::string tmp;
        tmp=std::to_string((int)port);
        char const *num_char = tmp.c_str();
        iResult = getaddrinfo(SOCK_IP, num_char, &hints, &result);
        portSock=port;
        if ( iResult != 0 ) {
            printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            exit(1);
        }
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            exit(1);
        }else
            printf("Socket successfully created..\n");
    }

    void bindSock() {
        iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            exit(1);
        }else
            printf("Socket successfully binded...\n");
        freeaddrinfo(result);
    }
    void listenSock(){
        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            exit(1);
        }else
            printf("Socket successfully listening...\n");
    }
    void setFlags(){
    };

    void acceptSock() {
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            exit(1);
        }else
            printf("server accept the client...\n");
        closesocket(ListenSocket);
    }

    void reconnectServerSock(int newPort) {

        setSock();
        createSock(newPort);
        bindSock();
        listenSock();
        acceptSock();
        printSockInfo();
        communicationSTC();

    }

    void communicationSTC() {
        closesocket(ListenSocket);
    char buff[MAX];
		int n;
		for(;;){
			bzero(buff, MAX);
			iResult=recv(ClientSocket, buff, sizeof(buff), 0);
			if((strncmp(buff, "exit", 4))==0){
				printf("Client Exit...\n");
				break;
			}
			if((strncmp(buff, "NewPort-",8)) == 0){
				int scanport;
				sscanf(buff,"%*8s %d",&scanport);
				printf("Client wants to change the port to %d\n", scanport);
				reconnectServerSock(scanport);
			}
            printf("From client: %s\t", buff);
			bzero(buff, MAX);

			printf("Your message : ");
			n=0;
			while((buff[n++] = getchar()) != '\n')
				;
			send( ClientSocket, buff, sizeof(buff), 0 );
			if((strncmp(buff, "exit", 4))==0){
				printf("Server Exit...\n");
				break;
			}
			if((strncmp(buff, "NewPort-",8)) == 0){
				int scanport;
				sscanf(buff,"%*8s %d",&scanport);
				printf("Server wants to change the port to %d\n", scanport);
				reconnectServerSock(scanport);
			}

		}
    }
    void printSockInfo(){
        printf("Current server port = %d\n", portSock);
    }

};

class ClientConnection{
private:
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    const char *sendbuf = "this is a test";
    int iResult;
    int portSock;

public:
    void setSock(){
        ZeroMemory( &hints, sizeof(hints) );
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
    }
    ClientConnection(int port){
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            exit(1);
        }

        setSock();
        createSock(port);
    }
    ~ClientConnection(){
        closesocket(ConnectSocket);
        WSACleanup();
    }
    void createSock(int port){
        std::string tmp;
        tmp=std::to_string((int)port);
        char const *num_char = tmp.c_str();
        iResult = getaddrinfo(SOCK_IP, num_char, &hints, &result);
        portSock=port;
        if ( iResult != 0 ) {
            printf("getaddrinfo failed with error: %d\n", iResult);
            WSACleanup();
            exit(1);
        }else
            printf("Socket successfully created..\n");

    }

    void connectSock(){
        for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                printf("socket failed with error: %ld\n", WSAGetLastError());
                WSACleanup();
                exit(1);
            }
            iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                continue;
            }
            break;
        }
        freeaddrinfo(result);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            WSACleanup();
            exit(1);
        }else
            printf("Socket successfully connected..\n");
    }

    void setFlags(){
    };

    void reconnectClientSock(int newPort){
        setSock();
        createSock(newPort);
        connectSock();
        printSockInfo();
        communicationCTS();

    }
    void communicationCTS() {
        char buff[MAX];
		int n;
		for(;;){
			bzero(buff, sizeof(buff));
			printf("Your message : ");
			n=0;
			while((buff[n++] = getchar()) != '\n')
				;
			send( ConnectSocket, buff, sizeof(buff), 0 );
			if((strncmp(buff, "NewPort-",8)) == 0){
				int scanport;
				sscanf(buff,"%*8s %d",&scanport);
				printf("Client wants to change the port to %d\n", scanport);
				reconnectClientSock(scanport);
			}

			if((strncmp(buff, "exit", 4))==0){
				printf("Client Exit...\n");
				break;
			}
			bzero(buff, sizeof(buff));
			recv(ConnectSocket, buff, sizeof(buff), 0);

			if((strncmp(buff, "exit", 4))==0){
				printf("Server Exit...\n");
				break;
			}
			if((strncmp(buff, "NewPort-",8)) == 0){
				int scanport;
				sscanf(buff,"%*8s %d",&scanport);
				printf("Server wants to change the port to %d\n", scanport);
				reconnectClientSock(scanport);
			}
			printf("From Server : %s\t", buff);
		}
    }
    void printSockInfo(){
        printf("Current client port = %d\n", portSock);
    }

};
