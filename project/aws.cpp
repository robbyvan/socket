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

/**/
int get_max(int *nums, int sample_volume);
int get_min(int *nums, int sample_volume);
int get_sum(int *nums, int sample_volume);
int get_sos(int *nums, int sample_volume);
void DieWithError(char *errorMessage) {
   perror(errorMessage);
   exit(1);
 }
/**/

int main(){
  int backsock;
  printf("\n---Phase 1 Start---\n\n");
  //UDP socket
  int sockD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  struct sockaddr_in server_D_addr;
  memset(&server_D_addr, 0, sizeof(server_D_addr));
  server_D_addr.sin_family = PF_INET;
  server_D_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_D_addr.sin_port = htons(24807);
  bind(sockD, (struct sockaddr*)&server_D_addr, sizeof(server_D_addr));
  printf("The Server D is up and running using UDP on port <24807>.\n");

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
    // struct sockaddr_in client_addr;
    // socklen_t client_addr_size = sizeof(client_addr);

    //accept后新建一个交换数据的client_sock
    int client_sock = accept(tcp_origin_sock, (struct sockaddr*)&client_addr, &client_addr_size);
    // if ( (buffer_len = recv(client_sock, bufRecvFromClnt, BUF_SIZE, 0)) >= 0){
    countClient++;
    // }

    backsock = client_sock;
    // char bufRecvFromClnt[BUF_SIZE] = {'\0'};
    int buffer_len = recv(client_sock, bufRecvFromClnt, BUF_SIZE, 0);
    
    //如果还没到结尾, 即收到数字, 往文件里写
    if (strcmp(bufRecvFromClnt, clientFin) != 0){
      printf("IP of Fin socket is: %u     ", client_addr.sin_addr.s_addr);
      printf("Port of Fin socket is: %u\n", client_addr.sin_port);
      printf("%s\n", bufRecvFromClnt);
      fprintf(fpRecvNum, "%s\n", bufRecvFromClnt);
      close(client_sock);
      printf("The client_socket: %d has closed.      ", client_sock);
      printf("The backsock is: %d\n", backsock);
      memset(bufRecvFromClnt, 0, BUF_SIZE);
    }

    //如果收到的是结尾
    if (strcmp(bufRecvFromClnt, clientFin) == 0){
      backsock = client_sock;
      printf("IP of Fin socket is: %u\n", client_addr.sin_addr.s_addr);
      printf("Port of Fin socket is: %u\n", client_addr.sin_port);
      printf("The client_socket is %d.     ", client_sock);
      printf("The backsock is: %d\n", backsock);
      fclose(fpRecvNum);
      printf("file closed.\n");
      // printf("Address of the client is: %d\n", client_addr.sin_addr.s_addr);
      // printf("Port of the client is: %d\n", ntohs(client_addr.sin_port));
      // close(client_sock);//关闭这次数据交换的socket
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
      // for (i = 0; i < countClient; i++){
        // printf("%d\n", nums[i]);
      // }
      break;
    }
  }

  // close(tcp_origin_sock);
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




  //lines above are phase 2
  printf("\n---Phase 2 End---\n\n---Phase 3 Start---\n");

  //接收从服务器A/B/C返回的结果

  struct sockaddr server_A_addr;
  struct sockaddr server_B_addr;
  struct sockaddr server_C_addr;
  socklen_t server_A_addr_size = sizeof(server_A_addr);
  socklen_t server_B_addr_size = sizeof(server_B_addr);
  socklen_t server_C_addr_size = sizeof(server_C_addr);

  char bufRecvFromABC[BUF_SIZE];
  int results[3];
  int strLen;

  //接收A
  strLen = recvfrom(sockD, bufRecvFromABC, BUF_SIZE, 0, &server_A_addr, &server_A_addr_size);
  results[0] = atoi(bufRecvFromABC);
  printf("received result from server A, it is: %d\n", results[0]);

  //接收B
  strLen = recvfrom(sockD, bufRecvFromABC, BUF_SIZE, 0, &server_B_addr, &server_B_addr_size);
  results[1] = atoi(bufRecvFromABC);
  printf("received result from server B, it is: %d\n", results[1]);

  //接收C
  strLen = recvfrom(sockD, bufRecvFromABC, BUF_SIZE, 0, &server_C_addr, &server_C_addr_size);
  results[2] = atoi(bufRecvFromABC);
  printf("received result from server C, it is: %d\n", results[2]);

  close(sockD);

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

  //返回给client数据
  printf("Check if Fin Socket's client_addr info is still there:\n");
  printf("The client_socket id is: %d.\n", client_sock);
  printf("The backsock is: %d\n", backsock);
  printf("IP of Fin socket is: %u     ", client_addr.sin_addr.s_addr);
  printf("Port of Fin socket is: %u\n", client_addr.sin_port);
  char bufSendToClient[BUF_SIZE] = {'\0'};

  memset(bufSendToClient, 0, BUF_SIZE);
  sprintf(bufSendToClient, "%d", finalResult);//整数转字符串

  printf("The final result string gonna send through send() function: %s\n", bufSendToClient);

  sleep(1);//need to slow down send back the result throgh the Fin socket.
  //it seems that the TCP are created during loop, not at the very beginning, 
  //seems it needs time to be active.

  send(backsock, bufSendToClient, BUF_SIZE, 0);//还是用的接收fin包的socket

  printf("Sent final result to client.\n");

  close(backsock);//关数据交换socket
  close(tcp_origin_sock);//关握手socket

  printf("\n---Phase 3 End---\n");



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