
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

vector<string> tokenize(char inputBuffer[], string token){
    vector<string> words;
    char* word = strtok(inputBuffer, token.c_str());
    while (word != NULL) {
        words.push_back(word);
        word = strtok(NULL, token.c_str());
    }  
    return words;
}

string opcode,arg1,filePath,mFileName,mFilePath, destinationPath;
// string cip,cport;
string cipAndPort;
string t1ip,t1port;

void startOwnTread(){


}

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
        string res=opcode+"|"+cipAndPort+"|"+filePath+"|"+m.hashOfFile+"|"+"end";
        send(sock , res.c_str() , res.size() , 0 );

    }
    if (opcode=="get")
    {
        MTorrent m=readMtorrent(mFilePath);
        string data,res;
        string sndData=opcode+"|"+m.hashOfFile+"|"+"end";
        send(sock , sndData.c_str() , sndData.size()+10 , 0 ); 
        char buffer[1024] = {0}; 
        while(1){
            cout<<"client"<<endl;
            int valread = read( sock , buffer, 1024);
            cout<<valread<<endl; 
            if (valread == 0)
            break;
            if (valread == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            data=buffer;
            cout<<data<<endl;
            res=res+buffer;
            char result[res.size()+2];
            strcpy(result, res.c_str());  
            vector<string> v=tokenize(result,"|");    
            if(v[v.size()-1]=="end")
               break;
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
    startOwnTread();
    return 0; 
} 
