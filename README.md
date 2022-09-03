GlobalLogic BaseCamp task1

Client/Server application for dynamic text communication.
- client and server has to start communication via port 7300(done).
- server can request to change communication port in any time(done).
- client also can request to change communication port in any time.(done)
  Ex: Communication established via port 7300.
- Text messages can go in both directions(done).
- To change communication port client or server has to send message
  "NewPort-<PortNumber>"(done)
- Client or server has to replay "success" or "failure"(Server client send info about current port after changing it)
- In case of success communication will be re-established via New port(done)

Bonus option: communication port must be changed each 45 seconds without user
  disturbing. (Maybe will be done but after deadline)

User guide
- To change port type "NewPort-8888" or any number ( from 1024 to 49151 )
- To exit type "exit"