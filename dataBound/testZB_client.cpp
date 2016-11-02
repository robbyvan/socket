#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define BUF_SIZE 100

void DieWithError(char *errorMessage) {
  perror(errorMessage);
  exit(1);
}

int main(){
  
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0){
    DieWithError("socket() failed!");
  }

  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
    DieWithError("connect() failed!");
  }

  char bufferSend[BUF_SIZE] = {'\0'};
  printf("Input a string: ");
  gets(bufferSend);
  for (int i = 0; i < 3; i++){
    send(sock, bufferSend, strlen(bufferSend), 0);
  }

  char bufferRecv[BUF_SIZE] = {'\0'};
  recv(sock, bufferRecv, BUF_SIZE, 0);
  printf("messages from server: %s\n", bufferRecv);

  close(sock);

  return 0;
}









