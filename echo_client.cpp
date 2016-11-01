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
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);

  char bufferSend[BUF_SIZE] = {'\0'};
  char bufferRecv[BUF_SIZE] = {'\0'};

  while (1){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
      DieWithError("socket() failed!");
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
      DieWithError("connect() failed!");
    }

    printf("Input a string to send: ");
    gets(bufferSend);


    if (send(sock, bufferSend, strlen(bufferSend), 0) < 0){
      DieWithError("Send() failed!");
    }

    if (bufferSend[0] == 'Q'){
      close(sock);
      break;
    }

    recv(sock, bufferRecv, BUF_SIZE, 0);

    printf("messages from server: %s\n", bufferRecv);

    memset(bufferSend, 0, BUF_SIZE);
    memset(bufferRecv, 0, BUF_SIZE);
    close(sock);
  }

  return 0;
}









