#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>


#define BUF_SIZE 1024

void DieWithError(char *errorMessage) {
  perror(errorMessage);
  exit(1);
}

int main(){
  char *fileName = "./send.txt";
  FILE *fp = fopen(fileName, "rb");

  if (fp == NULL){
    printf("Can't open file, press any key to exit.\n");
    exit(0);
  }

  int servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  struct sockaddr_in sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));
  sockAddr.sin_family = PF_INET;
  sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  sockAddr.sin_port = htons(1234);

  if ( bind(servSock, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) ){
    DieWithError("bind failed.");
  }

  if (listen(servSock, 20) < 0){
    DieWithError("listen failed.");
  }

  struct sockaddr_in clntAddr;
  socklen_t clntAddrSize = sizeof(clntAddr);
  int clntSock = accept(servSock, (struct sockaddr*)&clntAddr, &clntAddrSize);

  char buffer[BUF_SIZE] = {'\0'};
  int nCount;
  while ( (nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0){
    send(clntSock, buffer, nCount, 0);
  }

  shutdown(clntSock, SHUT_WR);
  recv(clntSock, buffer, BUF_SIZE, 0);

  fclose(fp);
  close(clntSock);
  close(servSock);

  return 0;
}





