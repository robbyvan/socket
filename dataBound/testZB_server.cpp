#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>


#define BUF_SIZE 100

void DieWithError(char *errorMessage) {
  perror(errorMessage);
  exit(1);
}

int main(){
  int serv_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(1234);
  if (bind(serv_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 ){
    DieWithError("bind failed.");
  };

  if (listen(serv_socket, 20) < 0){
    DieWithError("listen failed");
  };

  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  char buffer[BUF_SIZE] = {'\0'};
  
  int clnt_socket = accept(serv_socket, (struct sockaddr*)&clnt_addr, &clnt_addr_size); 
  if (clnt_socket < 0){
    DieWithError("accept failed.");
  };

  sleep(6);

  int bufferLen = recv(clnt_socket, buffer, BUF_SIZE, 0);
  send(clnt_socket, buffer, bufferLen, 0);

  close(clnt_socket);
  close(serv_socket);

  return 0;
}





