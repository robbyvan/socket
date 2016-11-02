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

  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = PF_INET;
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverAddr.sin_port = htons(1234);


  struct sockaddr fromAddr;
  socklen_t fromAddrSize = sizeof(fromAddr);

  while(1){
    char buffer[BUF_SIZE] = {'\0'};
    printf("Input a string: \n");
    gets(buffer);

    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &fromAddrSize);
    buffer[strLen] = '\0';

    printf("Messages from server: %s\n", buffer);
  }

  close(sock);
  return 0;
}