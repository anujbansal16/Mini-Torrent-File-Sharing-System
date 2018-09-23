/***********************************************************

OWNER:  ANUJ BANSAL
ROLLNO. 2018201096
COPYRIGHT PROTECTED
Tracker Functionality
***********************************************************/
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
#include <fstream>
#include <mutex>
using namespace std;
std::mutex mtx;      
string log_file;

//function writing in log file
void write_to_log_file( const std::string &text )
{
    mtx.lock();
    ofstream logfile(log_file, std::ios_base::out | std::ios_base::app );
    time_t now;
    time(&now);
    char *date=ctime(&now);
    date[strlen(date)-1]='\0';
    logfile<<date<<" "<<"SERVER/TRACKER "<<text<<endl;  
    mtx.unlock();
}

//Struct to pass data in multithreading routines 
struct ThreadParam
{
    int sok;
    string seederFile;
};

//map of hashString vs list of Associated Clients
static map<string,vector<ClientData>> mapHashToClient;

//function that tokenize steam of data using token as delimiter
vector<string> tokenize(char inputBuffer[], string token){
    vector<string> words;
    char* word = strtok(inputBuffer, token.c_str());
    while (word != NULL) {
        words.push_back(word);
        word = strtok(NULL, token.c_str());
    }  
    return words;
}

//construct struct of client data from array of data seperated by |
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

//Function reading seeder file, throw error if not exist
void readSeederfile(string seederFile){
    // cout<<"in read readSeederfile\n";
    write_to_log_file("In readSeederfile");
    // FILE *fp;  
    // char buff[1024];
    ClientData c;    
    ifstream in(seederFile);
    if(!in) {
        cout << "Cannot open seederFile file. SeederFile not exist.\n";
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
        // cout<<i->first<<endl;
        write_to_log_file("readSeederfile: HashOfFile "+i->first);
        for (vector<ClientData>::iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
                // cout<<(*j).clienAddr<<endl;
                write_to_log_file("readSeederfile: Client Address "+(*j).clienAddr);
        }
    }
}

//Function reading the input of socket(| seperated) and convert it into vector
vector<string> readSocketBuff(int sock){
    // cout<<"In read socket buffer"<<endl;
    write_to_log_file("In readSocketBuff");
    string res;
    vector<string> v;
    while(1){
        char buffer[1024*512]={0};
        int valread = read( sock , buffer, 1024*512);
        // cout<<valread<<endl; 
        write_to_log_file("readSocketBuff: "+to_string(valread));
        if (valread == 0)
        break;
        if (valread == -1) {
            perror("read");
            break;
        }
        string data=buffer;
        // cout<<buffer<<endl;
        write_to_log_file("readSocketBuff: buffer");
        res=res+buffer;
        char result[res.size()+2];
        strcpy(result, res.c_str());  
        v=tokenize(result,"|");    
        if(v[v.size()-1]=="end")
            return v;
    }
    return v;
}


//function handling the GET request of client
void getF(int sock,string hash){
    cout<<"Get File"<<endl;
    write_to_log_file("In getF");
    string result;
    for (unsigned int i = 0; i < mapHashToClient[hash].size(); ++i)
    {
        result=result+mapHashToClient[hash][i].clienAddr+"|";
        result=result+mapHashToClient[hash][i].filePath+"|";
    }
    result=result+"end";
    send(sock , result.c_str() , result.size() , 0 ); 
}

//function handling the SHARE request of client
void shareF(std::vector<string> words, string seederFile){
    cout<<"Sharing File"<<endl;
    write_to_log_file("In shareF");
    FILE *fp;
    ClientData c;
    c.hash=words[3];
    c.filePath=words[2];
    c.clienAddr=words[1];
    mapHashToClient[c.hash].push_back(c);
    fp = fopen(seederFile.c_str(), "ab"); 
    fprintf(fp,"%s|", words[3].c_str());
    fprintf(fp,"%s|", words[2].c_str());
    fprintf(fp,"%s|", words[1].c_str());
    fprintf(fp,"\n");//endl
    fclose(fp);
    for (auto i= mapHashToClient.begin(); i != mapHashToClient.end(); ++i)
    {   
        // cout<<i->first<<endl;
        write_to_log_file("shareF: HashOfFile "+i->first );
        for (vector<ClientData>::iterator j = i->second.begin(); j != i->second.end(); ++j)
        {
                // cout<<(*j).clienAddr<<endl;
                write_to_log_file("shareF: Client Address "+ (*j).clienAddr);
        }
    }
}

//thread routine called when new socket connection is made
void *connection_handler(void *t)
{
    cout<<"Connected"<<endl;
    write_to_log_file("In connection_handler");
    while(true){
    // cout<<"Connection Established\n";
    write_to_log_file("Connection Established");
    struct ThreadParam *tp=(ThreadParam*)t;
    int sock=tp->sok;
    string seederFile=tp->seederFile;
    vector<string> words=readSocketBuff(sock);
    if(words.size()==0)
        break;
    // cout<<words[0]<<endl;
    write_to_log_file("connection_handler: opcode "+ words[0]);
    if(words[0]=="share"){
        shareF(words,seederFile);
    }
    else if(words[0]=="get"){
        getF(sock,words[1]);
    }
    else if(words[0]=="exit"){
        cout<<"Exiting client"<<endl;
        write_to_log_file("connection_handler: Exiting client");
        close(sock);
        break;
    }
}
pthread_exit(NULL);
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
    log_file=args[4];
    // cout<<"main"<<endl;
    write_to_log_file("In main");
    //getting ip and port
    string ip,port;
    string idAndPort = args[1];
    int pos=idAndPort.find(":");
    ip = idAndPort.substr(0, pos); 
    port = idAndPort.substr(pos+1,idAndPort.size()); 
    int server_fd=getServerSocket(ip,port);
    readSeederfile(args[3]);
    string seederFile=args[3];
    pthread_t thrd;
    cout<<"Tracker Started "<<endl;
    while(true){
        write_to_log_file("Main: Tracker Started ");
        write_to_log_file("Main: Waiting for client Request ");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&CAddress,(socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }         
        ThreadParam t;
        t.sok=new_socket;
        t.seederFile=seederFile;
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
