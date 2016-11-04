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
  char clientFin[] = "clientFin";

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
      fclose(fpRecvNum);
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

  //1a)先发送数据总数给A
  int sockA = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  printf("sockA created.\n");

  struct sockaddr_in serverA_addr;
  memset(&serverA_addr, 0, sizeof(serverA_addr));
  serverA_addr.sin_family = PF_INET;
  serverA_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverA_addr.sin_port = htons(21807);

  int sample_volume = countClient/3;

  printf("sample_volume is: %d\n", sample_volume);

  char bufSendToA[BUF_SIZE] = {'\0'};

  memset(bufSendToA, 0, BUF_SIZE);
  sprintf(bufSendToA, "%d", sample_volume);//整数转字符串

  printf("sprinf is:%s\n", bufSendToA);

  // itoa(sample_volume, bufSendToA, 10);
  sendto(sockA, bufSendToA, strlen(bufSendToA), 0, (struct sockaddr*)&serverA_addr, sizeof(serverA_addr));
  memset(bufSendToA, 0, BUF_SIZE);

  //1b)先发送数据总数给B
  int sockB = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  printf("sockB created.\n");

  struct sockaddr_in serverB_addr;
  memset(&serverB_addr, 0, sizeof(serverB_addr));
  serverB_addr.sin_family = PF_INET;
  serverB_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverB_addr.sin_port = htons(22807);

  // int sample_volume = countClient/3;

  printf("sample_volume is: %d\n", sample_volume);

  char bufSendToB[BUF_SIZE] = {'\0'};

  memset(bufSendToB, 0, BUF_SIZE);
  sprintf(bufSendToB, "%d", sample_volume);//整数转字符串

  printf("sprinf is:%s\n", bufSendToB);

  // itoa(sample_volume, bufSendToA, 10);
  sendto(sockB, bufSendToB, strlen(bufSendToB), 0, (struct sockaddr*)&serverB_addr, sizeof(serverB_addr));
  memset(bufSendToB, 0, BUF_SIZE);

