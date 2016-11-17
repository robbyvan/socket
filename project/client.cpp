#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define BUF_SIZE 1000
#define LONG_BUF 3*sizeof(long int)*sizeof(long int)

int main(int argc, char const *argv[]){
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(25807);

  char bufSendToServ[BUF_SIZE] = {'\0'};
  char bufRecvFromServ[LONG_BUF] = {'\0'};

  //First send the reduction type to AWS
  strcpy(bufSendToServ, argv[1]);
  // printf("Gonna send: %s to server\n", bufSendToServ);

  int sock = socket(PF_INET, SOCK_STREAM, 0);
  printf("The client is up and running.\n");
  connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
  send(sock, bufSendToServ, strlen(bufSendToServ), 0);

  printf("The client has sent the reduction type <%s> to AWS.\n", bufSendToServ);

  //send numbers
  int numCount = 0;

  FILE *fp = fopen("./nums.csv", "rb");
  if (fp == NULL){
    printf("Can not open the file\n");
  }else{
    // printf("Open Success.\n");
  }
  
  while(!feof(fp)){
    fscanf(fp, "%s\n", &bufSendToServ);
    numCount += 1;

    // printf("gonna send: %s to AWS\n", bufSendToServ);
    send(sock, bufSendToServ, sizeof(long int), 0);
    memset(bufSendToServ, 0, BUF_SIZE);
  }

  //close file 
  fclose(fp);

  printf("The client has sent <%d> numbers to AWS\n", numCount);

  //send the finish message to AWS
  char finMsg[] = "Fin";
  strcpy(bufSendToServ, finMsg);
  
  send(sock, bufSendToServ, strlen(bufSendToServ), 0);
  // printf("sent:'%s' to AWS\n", finMsg);
  
  memset(bufSendToServ, 0, BUF_SIZE);

  //Receive the final result from AWS
  char max_func[] = "max";
  char min_func[] = "min";
  char sum_func[] = "sum";
  char sos_func[] = "sos";

  recv(sock, bufRecvFromServ, BUF_SIZE, 0);
  // printf("String is: %s\n", bufRecvFromServ);
  int finalResult = atoi(bufRecvFromServ);
  printf("The client has received reduction <%s>: %d\n",argv[1], finalResult);

  close(sock);

  return 0;
}
