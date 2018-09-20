
// Client side C/C++ program to demonstrate Socket programming 
#include <bits/stdc++.h> 
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <fcntl.h>
#include <arpa/inet.h>
#include"utility.h"
#include"classes.h"
#define CHUNK 1024*512
using namespace std;


string opcode,arg1,filePath,mFileName,mFilePath, destinationPath;
// string cip,cport;
string cipAndPort;
string t1ip,t1port;
void readCommands(){
    cin>>opcode;
    if(opcode=="share"){
        cin>>filePath;
        cin>>mFileName;
    }
    else if(opcode=="get"){
        cin>>mFilePath;
        cin>>destinationPath;
    }
    else{
        cout<<"Invalid operation\n";
    }

}

void processCommand(int sock, string tracker1, string tracker2){
    if(opcode=="share"){
        char buffer[1024]={0};
        getcwd(buffer,1024);
        string pwd=buffer;

        string inFile=filePath;
        string outFile=pwd+"/"+mFileName;
        MTorrent m=createMtorrent(inFile,outFile, tracker1, tracker2);
        string res=opcode+"|"+cipAndPort+"|"+filePath+"|"+m.hashOfFile;
        send(sock , res.c_str() , res.size() , 0 ); 
        // send(sock , opcode.c_str() , opcode.size() , 0 ); 
        // send(sock , cipAndPort.c_str() , cipAndPort.size() , 0 ); 
        // send(sock , filePath.c_str() , filePath.size() , 0 ); 
        // // send(sock , m.hashStr.c_str() , m.hashStr.size() , 0 ); 
        // send(sock , m.hashOfFile.c_str() , m.hashStr.size() , 0 ); 

    }
    if (opcode=="get")
    {
        MTorrent m=readMtorrent(mFilePath);
        string data;
        string res=opcode+"|"+m.hashOfFile;
        send(sock , res.c_str() , res.size() , 0 ); 
        // send(sock , opcode.c_str() , 1024 , 0 ); 
        // send(sock , m.hashOfFile.c_str() ,m.hashOfFile.size() , 0 ); 
        char buffer[1024] = {0}; 
        // int valread = read( sock , buffer, 1024); 
        while(1){
            int valread = read( sock , buffer, 1024); 
            if (valread == 0)
            break;
            if (valread == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            data=buffer;
            cout<<data<<endl;
        }

    }
}

int main(int arg, char  *args[]) 
{ 
    
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 

    if (arg < 5) {
         cout<<"No Tracker ip and port provided\n";
         exit(1);
    }   

    //client id port
    cipAndPort=args[1];
    cout<<cipAndPort<<endl;
    //getting ip and port
    string idAndPort = args[2];
    int pos=idAndPort.find(":");
    t1ip = idAndPort.substr(0, pos); 
    t1port = idAndPort.substr(pos+1,idAndPort.size()); 

    int portno = atoi(t1port.c_str());
    char buffer[CHUNK] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\nClient Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(portno); 
           
    if(inet_pton(AF_INET, t1ip.c_str(), &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
        printf("\nConnection Established \n"); 
        readCommands();
        processCommand(sock,args[2],args[3]);
        // send(sock , opcode.c_str() , opcode.size() , 0 ); 
        // FILE *fp;
        // fp = fopen("error2.txt", "wb"); 
        // while(1){
        //     valread = read( sock , buffer, CHUNK); 
        //     cout<<valread<<endl;
        //     if (valread == 0)
        //     break;
        //     if (valread == -1) {
        //         perror("read");
        //         exit(EXIT_FAILURE);
        //     }
        //     if (fwrite(buffer,1, valread,fp) <0) {
        //         perror("write");
        //         exit(EXIT_FAILURE);
        //     }
        // }
    
    return 0; 
} 
