#include "socketUtility.h"


int getServerSocket(string ip,string port){
    int server_fd; 
    struct sockaddr_in address; 
    int opt = 1; 
    // char buffer[1024] = {0}; 
    string s;
      
    int portno = atoi(port.c_str());
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    // address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( portno ); 
    if(inet_pton(AF_INET, ip.c_str(), &address.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        exit(1);
    } 
       
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    if (::bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 10) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    // cout<<address.sin_addr.s_addr;
    return server_fd;
}
