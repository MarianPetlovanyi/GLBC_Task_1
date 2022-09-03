#include<iostream>

#ifdef __linux__
	#include"LinuxAPI.h"
#elif _WIN32
	#include"WindowsAPI.h"
#endif



class ServerSocket:public ServerConnection{
	using ServerConnection::ServerConnection;
};
int main(){
	ServerSocket sock(7300);
	sock.bindSock();
	sock.setFlags();
	sock.listenSock();
	sock.acceptSock();
	sock.printSockInfo();
	sock.communicationSTC();
	return 0;
}
