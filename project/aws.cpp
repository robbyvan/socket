/*
USC ID: 69515-21807
1 UDP, 24000+xxx (last three digits of your USC ID) 
1 TCP, 25000+xxx (last three digits of your USC ID)
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define BUF_SIZE 100

int main(){
  //用来握手的socket
  int tcp_origin_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = PF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//
  server_addr.sin_port = htons(25807);
  bind(tcp_origin_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

  if ( listen(tcp_origin_sock, 50) > -1 ){
    printf("The AWS is up and running.\n");
  }

  int countClient = 0;
  char *clientFin = "clientFin";
  FILE *fpRecvNum = fopen("./redvNum.csv", "wb");
  while(1){
    //accept后新建一个交换数据的client_sock
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_sock = accept(tcp_origin_sock, (struct sockaddr*)&client_addr, &client_addr_size);
    countClient++;

    //服务器接受client来的数据的buffer
    char bufRecvFromClnt[BUF_SIZE] = {'\0'};
    // char fileBuffer[BUF_SIZE] = {'\0'};
    int buffer_len = recv(client_sock, bufRecvFromClnt, BUF_SIZE, 0);

    if (strcmp(bufRecvFromClnt, clientFin) == 0){
      countClient -= 1;
      printf("The AWS has received %d numbers from the client using TCP over port 25807\n", countClient);
      break;
    }

    if (buffer_len > 0){
      fprintf(fpRecvNum, "%s\n", bufRecvFromClnt);
    }

    close(client_sock);
    memset(bufRecvFromClnt, 0, BUF_SIZE);
  }

  close(tcp_origin_sock);
  printf("tcp_origin_sock closed\n");
  return 0;
}