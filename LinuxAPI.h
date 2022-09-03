#include<sys/socket.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>  
#include <thread>
#define MAX 80
#define SA struct sockaddr
const char* SOCK_IP = "127.0.0.1"; 
class ServerConnection{
protected:
	int sock,conn;
	struct sockaddr_in sa,ca;
	
public:
	void setSock(short port){
		memset(&this->sa, 0, sizeof(sa));
		this->sa.sin_family = AF_INET;
		this->sa.sin_port = htons(port);
		inet_pton(AF_INET, SOCK_IP, &this->sa.sin_addr);
	
	}
	
	ServerConnection(short portCtor){
	
		createSock();
		this->setSock(portCtor);
		
	

	}
	~ServerConnection(){
		std::cout<<"Socket destroyed\n";
		close(this->sock);
		close(this->conn);
	}
	void createSock(){
		setSock(socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, IPPROTO_TCP));
		if(sock < 0) {
			int err = errno;
			printf("socket: strerror=%d: %s \n", err, strerror(err));
			this->~ServerConnection();
			exit(1);
		}
		else
			printf("Socket successfully created..\n");
		
	}
	void bindSock(){
		if(bind(sock, (struct sockaddr *) &sa, sizeof sa) == -1){
			int err = errno;
			printf("bind: strerror=%d: %s \n", err, strerror(err));
			this->~ServerConnection();
			exit(1);
		}
		else
			printf("Socket successfully binded...\n");

	}
	void listenSock(){
		if(listen(sock, 5) == -1) {
			int err = errno;
			printf("listen: strerror=%d: %s \n", err, strerror(err));
			this->~ServerConnection();
			exit(1);
		}
		else
			printf("Socket successfully listening...\n");
	}
	void setFlags(){
		int flags = fcntl(sock, F_GETFL);
		if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1) {
			printf("failed to set flag\n");
			int err = errno;
			printf("fcntl: strerror=%d: %s \n", err, strerror(err));
			this->~ServerConnection();
			exit(1);
			}
	}
	void acceptSock(){
		do{
		int len = sizeof(ca);
		conn = accept(sock, (SA*)&ca, (socklen_t*)&len);
		if (conn < 0) {
			int err = errno;
			//printf("socket: strerror=%d: %s \n", err, strerror(err));

			//printf("server accept failed...\n");	
			//this->~ServerConnection();
			}
		else
			printf("server accept the client...\n");
		}while(conn<0);
	
	}
	void reconnectServerSock(short newPort){
		//close(this->sock);
		//close(this->conn);
		createSock();
		setSock(newPort);
		bindSock();
		listenSock();
		setFlags();
		acceptSock();
		printSockInfo();
		communicationSTC();
	}
	
	
	
	
	
	void communicationSTC(){   //server to client
		char buff[MAX];
		int n;
		for(;;){
			bzero(buff, MAX);
			read(conn, buff, sizeof(buff));
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
			else
				printf("From client: %s\t", buff);
			bzero(buff, MAX);
			
			printf("Your message : ");
			n=0;
			while((buff[n++] = getchar()) != '\n')
				;	
			write(conn, buff, sizeof(buff));
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
	
	
	
	
	
	
	
	
	
	void setSock(int value){
		this->sock=value;
	}
	//getters and setters
	sockaddr_in getSA(){
		return this->sa;
	}
	sockaddr_in getCA(){
		return this->ca;
	}
	void setSA(sockaddr_in setValue){
		this->sa = setValue;
	}
	void setCA(sockaddr_in setValue){
		this->ca = setValue;
	}
	
	
	void printSockInfo(){
		std::cout<<"Server port - "<<ntohs(this->getSA().sin_port)<<"\n";
	}
	
};

class ClientConnection{
private:
	int sock;
	struct sockaddr_in sa;


public:
	void setSock(short port){
		memset(&this->sa, 0, sizeof(sa));
		this->sa.sin_family = AF_INET;
		this->sa.sin_port = htons(port);
		inet_pton(AF_INET, SOCK_IP, &this->sa.sin_addr);
	
	}
	
	ClientConnection(short portCtor){
	
		createSock();
			
			
		this->setSock(portCtor);
	}
	~ClientConnection(){
		std::cout<<"Socket destroyed\n";
		close(this->sock);
		
	}
	
	void createSock(){
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sock < 0) {
			int err = errno;
			printf("socket: strerror=%d: %s \n", err, strerror(err));
			this->~ClientConnection();
			exit(1);
		}
		else
			printf("Socket successfully created..\n");	
	}
	void connectSock(){
		do{
		if(connect(sock, (struct sockaddr*)&sa, sizeof sa) == -1) {
			int err = errno;
			printf("connect: strerror=%d: %s \n", err, strerror(err));
			this->~ClientConnection();
		}
		else
			printf("Socket successfully connected..\n");
		}while(sock==-1);
	}
	void setFlags(){
		int flags = fcntl(sock, F_GETFL);
		if (fcntl(sock, F_SETFL, flags|O_NONBLOCK) == -1) {
			printf("failed to set flag\n");
			int err = errno;
			printf("fcntl: strerror=%d: %s \n", err, strerror(err));
		}	
	}
	void reconnectClientSock(int newPort){
		close(sock);
		createSock();
		setSock(newPort);
		connectSock();
		setFlags();
		printSockInfo();	
		communicationCTS();
	}
	void communicationCTS(){ //client to server
		char buff[MAX];
		int n;
		for(;;){
			bzero(buff, sizeof(buff));
			printf("Your message : ");
			n=0;
			while((buff[n++] = getchar()) != '\n')
				;
			write(sock, buff, sizeof(buff));
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
			read(sock, buff, sizeof(buff));
			
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
	
	
	
	

	sockaddr_in getSA(){
		return this->sa;
	}
	void setSA(sockaddr_in setValue){
		this->sa = setValue;
	}
	void printSockInfo(){
		std::cout<<"Client port - "<<ntohs(this->getSA().sin_port)<<"\n";
	}

};
