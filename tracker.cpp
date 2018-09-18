#include <bits/stdc++.h> 
#include <unistd.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socketUtility.h"
#define PORT 8080 
using namespace std;

void readSocketBuff(int sock){
    cout<<"in readSocketBuff\n";
    while(1){
        char buffer[1024]={0};
        int valread = read( sock , buffer, 1024); 
        cout<<buffer<<endl;
        if (valread == 0)
        break;
        if (valread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        // if (fwrite(buffer,1, valread,fp) <0) {
        //     perror("write");
        //     exit(EXIT_FAILURE);
        // }
    }    
}

int main(int arg, char *args[]) 
{ 
    struct sockaddr_in CAddress; 
    int new_socket;
    int addrlen = sizeof(CAddress); 
    if (arg < 2) {
         cout<<"No Tracker ip and port provided";
         exit(1);
    }

    //getting ip and port
    string ip,port;
    string idAndPort = args[1];
    int pos=idAndPort.find(":");
    ip = idAndPort.substr(0, pos); 
    port = idAndPort.substr(pos+1,idAndPort.size()); 
    int server_fd=getServerSocket(ip,port);

    int count=0;
    while(true){
        if ((new_socket = accept(server_fd, (struct sockaddr *)&CAddress,(socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 
        
        count++;           
        if(fork() == 0) {
                cout<<"infork\n";
                char buffer[1024]={0};
                int valread = read( new_socket , buffer, 1024); 
                string opcode=buffer;
                cout<<opcode<<"\n";
                if(opcode=="share"){
                    readSocketBuff(new_socket);
                }
                // FILE *fp = fopen("error.txt","rb");
                // size_t size;
                // unsigned char inputB[1024*512]={0};
                // while ((size = fread(inputB,1, 1024*512,fp)) > 0) {
                //     cout<<size<<"\n";
                //     write(new_socket , inputB , size );     
                // }       
                close(new_socket);
                exit(0);
        }
        else{
                close(new_socket);
        }
    }
    return 0; 
} 
