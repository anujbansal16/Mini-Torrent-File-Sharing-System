#include <bits/stdc++.h> 
#include <unistd.h>
#include <sys/socket.h> 
#include <stdlib.h> 
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fstream>
#include "socketUtility.h"
#define PORT 8080 
using namespace std;
static map<string,vector<ClientData>> mapHashToClient;
ClientData getClientData(char inputBuffer[], string token){
  vector<string> words;
  char* word = strtok(inputBuffer, token.c_str());
  while (word != NULL) {
    words.push_back(word);
    word = strtok(NULL, token.c_str());
  }
    ClientData c;
    c.hash=words[0];
    c.filePath=words[1];
    c.clienAddr=words[2]; 
    return c;
}

void readSeederfile(string seederFile){
    cout<<"in read readSeederfile\n";
    // seederFile="seedList.txt";
    FILE *fp;  
    char buff[255];
    ClientData c;    
    ifstream in(seederFile);
    if(!in) {
        cout << "Cannot open seederFile file.\n";
        exit(1);
    }   
    char str[255];
    while(in) {
        in.getline(str, 255);
        if(in) {
            vector<string> words;
            c=getClientData(str,"|");
            mapHashToClient[c.hash].push_back(c);
        }
    }
    in.close();
    for (auto i= mapHashToClient.begin(); i != mapHashToClient.end(); ++i)
    {   
        cout<<i->first<<endl;
        for (vector<ClientData>::iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
                cout<<(*j).clienAddr<<endl;
        }
    }
}

void readSocketBuff(int sock, string seederFile){
    // seederFile="seedList.txt";
    cout<<"in readSocketBuff\n";
    FILE *fp;
    std::vector<string> clientData;
    while(1){
        char buffer[1024]={0};
        int valread = read( sock , buffer, 1024); 
        string data=buffer;
        if (valread == 0)
        break;
        // cout<<buffer;
        if (valread == -1) {
            perror("read");
            break;
            // exit(EXIT_FAILURE);
        }
        clientData.push_back(data);
        
    }    
    ClientData c;
    c.hash=clientData[2];
    c.filePath=clientData[1];
    c.clienAddr=clientData[0];
    mapHashToClient[c.hash].push_back(c);
    fp = fopen(seederFile.c_str(), "ab"); 
    fprintf(fp,"%s|", clientData[2].c_str());
    fprintf(fp,"%s|", clientData[1].c_str());
    fprintf(fp,"%s|", clientData[0].c_str());
    fprintf(fp,"\n");//endl
    fclose(fp);
    // readSeederfile("");
    for (auto i= mapHashToClient.begin(); i != mapHashToClient.end(); ++i)
    {   
        cout<<i->first<<endl;
        for (vector<ClientData>::iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
                cout<<(*j).clienAddr<<endl;
        }
    }
}

int main(int arg, char *args[]) 
{ 
    struct sockaddr_in CAddress; 
    int new_socket;
    int addrlen = sizeof(CAddress); 
    if (arg < 5) {
         cout<<"No Tracker ip and port provided";
         exit(1);
    }
    cout<<"main"<<endl;
    //getting ip and port
    string ip,port;
    string idAndPort = args[1];
    int pos=idAndPort.find(":");
    ip = idAndPort.substr(0, pos); 
    port = idAndPort.substr(pos+1,idAndPort.size()); 
    int server_fd=getServerSocket(ip,port);
    int count=0;
    readSeederfile(args[3]);
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
                // cout<<opcode<<"\n";
                if(opcode=="share"){
                    readSocketBuff(new_socket,args[3]);
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
