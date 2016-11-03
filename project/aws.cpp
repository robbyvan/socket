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

  struct sockaddr_in serverD_addr;
  memset(&serverD_addr, 0, sizeof(serverD_addr));
  serverD_addr.sin_family = PF_INET;
  serverD_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//
  serverD_addr.sin_port = htons(25807);
  bind(tcp_origin_sock, (struct sockaddr*)&serverD_addr, sizeof(serverD_addr));

  if ( listen(tcp_origin_sock, 50) > -1 ){
    printf("The AWS is up and running.\n");
  }

  char function_name[BUF_SIZE] = {'\0'};
  int countClient = 0;
  char *clientFin = "clientFin";

  //首先接收控制台参数function name
  struct sockaddr_in client_addr;
  socklen_t client_addr_size = sizeof(client_addr);
  int client_sock = accept(tcp_origin_sock, (struct sockaddr*)&client_addr, &client_addr_size);

  char bufRecvFromClnt[BUF_SIZE] = {'\0'};
  int buffer_len = recv(client_sock, bufRecvFromClnt, BUF_SIZE, 0);

  strcpy(function_name, bufRecvFromClnt);
  // printf("Received function name: %s at 1st connection.\n", function_name);

  //开始接收数据, 并将收到数据生成服务器本地文件"recvNum.csv"
  FILE *fpRecvNum = fopen("./recvNum.csv", "wb");

  while(1){
    //accept后新建一个交换数据的client_sock
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_sock = accept(tcp_origin_sock, (struct sockaddr*)&client_addr, &client_addr_size);
    countClient++;

    //服务器接收client来的数据的buffer
    char bufRecvFromClnt[BUF_SIZE] = {'\0'};
    int buffer_len = recv(client_sock, bufRecvFromClnt, BUF_SIZE, 0);

    if (strcmp(bufRecvFromClnt, clientFin) == 0){
      fclose(fpRecvNum);
      printf("file closed.\n");
      countClient -= 1;
      printf("The AWS has received %d numbers from the client using TCP over port <25807>.\n", countClient);

      //把刚刚写在文件里的数据写到数组里(其实也可以第一遍写到一个大数组,以后直接用clientCount)
      int nums[countClient];
      FILE *fpRecvNum = fopen("./recvNum.csv", "rb");
      int i = 0;
      char numStr[BUF_SIZE];
      while (!feof(fpRecvNum)){
        fscanf(fpRecvNum, "%s\n", &numStr);
        nums[i++] = atoi(numStr);
      }
      for (i = 0; i < countClient; i++){
        printf("%d\n", nums[i]);
      }
      break;
    }

    if (buffer_len > 0){
      fprintf(fpRecvNum, "%s\n", bufRecvFromClnt);
    }

    close(client_sock);
    memset(bufRecvFromClnt, 0, BUF_SIZE);
  }

  close(tcp_origin_sock);
  // printf("tcp_origin_sock closed\n");

  /*lines above are Phase 1*/

  printf("\n---Phase 1 End---\n\n---Phase 2 Start---\n");
  //send numbers to server A
  // int sockA = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // struct sockaddr_in serverD_addr;
  // memset(&serverD_addr, 0, sizeof(serverD_addr));
  // serverD_addr.sin_family = PF_INET;
  // serverD_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // serverD_addr.sin_port = htons(21807);

  // struct sockaddr serverD_addr;
  // socklen_t serverD_addr_size = sizeof(serverD_addr);

  // int sample_volume = countClient/3;














  return 0;
}