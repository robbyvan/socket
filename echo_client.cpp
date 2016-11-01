#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
void DieWithError(char *errorMessage) {
  perror(errorMessage);
  exit(1);
}

int main(){
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    DieWithError("socket() failed!");
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1235);
  if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
    DieWithError("connect() failed!");
  }

  char bufferSend[30] = {'\0'};
  printf("Input a string to send: ");
  scanf("%s", bufferSend);
  if (send(sock, bufferSend, strlen(bufferSend), 0) < 0){
    DieWithError("Send() failed!");
  }

  char buffer[40] = {'\0'};
  recv(sock, buffer, sizeof(buffer)-1, 0);

  printf("messages from server: %s\n", buffer);

  close(sock);

  return 0;
}









