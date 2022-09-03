#include<iostream>


#ifdef __linux__
	#include"LinuxAPI.h"
#elif _WIN32
	#include"WindowsAPI.h"
#endif

class ClientSocket:public ClientConnection{
	using ClientConnection::ClientConnection;
};

int main(){
	ClientSocket s(7300);
	s.connectSock();
	s.communicationCTS();


}