//1c)先发送数据总数给C
  int sockC = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  printf("sockC created.\n");

  struct sockaddr_in serverC_addr;
  memset(&serverC_addr, 0, sizeof(serverC_addr));
  serverC_addr.sin_family = PF_INET;
  serverC_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverC_addr.sin_port = htons(23807);

  printf("sample_volume is: %d\n", sample_volume);

  char bufSendToC[BUF_SIZE] = {'\0'};

  memset(bufSendToC, 0, BUF_SIZE);
  sprintf(bufSendToC, "%d", sample_volume);//整数转字符串

  printf("sprinf is:%s\n", bufSendToC);

  sendto(sockC, bufSendToC, strlen(bufSendToC), 0, (struct sockaddr*)&serverC_addr, sizeof(serverC_addr));
  memset(bufSendToC, 0, BUF_SIZE);


  //2a)再发送函数名给A
  strcpy(bufSendToA, function_name);

  printf("function name is: %s\n", bufSendToA);

  sendto(sockA, bufSendToA, strlen(bufSendToA), 0, (struct sockaddr*)&serverA_addr, sizeof(serverA_addr));
  memset(bufSendToA, 0, BUF_SIZE);

  //2b)再发送函数名给B
  strcpy(bufSendToB, function_name);

  printf("function name is: %s\n", bufSendToB);

  sendto(sockB, bufSendToB, strlen(bufSendToB), 0, (struct sockaddr*)&serverB_addr, sizeof(serverB_addr));
  memset(bufSendToB, 0, BUF_SIZE);

  //2c)再发送函数名给C
  strcpy(bufSendToC, function_name);

  printf("function name is: %s\n", bufSendToC);

  sendto(sockC, bufSendToC, strlen(bufSendToC), 0, (struct sockaddr*)&serverC_addr, sizeof(serverC_addr));
  memset(bufSendToC, 0, BUF_SIZE);


  //3a)最后发送sample_volume个数据给A
  FILE *fp = fopen("./recvNum.csv", "rb");

  for (int i = 0; i < sample_volume; i++){
    fscanf(fp, "%s\n", &bufSendToA);
    printf("sending: %s\n", bufSendToA);
    sendto(sockA, bufSendToA, strlen(bufSendToA), 0, (struct sockaddr*)&serverA_addr, sizeof(serverA_addr));
  }
  printf("Sent %d numbers to Server A.\n", sample_volume);
  close(sockA);

  //3b)最后发送sample_volume个数据给B
  for (int i = 0; i < sample_volume; ++i){
    fscanf(fp, "%s\n", &bufSendToB);
    printf("sending: %s\n", bufSendToB);
    sendto(sockB, bufSendToB, strlen(bufSendToB), 0, (struct sockaddr*)&serverB_addr, sizeof(serverB_addr));
  }
  printf("Sent %d numbers to Server B.\n", sample_volume);
  close(sockB);

  //3b)最后发送sample_volume个数据给C
  for (int i = 0; i < sample_volume; ++i){
    fscanf(fp, "%s\n", &bufSendToC);
    printf("sending: %s\n", bufSendToC);
    sendto(sockC, bufSendToC, strlen(bufSendToC), 0, (struct sockaddr*)&serverC_addr, sizeof(serverC_addr));
  }
  printf("Sent %d numbers to Server C.\n", sample_volume);
  if(feof(fp)){
    printf("Yes, this is the End Of File.\n");
  }
  fclose(fp);
  close(sockC);




  //接收从服务器A/B/C返回的结果
  int sockD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  struct sockaddr_in, serverD_addr;
  memset(&serverD_addr, 0, sizeof(serverD_addr));
  serverD_addr.sin_family = PF_INET;
  serverD_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverD_addr.sin_port = htons(24807);
  bind(sockD, (struct sockaddr*)&serverD_addr, sizeof(serverD_addr));
  printf("The Server D is up and running using UDP on port <24807>.\n");

  struct sockaddr serverA_addr, serverB_addr, serverC_addr;
  socklen_t serverA_addr_size = sizeof(serverA_addr);
  socklen_t serverB_addr_size = sizeof(serverB_addr);
  socklen_t serverC_addr_size = sizeof(serverC_addr);

  char bufRecvFromA[BUF_SIZE];
  char bufRecvFromB[BUF_SIZE];
  char bufRecvFromC[BUF_SIZE];
  int results[3];

  //接收A
  int strLen = recvfrom(sockD, bufRecvFromA, BUF_SIZE, 0, &serverA_addr, &serverA_addr_size);
  int results[0] = atoi(bufRecvFromA);

  //接收B
  int strLen = recvfrom(sockD, bufRecvFromB, BUF_SIZE, 0, &serverA_addr, &serverA_addr_size);
  int results[1] = atoi(bufRecvFromB);

  //接收C
  int strLen = recvfrom(sockD, bufRecvFromC, BUF_SIZE, 0, &serverC_addr, &serverC_addr_size);
  int result[2] = atoi(bufRecvFromC);

  //处理来自ABC的数据
  char max_func[] = "max";
  char min_func[] = "min";
  char sum_func[] = "sum";
  char sos_func[] = "sos";
  int finalResult;

  if (strcmp(function_name, max_func) == 0){
    //get max
    finalResult = get_max(results, 3);
  }else if (strcmp(function_name, min_func) == 0) {
    //get min
    finalResult = get_min(results, 3);
  }else if (strcmp(function_name, sum_func) == 0){
    //get sum
    finalResult = get_sum(results, 3);
  }else if (strcmp(function_name, sos_func) == 0){
    //get sos
    finalResult = get_sum(results, 3);
  }else {
    printf("No such operation.\n");
  }
  printf("The final result is: %d\n", finalResult);




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