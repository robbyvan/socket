#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define BUF_SIZE 100

int main(int argc, char const *argv[]){
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(25807);

  char bufSendToServ[BUF_SIZE] = {'\0'};
  char bufRecvFromServ[BUF_SIZE] = {'\0'};

  strcpy(bufSendToServ, argv[1]);
  // printf("Gonna send: %s to server\n", bufSendToServ);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  printf("The client is up and running.\n");
  connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
  send(sock, bufSendToServ, strlen(bufSendToServ), 0);

  printf("The client has sent the reduction type <%s> to AWS.\n", bufSendToServ);

  int numCount = 0;

  FILE *fp = fopen("./nums.csv", "rb");
  if (fp == NULL){
    printf("Can not open file\n");
  }else{
    printf("Open Success.\n");
  }
  
  while(!feof(fp)){
    fscanf(fp, "%s\n", &bufSendToServ);
    numCount += 1;

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    send(sock, bufSendToServ, strlen(bufSendToServ), 0);
    memset(bufSendToServ, 0, BUF_SIZE);
    close(sock);//发送数字结束
  }

  printf("The client has sent <%d> numbers to AWS\n", numCount);

  char finMsg[] = "clientFin";
  strcpy(bufSendToServ, finMsg);
  
  sock = socket(PF_INET, SOCK_STREAM, 0);
  connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
  
  send(sock, bufSendToServ, strlen(bufSendToServ), 0);
  printf("%s\n", finMsg);
  
  memset(bufSendToServ, 0, BUF_SIZE);
  shutdown(sock, SHUT_WR);

  return 0;
}