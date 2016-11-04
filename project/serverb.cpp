/*
USC ID: xxxxx-xx807
1 UDP, 22000+xxx (last three digits of your USC ID)
Port: 22807
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

/**/
int get_max(int *nums, int sample_volume);
int get_min(int *nums, int sample_volume);
int get_sum(int *nums, int sample_volume);
int get_sos(int *nums, int sample_volume);
/**/

int main(){
  int sockB = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  struct sockaddr_in serverB_addr;
  memset(&serverB_addr, 0, sizeof(serverB_addr));
  serverB_addr.sin_family = PF_INET;
  serverB_addr.sin_addr.s_addr = htonl(INADDR_ANY);//自动获取本地IP
  serverB_addr.sin_port = htons(22807);
  bind(sockB, (struct sockaddr*)&serverB_addr, sizeof(serverB_addr));
  printf("The Server B is up and running using UDP on port <22807>.\n");

  struct sockaddr serverD_addr;
  socklen_t serverD_addr_size = sizeof(serverD_addr);
  
  char bufRecvFromD[BUF_SIZE];

  //步骤:接收数据总数->接收操作函数->接收数据->操作->返回给D

  //接收要处理的数据数目
  int strLen = recvfrom(sockB, bufRecvFromD, BUF_SIZE, 0, &serverD_addr, &serverD_addr_size);
  int sample_volume = atoi(bufRecvFromD);
  printf("The server gonna deal with <%d> samples", sample_volume);

  //接收要进行的函数操作
  char function_name[BUF_SIZE] = {'\0'};
  
  strLen = recvfrom(sockB, bufRecvFromD, BUF_SIZE, 0, &serverD_addr, &serverD_addr_size);
  
  strcpy(function_name, bufRecvFromD);
  printf("The server has received reduction type <%s>", function_name);

  //接收要处理的数据
  int numsB[sample_volume];
  int numCount = 0;
  while(1){
    if (numCount == sample_volume){
        printf("The Server B has received <%d> numbers\n", numCount);
        break;
      }
    int strLen = recvfrom(sockB, bufRecvFromD, BUF_SIZE, 0, &serverD_addr, &serverD_addr_size);
    numsB[numCount++] = atoi(bufRecvFromD);
    printf("Receiving %d-th number: %d\n", numCount, numsB[numCount-1]);
  }
  close(sockB);
  printf("Socket for receiving data from AWS has closed.\n");

  printf("Received numbers have been printed above.\n");

  //本地服务器B处理接收到的数据
  char max_func[] = "max";
  char min_func[] = "min";
  char sum_func[] = "sum";
  char sos_func[] = "sos";
  int result;

  if (strcmp(function_name, max_func) == 0){
    //get max
    result = get_max(numsB, sample_volume);
  }else if (strcmp(function_name, min_func) == 0) {
    //get min
    result = get_min(numsB, sample_volume);
  }else if (strcmp(function_name, sum_func) == 0){
    //get sum
    result = get_sum(numsB, sample_volume);
  }else if (strcmp(function_name, sos_func) == 0){
    //get sos
    result = get_sos(numsB, sample_volume);
  }else {
    printf("No such operation.\n");
  }

  printf("After doing %s operation, the result is : %d\n", function_name, result);



  //完成数据处理后, 将结果返回给AWS
  int sockD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  struct sockaddr_in server_D_addr;
  server_D_addr.sin_family = PF_INET;
  server_D_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_D_addr.sin_port = htons(24807);

  char bufSendToD[BUF_SIZE] = {'\0'};

  memset(bufSendToD, 0, BUF_SIZE);
  sprintf(bufSendToD, "%d", result);

  sleep(2);
  sendto(sockD, bufSendToD, strlen(bufSendToD), 0, (struct sockaddr*)&server_D_addr, sizeof(server_D_addr));
  close(sockD);








  return 0;
}

int get_max(int *nums, int sample_volume){
  //sort the array in ASC
  // return nums[sample_volume-1];
  int temp = nums[0];
  for (int i = 1; i < sample_volume; ++i){
    if (nums[i] > temp){
      temp = nums[i];
    }
  }
  return temp;
}
int get_min(int *nums, int sample_volume){
  //sort the array in ASC
  // return nums[0];
  int temp = nums[0];
  for (int i = 1; i < sample_volume; ++i){
    if (nums[i] < temp){
      temp = nums[i];
    }
  }
  return temp;
}
int get_sum(int *nums, int sample_volume){
  int sum = 0;
  for (int i = 0; i < sample_volume; ++i){
    sum += nums[i];
  }
  return sum;
}
int get_sos(int *nums, int sample_volume){
  int sos = 0;
  for (int i = 0; i < sample_volume; ++i){
    sos += nums[i] * nums[i];
  }
  return sos;
}

// }