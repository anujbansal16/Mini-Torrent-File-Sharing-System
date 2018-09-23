
// Client side C/C++ program to demonstrate Socket programming 
#include <bits/stdc++.h> 
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <fcntl.h>
#include <arpa/inet.h>
#include"utility.h"
#include"classes.h"
#include "socketUtility.h"
#define CHUNK 1024*512
using namespace std;
struct ClientThreadParam
{
    int arg;
    char** args;
};

struct seederIPPortInfo
{
    string seederIP;
    string seederPort;
    string filePath;
    string destinationPath;
    MTorrent m;
    int noofSeeders;
};

vector<string> tokenize(char inputBuffer[], string token){
    vector<string> words;
    char* word = strtok(inputBuffer, token.c_str());
    while (word != NULL) {
        words.push_back(word);
        word = strtok(NULL, token.c_str());
    }  
    return words;
}

void *createPeerClient(void *t){
    cout<<"Create Peer Client"<<endl;
    string fileName;
    seederIPPortInfo *st=(seederIPPortInfo *)t;
    string seederIP=st->seederIP;
    string seederPort=st->seederPort;
    string filePath=st->filePath;
    string destinationPath=st->destinationPath;
    MTorrent m=st->m;
    int noofSeeders=st->noofSeeders;
    cout<<"seederIP "<<seederIP<<endl;
    cout<<"seederPort "<<seederPort<<endl;
    cout<<"filePath "<<filePath<<endl;
    cout<<"destinationPath "<<destinationPath<<endl;
    cout<<"noofSeeders "<<noofSeeders<<endl;
    string res=filePath+"|"+"end";

    size_t pos = filePath.rfind("/", filePath.length());
    if (pos != string::npos) {
        fileName=(filePath.substr(pos+1, filePath.length() - pos));
    }


    int peerClientSock=getClientSocket(seederIP,seederPort);
    if(peerClientSock==-1)
        pthread_exit(NULL);
    send(peerClientSock, res.c_str(), res.size() , 0);
    
    //reading sent data
    // FILE *fp;
    // fp = fopen((destinationPath+"/"+fileName).c_str(), "wb"); 
    char buffer[1024*512] = {0}; 
    while(1){
        cout<<"Downloading.."<<seederPort<<endl;
        int valread = read( peerClientSock , buffer, 1024*512); 
        cout<<valread<<endl;
        if (valread == 0)
        break;
        if (valread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        /*if (fwrite(buffer,1, valread,fp) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }*/
    }
    

    close(peerClientSock);

}


void createPeerClientThread(seederIPPortInfo sd){
    cout<<"Inside createPeerClientThread"<<endl;
    pthread_t thrd;
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

    if( pthread_create( &thrd , &thread_attr ,  createPeerClient , &sd) < 0)
    {
        perror("could not create thread");
    }
    usleep(50000);
    // thread t (createPeerClient, &sd);

}

void createAllPeerConnections(seederIPPortInfo seedersList[], int noofSeeders, MTorrent m){
    int fileSize = atoi(m.fileSize.c_str());
    int noofChunks=fileSize/(CHUNK);
    for (int i = 0; i < noofSeeders; ++i)
    {
        createPeerClientThread(seedersList[i]);
    }

}

void processCommand(int sock,string opcode, string tracker1, string tracker2, string filePath,string mFileName, string mFilePath, string cipAndPort, string destinationPath){
    if(opcode=="share"){
        char buffer[1024]={0};
        getcwd(buffer,1024);
        string pwd=buffer;

        string inFile=filePath;
        string outFile=pwd+"/"+mFileName;
        MTorrent m=createMtorrent(inFile,outFile, tracker1, tracker2);
        string res=opcode+"|"+cipAndPort+"|"+filePath+"|"+m.hashOfFile+"|"+"end";
        cout<<"send client from share"<<endl;
        send(sock , res.c_str() , res.size() , 0 );

    }
    if (opcode=="get")
    {
        MTorrent m=readMtorrent(mFilePath);
        string data,res;
        vector<string> v;
        int noofSeeders;
        string sndData=opcode+"|"+m.hashOfFile+"|"+"end";
        cout<<"send client from get"<<endl;
        send(sock , sndData.c_str() , sndData.size()+10 , 0 ); 
        char buffer[1024] = {0}; 
        while(1){
            cout<<"client"<<endl;
            int valread = read( sock , buffer, 1024);
            // cout<<valread<<endl; 
            if (valread == 0)
            break;
            if (valread == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            data=buffer;
            // cout<<data<<endl;
            res=res+buffer;
            char result[res.size()+2];
            strcpy(result, res.c_str());  
            v=tokenize(result,"|");    
            noofSeeders=(v.size()-1)/2;
            if(v[v.size()-1]=="end")
               break;

        }
        seederIPPortInfo seedersList[noofSeeders];
        for (int i = 0; i < v.size()-1; i+=2)
        {
            cout<<v[i]<<endl;
            string seederIPAndPort = v[i];
            int pos=seederIPAndPort.find(":");
            string seederIP = seederIPAndPort.substr(0, pos); 
            string seederPort = seederIPAndPort.substr(pos+1,seederIPAndPort.size()); 
            string filePath=v[i+1];
            cout<<filePath<<endl;
            seederIPPortInfo sd;
            sd.seederIP=seederIP;
            sd.seederPort=seederPort;
            sd.filePath=filePath;
            sd.destinationPath=destinationPath;
            sd.m=m;
            sd.noofSeeders=noofSeeders;

            seedersList[i/2]=sd;
            // createPeerClientThread(sd);
        }
        createAllPeerConnections(seedersList,noofSeeders,m);
        //create sockets each in a new thread
        

    }
    if(opcode=="exit"){
        cout<<"Exiting client"<<endl;
        string exitData=opcode+"|"+"end";
        send(sock , exitData.c_str() , exitData.size() , 0 ); 
    }

}

int readCommands(int sock, string tracker1, string tracker2, string cipAndPort){
    string filePath, mFileName, destinationPath,opcode,mFilePath;
    cout<<"Read Command"<<endl;
    cin>>opcode;
    if(opcode=="share"){
        cin>>filePath;
        cin>>mFileName;
        processCommand(sock,opcode,tracker1,tracker2,filePath,mFileName,mFilePath,cipAndPort,destinationPath);
    }
    else if(opcode=="get"){
        cin>>mFilePath;
        cin>>destinationPath;
        processCommand(sock,opcode,tracker1,tracker2,filePath,mFileName,mFilePath,cipAndPort,destinationPath);
    }
    else if(opcode=="exit"){
        processCommand(sock,opcode,tracker1,tracker2,filePath,mFileName,mFilePath,cipAndPort,destinationPath);
        close(sock);
        return 0;
    }
    else{
        cout<<"Invalid operation\n";
    }
    return 1;

}

void *createTrackerSocket(void *t){
    cout<<"Created Tracker Socket"<<endl;
    ClientThreadParam *ct=(ClientThreadParam *)t;
    int arg=ct->arg;
    char ** args=ct->args;
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 

    if (arg < 5) {
         cout<<"No Tracker ip and port provided\n";
         exit(1);
    }   

    //client id port
    string cipAndPort=args[1];
    cout<<cipAndPort<<endl;
    
    //getting tracker ip and port
    string idAndPort = args[2];
    int pos=idAndPort.find(":");
    string t1ip = idAndPort.substr(0, pos); 
    string t1port = idAndPort.substr(pos+1,idAndPort.size()); 

    int portno = atoi(t1port.c_str());
    char buffer[CHUNK] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\nClient Socket creation error \n"); 
        // return -1; 
        // thread_exit(NULL);
    } 
    int opt=1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(portno); 
           
    if(inet_pton(AF_INET, t1ip.c_str(), &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        // return -1; 
        exit(EXIT_FAILURE); 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        // return -1; 
        exit(EXIT_FAILURE); 
    } 
    printf("\nConnection Established \n"); 
    //breaking below loop on returning 0 when type exit
    while(readCommands(sock,args[2],args[3],cipAndPort));
}

void * connection_handler(void *t){
    cout<<"Uploading file"<<endl;
    int sock=(int)t;
    char buffer[1024]={0};
    // cout<<"Connected peer "<<sock<<" "<<readbytes<<" "<<buffer <<endl;
    unsigned char inputB[1024*512]={0};
    string res;
    vector<string> v;
    while(1){
            int valread = read( sock , buffer, 1024);
            cout<<valread<<endl; 
            if (valread == 0)
            break;
            if (valread == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }   
            string data=buffer;
            cout<<buffer<<endl;
            res=res+buffer;
            char result[res.size()+2];
            strcpy(result, res.c_str());  
            v=tokenize(result,"|");    
            if(v[v.size()-1]=="end"){
                break;
            }
    }
    FILE *fp = fopen(v[0].c_str(),"rb");
    size_t size;
    while ((size = fread( inputB,1, 1024*512,fp)) > 0) {
        cout<<size<<"\n";
        write(sock , inputB , size );     
    }       
    // close(new_socket);
}

void *makeMyServer(void *t){
    cout<<"Created makeMyServer"<<endl;
    ClientThreadParam *ct=(ClientThreadParam *)t;
    int arg=ct->arg;
    char ** args=ct->args;
    string cipAndPort=args[1];
    cout<<cipAndPort<<endl;
    //getting tracker ip and port
    string idAndPort = args[1];
    int pos=idAndPort.find(":");
    string clip = idAndPort.substr(0, pos); 
    string clport = idAndPort.substr(pos+1,idAndPort.size()); 
    cout<<clip<<" "<<clport<<endl;

    int server_fd=getServerSocket(clip,clport);    
    struct sockaddr_in CAddress; 
    int new_socket;
    int addrlen = sizeof(CAddress); 
    while(true){
        cout<<"Waiting for Other Peer"<<endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&CAddress,(socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        //create thread for serving clients
        pthread_t thrd;
        pthread_attr_t thread_attr;
        int res = pthread_attr_init(&thread_attr);
        if (res != 0) {
            perror("Attribute creation failed");
            // exit(EXIT_FAILURE);
            continue;
        }
        res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        if (res != 0) {
            perror("Setting detached attribute failed");
            // exit(EXIT_FAILURE);
            continue;
        }
        if( pthread_create( &thrd , &thread_attr ,  connection_handler , (void *)new_socket) < 0)
        {
            perror("could not create thread");
            continue;
        }
    }
}

int main(int arg, char  *args[]) 
{ 
    pthread_t thrd;
    ClientThreadParam t;
    t.arg=arg;
    t.args=args;


    pthread_attr_t thread_attr;
    int res = pthread_attr_init(&thread_attr);
    if (res != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }
   
    if( pthread_create( &thrd , &thread_attr ,  createTrackerSocket , &t) < 0)
    {
        perror("could not create thread");
        return 1;
    }


    pthread_t thrd2;
    ClientThreadParam t2;
    t2.arg=arg;
    t2.args=args;


    pthread_attr_t thread_attr2;
    int res2 = pthread_attr_init(&thread_attr2);
    if (res2 != 0) {
        perror("Attribute creation failed");
        exit(EXIT_FAILURE);
    }
   
    if( pthread_create( &thrd , &thread_attr2 ,  makeMyServer , &t) < 0)
    {
        perror("could not create thread");
        return 1;
    }

    cout<<"gone"<<endl;
    while(1);
    return 0; 
} 
