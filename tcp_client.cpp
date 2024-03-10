#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "/data/data/com.termux/files/home/headers/colors.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define LOCAL_HOST "127.0.0.1"

void clientinfo(){
  int sock_fd;
  struct sockaddr_in server_addr;
  char ip_address[INET_ADDRSTRLEN];
  
  //Create socket
  if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror(linkColor("[socket failed si!]", SZ_DEFAULT, Red));
    return;
  }
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, LOCAL_HOST, &(server_addr.sin_addr));
  
  //Connect to server to get client
  if(connect(sock_fd, (struct sockaddr* )&server_addr, sizeof(server_addr)) < 0){
    perror(linkColor("[Connection Failed]", SZ_DEFAULT, Red));
  }
  
  inet_ntop(AF_INET, &(server_addr.sin_addr), ip_address, INET_ADDRSTRLEN);
  std::cout << linkColor("Client IP: ", SZ_DEFAULT, Red) << ip_address << std::endl;
  close(sock_fd);
}

int main(){
  
  while(true){
    int sock{0}, response;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE]{0};
    std::string message;
    
    system("clear");
    clientinfo();
    std::cout << "\n> ";
    getline(std::cin, message);
    
    //Create socket file descriptor
    if( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
      perror("Socket creation failed!");
      exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    //Convert IPv4 and IPv6 addresses from text to binary
    if(inet_pton(AF_INET, LOCAL_HOST, &serv_addr.sin_addr) <= 0){
      perror("Invalid address / Address not supported!");
      exit(EXIT_FAILURE);
    }
    
    //Connect to server
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
      perror(linkColor("[Conection Failed]", SZ_DEFAULT, Red));
      exit(EXIT_FAILURE);
    }
    
    //Send message to server
    send(sock, message.c_str(), message.size(), 0);
    std::cout << "Message has been sent to the server!\nPls check out the server\n";
    
    //Receive response from server
    response = read(sock, buffer, BUFFER_SIZE);
    if(response < 0)
      perror(linkColor("no response from server!", SZ_DEFAULT, Red));
    else
      std::cout << linkColor(buffer,SZ_DEFAULT, Cyan);
      
    getchar();
  }
  
  return 0;
}