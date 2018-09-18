
// Client side C/C++ program to demonstrate Socket programming 
#include <bits/stdc++.h> 
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <fcntl.h>
#include <arpa/inet.h>
#define PORT 8080
using namespace std;
int main(int argc, char const *argv[]) 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    // char hello[] = "Hello from client"; 
    char buffer[1024*512] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\nClient Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
        
        FILE *fp;
        fp = fopen("error4.txt", "wb"); 
        while(1){
            valread = read( sock , buffer, 1024*512); 
            cout<<valread<<endl;
            if (valread == 0)
            break;
            if (valread == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (fwrite(buffer,1, valread,fp) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
    
    return 0; 
} 
