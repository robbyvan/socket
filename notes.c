//socket creation
int sockid = socket(family, type, protocol);

//socket close: close a connection(TCP); frees up the port
status = close(sockid);
//status: 0 or -1

//Addresses -Generic
struct sockaddr {
  unsigned short sa_family; // Address family(eg. AF_INET)
  char sa_data[14];// family-specific addr
};
//Addresses -TCP/IP
struct in_addr {
  unsigned long s_addr; //Internet addr, 32 bits
};
struct sockaddr_in {
  unsigned short sin_family; //Internet protocol (AF_INET)
  unsigned short sin_port; //Port 16 bits
  struct in_addr sin_addr;// IP 32 bits
  char sin_zero[8]; //not used
};

// Bind: Assign addr to socket
int status = bind(sockid, &addrport, size);
//addrport: struct sockaddr, IP & port of the machine
//size: size of the addrport structure(in Bytes)
//注: 对UDP, 如果只send, 不用bind, OS每次send时找一个port;若收, 不用bind
//注: 对TCP, destination determined during connection setup, 不用知道port sending from, 在connection setup, receiving end is informed of port

//Listen: 监听
int status = listen(sockid, queueLimit);
//queueLimit: # of active participants that can wait for a connection
//This listen socket is used for LISTENING by SERVER only

// connect(): Establish connection
int status = connect(sockid, &foreignAddr, addrlen);
//foreignAddr: address of the passive participant
//addrlen: sizeof(name)  ??
//connect() is blocking

//Incoming Connection: accept(), server gets a socket for an imcoming client connection by calling this function 
int s = accept(sockid, &clientAddr, &addrLen);
// s: new socket used for data-transfer
// sockid: the origin socket, being listened on
// clientAddr: addr of the active participant
// addrLen: sizeof(clientAddr), must be set appropriately before call; adjusted upon return 
// accept() is blocking: waits for connection before returning; dequeues the next connection on the queue for the socket (sockid)

//Exchanging data:
int count = send(sockid, msg, msgLen, flags);
//msg: const void[], messages to be transmitted
//msgLen: in Bytes
//flags: special options usually 0
//count: # bytes transmitted
int count = recv(sockid, recvBuf, bufLen, flags);
//recv: stores received bytes
//bufLen: # bytes received

int count = sendto(sockid, msg, msgLen, flags, &foreignAddr, addrLen);
//msg msgLen flags count, same with send()
//foreignAddr: destination Address
//addrLen: sizeof(foreignLen)

int count = recvfrom(sockid, recvBuf, buffLen, flags, &clientAddr, addrLen);
//clientAddr: address of the client
//calls are blocking, returns onlu after data is sent / received

//Echo using stream socket
/*Client: 新建TCP socket, 建立*/














