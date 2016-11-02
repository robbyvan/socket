#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define BUF_SIZE 1024

int main(){
  char fileName[100] = {'\0'};
  printf("input filename to save:");
  gets(fileName);

  FILE *fp = fopen(fileName, "wb");
  if (fp == NULL){
    printf("Can't open file, press to exit.\n");
    exit(0);
  }

  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  struct sockaddr_in sockAddr;
  memset(&sockAddr, 0, sizeof(sockAddr));
  sockAddr.sin_family = PF_INET;
  sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  sockAddr.sin_port = htons(1234);

  connect(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr));

  char buffer[BUF_SIZE] = {'\0'};
  int nCount;
  while ( (nCount = recv(sock, buffer, BUF_SIZE, 0)) > 0 ){
    fwrite(buffer, nCount, 1, fp);
  }
  puts("File transfer success!");

  fclose(fp);
  close(sock);
  return 0;
}