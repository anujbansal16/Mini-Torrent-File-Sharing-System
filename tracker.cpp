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
struct ThreadParam
{
    int sok;
    string seederFile;
};
static map<string,vector<ClientData>> mapHashToClient;

vector<string> tokenize(char inputBuffer[], string token){
    vector<string> words;
    char* word = strtok(inputBuffer, token.c_str());
    while (word != NULL) {
        words.push_back(word);
        word = strtok(NULL, token.c_str());
    }  
    return words;
}
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
    char buff[1024];
    ClientData c;    
    ifstream in(seederFile);
    if(!in) {
        cout << "Cannot open seederFile file.\n";
        exit(1);
    }   
    char str[1024];
    while(in) {
        in.getline(str, 1024);
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


vector<string> readSocketBuff(int sock){
    cout<<"In read socket buffer"<<endl;
    string res;
    while(1){
        cout<<"here"<<endl;
        char buffer[1024*512]={0};
        int valread = read( sock , buffer, 1024*512);
        cout<<valread<<endl; 
        if (valread == 0)
        break;
        if (valread == -1) {
            perror("read");
            break;
        }
        string data=buffer;
        cout<<buffer<<endl;
        res=res+buffer;
        char result[res.size()+2];
        strcpy(result, res.c_str());  
        vector<string> v=tokenize(result,"|");    
        if(v[v.size()-1]=="end")
            return v;
    }
    // char result[res.size()+2];
    // strcpy(result, res.c_str());  
    // return tokenize(result,"|");
   
}

void getF(int sock,string hash){
    cout<<"In Get"<<endl;
    string result;
    for (int i = 0; i < mapHashToClient[hash].size(); ++i)
    {
        result=result+mapHashToClient[hash][i].clienAddr+"|";
    }
    result=result+"end";
    send(sock , result.c_str() , result.size() , 0 ); 
}

void shareF(std::vector<string> words){
    cout<<"In share"<<endl;
    FILE *fp;
    string seederFile="seedList.txt";
    ClientData c;
    c.hash=words[3];
    c.filePath=words[2];
    c.clienAddr=words[1];
    mapHashToClient[c.hash].push_back(c);
    fp = fopen(seederFile.c_str(), "ab"); 
    cout<<"here"<<endl;
    fprintf(fp,"%s|", words[3].c_str());
    fprintf(fp,"%s|", words[2].c_str());
    fprintf(fp,"%s|", words[1].c_str());
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

void *connection_handler(void *t)
{
    while(true){
    cout<<"Connection Established\n";
    struct ThreadParam *tp=(ThreadParam*)t;
    int sock=tp->sok;
    // string seederFile="seedList.txt";
    string seederFile=tp->seederFile;
    vector<string> words=readSocketBuff(sock);
    if(words.size()==0)
        break;
    cout<<words[0]<<endl;
    if(words[0]=="share"){
        shareF(words);
    }
    else if(words[0]=="get"){
        getF(sock,words[1]);
    }
    else if(words[0]=="exit"){
        close(sock);
        break;
    }
    cout<<"Done"<<endl;
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
    string seederFile=args[3];
    pthread_t thrd;
    while(true){
        cout<<"Waiting for client Request"<<endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&CAddress,(socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }         
        ThreadParam t;
        t.sok=new_socket;
        t.seederFile=args[3];

        pthread_attr_t thread_attr;
        int res = pthread_attr_init(&thread_attr);
        if (res != 0) {
            perror("Attribute creation failed");
            exit(EXIT_FAILURE);
        }
        res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        if (res != 0) {
            perror("Setting detached attribute failed");
            exit(EXIT_FAILURE);
        }

        if( pthread_create( &thrd , &thread_attr ,  connection_handler , &t) < 0)
        {
            perror("could not create thread");
            return 1;
        }
    }
    return 0; 
} 
