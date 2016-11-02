// ssize_t sendto(int sock, void *buf, size_t nbytes, int flags, struct sockaddr *to, socklen_t addrlen);  //Linux
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define BUF_SIZE 100

int main(){

  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = PF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(1234);
  bind(sock, (struct sockaddr*)&servAddr, sizeof(servAddr));

  struct sockaddr clntAddr;
  socklen_t clntAddrSize = sizeof(clntAddr);
  char buffer[BUF_SIZE];
  while(1){
    int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &clntAddr, &clntAddrSize);
    sendto(sock, buffer, strLen, 0, &clntAddr, clntAddrSize);
  }

  close(sock);
  return 0;
}
