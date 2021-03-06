/*
USC ID: xxxxx-xx807
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

#define BUF_SIZE 1000
#define LONG_BUF 3*sizeof(long int)*sizeof(long int)

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
  // printf("\n---Phase 1 Start---\n");
  //UDP socket
  int sockD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  struct sockaddr_in server_D_addr;
  memset(&server_D_addr, 0, sizeof(server_D_addr));
  server_D_addr.sin_family = PF_INET;
  server_D_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_D_addr.sin_port = htons(24807);
  bind(sockD, (struct sockaddr*)&server_D_addr, sizeof(server_D_addr));
  // printf("The Server D is up and running using UDP on port <24807>.\n");

  //The socket to be used as TCP handshaking
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
  int countClient = -1;
  char clientFin[] = "Fin";

  //First receives reduction type from client.
  char fun_name_model[] = "fun";
  struct sockaddr_in client_addr;
  socklen_t client_addr_size = sizeof(client_addr);
  int client_sock = accept(tcp_origin_sock, (struct sockaddr*)&client_addr, &client_addr_size);

  char bufRecvFromClnt[BUF_SIZE] = {'\0'};
  int buffer_len = recv(client_sock, bufRecvFromClnt, strlen(fun_name_model), 0);

  strcpy(function_name, bufRecvFromClnt);
  function_name[3] = '\0';
  // printf("Received function name: %s at 1st connection.\n", function_name);

  //Start to receive numbers, save them all in nums[3000] array
  int nums[3000] = {'\0'};
  
  while(1){
    
    int buffer_len = recv(client_sock, bufRecvFromClnt, sizeof(long int), 0);
    
    countClient++;//first time: Client count = -1 + 1 = 0

    //if not finish receiving, i.e didn't receive the "fin" message.
    if (strcmp(bufRecvFromClnt, clientFin) != 0){
      // printf("%d-th received: ", countClient);
      // printf("%s\n", bufRecvFromClnt);
      nums[countClient] = atoi(bufRecvFromClnt);
      memset(bufRecvFromClnt, 0, BUF_SIZE);
    }

    //if client finishes transmitting numbers
    if (strcmp(bufRecvFromClnt, clientFin) == 0){
      printf("The AWS has received %d numbers from the client using TCP over port <25807>.\n", countClient);
      
      break;
    }
  }

  /*lines above are Phase 1*/

  // printf("---Phase 1 End---\n\n---Phase 2 Start---\n");

  //1a)send the volume of numbers server A gonna deal with
  int sockA = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // printf("sockA created.\n");

  struct sockaddr_in serverA_addr;
  memset(&serverA_addr, 0, sizeof(serverA_addr));
  serverA_addr.sin_family = PF_INET;
  serverA_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverA_addr.sin_port = htons(21807);

  int sample_volume = countClient/3;

  char bufSendToA[BUF_SIZE] = {'\0'};

  memset(bufSendToA, 0, BUF_SIZE);
  sprintf(bufSendToA, "%d", sample_volume);//int to ascii

  sendto(sockA, bufSendToA, strlen(bufSendToA), 0, (struct sockaddr*)&serverA_addr, sizeof(serverA_addr));
  memset(bufSendToA, 0, BUF_SIZE);

  //1b)send the volume of numbers server B gonna deal with
  int sockB = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // printf("sockB created.\n");

  struct sockaddr_in serverB_addr;
  memset(&serverB_addr, 0, sizeof(serverB_addr));
  serverB_addr.sin_family = PF_INET;
  serverB_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverB_addr.sin_port = htons(22807);

  char bufSendToB[BUF_SIZE] = {'\0'};

  memset(bufSendToB, 0, BUF_SIZE);
  sprintf(bufSendToB, "%d", sample_volume);// int to ascii

  sendto(sockB, bufSendToB, strlen(bufSendToB), 0, (struct sockaddr*)&serverB_addr, sizeof(serverB_addr));
  memset(bufSendToB, 0, BUF_SIZE);

