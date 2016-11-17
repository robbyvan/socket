##GitHub
  [Socket programming](https://github.com/robbyvan/socket)

##Objective
To implement a simple model of computational offloading where a single client offloads some computation to a server which in turn distributes the load over 3 backend servers. The server facing the client then collects the results from the backend and communicates the same to the client in the required format.

##About code files
There are 5 files in total
+ **client.cpp**
  - Create TCP socket with AWS
  - Read loacal file "nums.csv" and send all numbers to AWS
  - Send a finish message (which is defined as 'Fin') to AWS to denote it finishes sending bumbers
  - Waiting to receive the final result from AWS
+ **aws.cpp**
  - Create a UDP socket(later will use)
  - Create a TCP hand-shaking socket ('tcp_origin_sock')
  - Accept TCP connection from client ('client_sock')
  - Receiving from client
    + First receive reduction type
    + Then receive numbers from client until it receives the 'Fin' message
  - Distribute all numbers with same volume to backend servers A/B/C, through UDP socket
    + First send volume of numbers each backend server gonna deal with
    + Then send reduction type
    + At last, send numbers
  - Waiting to receive results from backend servers
  - Collect results from backend servers and do the final reduction
  - Send the final result to client through 'client_sock'
  - Close hand-shaking socket and data-exchanging socket
+ **servera.cpp** **serverb.cpp** **serverc.cpp**
  - Receiving from AWS
    + Receive volume of numbers
    + Receive reduction type
    + Receive numbers
  - Do the reduction
  - Send result back to AWS
  - Close UDP socket

##To run the program
  1. open five terminals
  2. Use 'make all' command to compile all files
  3. Type 'make serverA' / 'make serverB' / 'make serverC' / 'make AWS' respectively in 4 diffrent terminals to run Server A B C & AWS
  4. In the last terminal, type './client <reduction_type>', there are four reduction types which are 'max', 'min', 'sum', 'sos'. Eg: './client max'
  5.Then messages will display on each terminal.

##Format of all messages exchanged
  All messages are exchanged as ASCII.

##Idiosyncrasy of the project
  In the 1st version of my program, when I test on my own PC, everything is fine because the network is good. However on nunki, sometimes when the network is congested, 'numbers' may pile up after the 'reduction name' in the receiving buffer, which may cause the reduction name received incorrectly. And this could lead the program to fail. Also, it's because I did not limit the exact length to get from buffer when receiving reduction type at the 1st version.

  In order avoid this, I think it is better to send numbers first and then goes reduction type. Because even if numbers pile up in the buffer, we can limit the size as 'sizeof(long int)' when receiving, which means we can always get numbers from buffer correclty. As in general, we may have more reduction types and it may not all be the same length, so it is hard to tell how big the size is if we revceive it first.

  But in the end, I still receive reduction type first. I limitted the size when receiving reduction type, where I set it to 'strlen(fun_name_model)' in the routine. The 'fun_name_model' is a 3 character arbitraty string, all I need is the size. So this may prevent the 'pile up in the buffer' problem. I've tested several times on numki and everything went all well.

###Reused code
  All routines are wriiten by myself.