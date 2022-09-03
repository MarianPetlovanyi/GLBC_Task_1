all: server client
	
server: server.cpp
	g++ server.cpp -o server -w -lws2_32

client: client.cpp
	g++ client.cpp -o client -w -lws2_32

clean:
	$(RM) client
	$(RM) server