//1c)send the volume of numbers server C gonna deal with
  int sockC = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  // printf("sockC created.\n");

  struct sockaddr_in serverC_addr;
  memset(&serverC_addr, 0, sizeof(serverC_addr));
  serverC_addr.sin_family = PF_INET;
  serverC_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverC_addr.sin_port = htons(23807);

  char bufSendToC[BUF_SIZE] = {'\0'};

  memset(bufSendToC, 0, BUF_SIZE);
  sprintf(bufSendToC, "%d", sample_volume);//int to ascii

  sendto(sockC, bufSendToC, strlen(bufSendToC), 0, (struct sockaddr*)&serverC_addr, sizeof(serverC_addr));
  memset(bufSendToC, 0, BUF_SIZE);


  //2a)send the reduction type to A
  strcpy(bufSendToA, function_name);
  
  // printf("function name is: %s\n", bufSendToA);

  sendto(sockA, bufSendToA, strlen(bufSendToA), 0, (struct sockaddr*)&serverA_addr, sizeof(serverA_addr));
  memset(bufSendToA, 0, BUF_SIZE);

  //2b)send the reduction type to B
  strcpy(bufSendToB, function_name);

  // printf("function name is: %s\n", bufSendToB);

  sendto(sockB, bufSendToB, strlen(bufSendToB), 0, (struct sockaddr*)&serverB_addr, sizeof(serverB_addr));
  memset(bufSendToB, 0, BUF_SIZE);

  //2c)send the reduction type to C
  strcpy(bufSendToC, function_name);
  
  // printf("function name is: %s\n", bufSendToC);

  sendto(sockC, bufSendToC, strlen(bufSendToC), 0, (struct sockaddr*)&serverC_addr, sizeof(serverC_addr));
  memset(bufSendToC, 0, BUF_SIZE);


  //3a)send the numbers to A
  int i = 0;
  for (; i < sample_volume; i++){
    // memset(bufSendToA, 0, BUF_SIZE);
    sprintf(bufSendToA, "%d", nums[i]);//int to ascii

    // printf("sending: %s to Server A, %d-th\n", bufSendToA, i);
    sendto(sockA, bufSendToA, strlen(bufSendToA), 0, (struct sockaddr*)&serverA_addr, sizeof(serverA_addr));
  }
  printf("The AWS sent <%d> numbers to Backend­Server <A>\n", sample_volume);
  close(sockA);

  //3b)send the numbers to B
  for (/*int i = sample_volume*/; i < (2*sample_volume); ++i){
    sprintf(bufSendToB, "%d", nums[i]);//int to ascii
    // printf("sending: %s to Server B, %d-th\n", bufSendToB, i);
    sendto(sockB, bufSendToB, strlen(bufSendToB), 0, (struct sockaddr*)&serverB_addr, sizeof(serverB_addr));
  }
  printf("The AWS sent <%d> numbers to Backend­Server <B>\n", sample_volume);
  close(sockB);

  //3b)send the numbers to C
  for (; i < countClient; ++i){
    sprintf(bufSendToC, "%d", nums[i]);//int to ascii
    // printf("sending: %s to Server C, %d-th\n", bufSendToC, i);
    sendto(sockC, bufSendToC, strlen(bufSendToC), 0, (struct sockaddr*)&serverC_addr, sizeof(serverC_addr));
  }
  printf("The AWS sent <%d> numbers to Backend­Server <C>\n", sample_volume);
  close(sockC);
  //lines above are phase 2
  // printf("---Phase 2 End---\n\n---Phase 3 Start---\n");

  //Start to receive results from back-end servers
  struct sockaddr server_A_addr;
  struct sockaddr server_B_addr;
  struct sockaddr server_C_addr;
  socklen_t server_A_addr_size = sizeof(server_A_addr);
  socklen_t server_B_addr_size = sizeof(server_B_addr);
  socklen_t server_C_addr_size = sizeof(server_C_addr);

  char bufRecvFromABC[LONG_BUF] = {'\0'};
  int results[3];
  int strLen;

  char max_func[] = "max";
  char min_func[] = "min";
  char sum_func[] = "sum";
  char sos_func[] = "sos";

  //receive from A
  strLen = recvfrom(sockD, bufRecvFromABC, LONG_BUF, 0, &server_A_addr, &server_A_addr_size);
  results[0] = atoi(bufRecvFromABC);
  printf("The AWS received reduction result of <%s> from Backend­Server <A> using UDP over port <21807> and it is %d\n",function_name, results[0]);

  //receive from B
  strLen = recvfrom(sockD, bufRecvFromABC, LONG_BUF, 0, &server_B_addr, &server_B_addr_size);
  results[1] = atoi(bufRecvFromABC);
  printf("The AWS received reduction result of <%s> from Backend­Server <B> using UDP over port <22807> and it is %d\n",function_name, results[1]);

  //receive from C
  strLen = recvfrom(sockD, bufRecvFromABC, LONG_BUF, 0, &server_C_addr, &server_C_addr_size);
  results[2] = atoi(bufRecvFromABC);
  printf("The AWS received reduction result of <%s> from Backend­Server <C> using UDP over port <23807> and it is %d\n",function_name, results[2]);

  close(sockD);

  //AWS do the final reduction
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
  printf("The AWS has successfully finished the reduction <%s>: %d\n",function_name, finalResult);

  //send final result back to client
  // printf("The client_socket id is: %d.\n", client_sock);
  // printf("IP of client_socket is: %u    ", client_addr.sin_addr.s_addr);
  // printf("Port of client_socket is: %u\n", client_addr.sin_port);
  char bufSendToClient[LONG_BUF] = {'\0'};

  memset(bufSendToClient, 0, LONG_BUF);
  sprintf(bufSendToClient, "%d", finalResult);//int to ascii

  // printf("The final result string gonna send through send() function: %s\n", bufSendToClient);

  int counter = send(client_sock, bufSendToClient, LONG_BUF, 0);
  if(counter != -1){
    printf("The AWS has successfully finished sending the reduction value to client.\n");
  }

  close(client_sock);//close the data exchanging socket
  close(tcp_origin_sock);//close the hand-shaking socket

  // printf("---Phase 3 End---\n\n");

  return 0;
}

int get_max(int *nums, int sample_volume){
  int temp = nums[0];
  for (int i = 1; i < sample_volume; ++i){
    if (nums[i] > temp){
      temp = nums[i];
    }
  }
  return temp;
}
int get_min(int *nums, int sample_volume){
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
