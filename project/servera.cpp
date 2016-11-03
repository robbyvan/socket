/*
USC ID: xxxxx-xx807
1 UDP, 21000+xxx (last three digits of your USC ID)
Port: 21807
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define BUF_SIZE 100
#define MAX_VOLUME 1000

int main(){
  int sockA = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  printf("The Server A is up and running using UDP on port <21807>.\n");

  struct sockaddr_in serverA_addr;
  memset(&serverA_addr, 0, sizeof(serverA_addr));
  serverA_addr.sin_family = PF_INET;
  serverA_addr.sin_addr.s_addr = htonl(INADDR_ANY);//自动获取本地IP
  serverA_addr.sin_port = htons(21807);
  bind(sock, (struct sockaddr*)&serverA_addr, sizeof(serverA_addr));

  struct sockaddr serverD_addr;
  socklen_t serverD_addr_size = sizeof(serverD_addr);
  
  char bufRecvFromD[BUF_SIZE];


  //接收要处理的数据数目
  int strLen = recvfrom(sockA, bufRecvFromD, BUF_SIZE, 0, &serverD_addr, &serverD_addr_size);
  int sample_volume = atoi(bufRecvFromD);
  printf("The server gonna deal with <%d> samples", sample_volume);

  //接收要进行的函数操作
  char function_name[BUF_SIZE] = {'\0'};
  
  int strLen = recvfrom(sockA, bufRecvFromD, BUF_SIZE, 0, &serverD_addr, &serverD_addr_size);
  
  strcpy(function_name, bufRecvFromD);
  printf("The server has received reduction type <%s>", function_name);

  //接收要处理的数据
  int nums[sample_volume];
  int numCount = 0;
  while(1){
    if ( numCount == sample_volume){
        printf("The Server A has received <%d> numbers\n", numCount+1);
        break;
      }
    int strLen = recvfrom(sockA, bufRecvFromD, BUF_SIZE, 0, &serverD_addr, &serverD_addr_size);
    nums[numCount++] = atoi(bufRecvFromD);
  }





  return 0;
}