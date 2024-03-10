#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "/data/data/com.termux/files/home/headers/colors.h"

#define PORT 8080
#define BUFFER_SIZE 100

void* handle_client(void* arg){
  int client_socket = *((int* )arg); // casting void* to int*, then deference to int
  char buffer[BUFFER_SIZE]{0};
  std::string message{ linkColor("[Established]", 13, Cyan) };
  
  //Receive data from client
  int valread = read(client_socket, buffer, BUFFER_SIZE);
  int check = strlen(buffer);
  if(check < BUFFER_SIZE){
    std::cout << buffer << linkColor("[Received]",10, Cyan) << std::endl;
  }
  else{
    std::cout << linkColor("[Data lost]", 11, Red) << std::endl;
    message = linkColor("[Buffer limit]", SZ_DEFAULT, Red);
  }
  //Send response to client
  send(client_socket, message.c_str(), message.size(), 0);
  // std::cout << "Message has been sent!\n";
  close(client_socket);
  
  return NULL;
}

void serverinfo(){
  int sock_fd;
  struct sockaddr_in server_addr;
  char ip_address[INET_ADDRSTRLEN];
  
  //Create socket
  if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror(linkColor("[socket failed si!]", SZ_DEFAULT, Red));
  }
  
  //Fill server address struct
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  
  //Convert IP to string
  inet_ntop(AF_INET, &(server_addr.sin_addr), ip_address, INET_ADDRSTRLEN);
  std::cout << linkColor("Server IP: ", SZ_DEFAULT, Fuchsia) << ip_address << std::endl;
  
  close(sock_fd);
}

int main(){
  
  int server_fd, new_socket, opt{1};
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  
  //Create Socket File Descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if( server_fd == 0 ){
    perror(linkColor("Socket Failed!", SZ_DEFAULT, Red));
    exit(EXIT_FAILURE);
  }
  
  //Set Socket Options
  if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
    perror(linkColor("Set sockopt!", SZ_DEFAULT, Red));
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);
  
  //Bind socket to address and port
  if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
    perror(linkColor("Bind process has failed!", SZ_DEFAULT, Red));
    exit(EXIT_FAILURE);
  }
  
  //Listen for incoming connections
  if(listen(server_fd, 3) < 0){
    perror(linkColor("Listen failed!", SZ_DEFAULT, Red));
    exit(EXIT_FAILURE);
  }
  
  serverinfo();
  while(true){
    //Accept incoming connections
    new_socket = accept(server_fd, (struct sockaddr* )&address, (socklen_t* )&addrlen);
    if(new_socket < 0){
      perror(linkColor("Socket conection denied!", SZ_DEFAULT, Red));
      exit(EXIT_FAILURE);
    }
    
    //Creat a new thread to handle the client connection
    pthread_t thread_id;
    if(pthread_create(&thread_id, NULL, handle_client, (void *)&new_socket) < 0){
      perror("Pthread create!");
      exit(EXIT_FAILURE);
    }
    
    //Detach the thread (allow it to run independently)
    if(pthread_detach(thread_id) < 0){
      perror("Pthread_detach!");
      exit(EXIT_FAILURE);
    }
    
  }
  
  return 0;
}