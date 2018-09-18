#include <bits/stdc++.h> 
#include <unistd.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <netinet/in.h>
#define PORT 8080 
using namespace std;
int main(int arg, char *args[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    struct sockaddr_in CAddress; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    string s;
       
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
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    int count=0;
 while(true){
    if ((new_socket = accept(server_fd, (struct sockaddr *)&CAddress,(socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    count++;
    cout<<count<<endl;
    if(fork() == 0) {
            
            FILE *fp = fopen("error.txt","rb");
            size_t size;
            unsigned char inputB[1024*512]={0};
            while ((size = fread( inputB,1, 1024*512,fp)) > 0) {
                cout<<size<<"\n";
                write(new_socket , inputB , size );     
            }       
            close(new_socket);
            exit(0);
        
    }
    else{
            close(new_socket);
    }
 }
    return 0; 
} 
