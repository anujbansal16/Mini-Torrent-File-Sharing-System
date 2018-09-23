/***********************************************************

OWNER:  ANUJ BANSAL
ROLLNO. 2018201096
COPYRIGHT PROTECTED
Peers Functionality (Peer as a Client) and (Peer as Server)
***********************************************************/
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

std::mutex mtx;//mutex for mutual exclusion of log file

string log_file;//log file

//function writing in log file
void write_to_log_file( const std::string &text )
{
    mtx.lock();
    ofstream logfile(log_file, std::ios_base::out | std::ios_base::app );
    time_t now;
    time(&now);
    char *date=ctime(&now);
    date[strlen(date)-1]='\0';
    logfile<<date<<" "<<"CLIENT/SEEDER "<<text<<endl;  
    mtx.unlock();
}

//Struct to pass data in multithreading routines 
struct ClientThreadParam
{
    int arg;
    char** args;
};

//Struct to strore all information related to a seeder
struct seederIPPortInfo
{
    string seederIP;
    string seederPort;
    string filePath;
    string destinationPath;//destination path where to download the file
    MTorrent m;//object for mtorrent file information 
    int noofSeeders;//no of seeders on file to download
    long mynoofchunks;//no of chunks i will provide
    long start;// start block of chunk i will download
};

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

//thread routine called on created peer thread
void *createPeerClient(void *t){
    // cout<<"Create Peer Client"<<endl;
    write_to_log_file("In createPeerClient");
    string fileName;
    seederIPPortInfo *st=(seederIPPortInfo *)t;
    string seederIP=st->seederIP;
    string seederPort=st->seederPort;
    string filePath=st->filePath;
    string destinationPath=st->destinationPath;
    long mynoofchunks=st->mynoofchunks;
    long start=st->start;
    MTorrent m=st->m;
    int noofSeeders=st->noofSeeders;
    // cout<<"seederIP "<<seederIP<<endl;
    write_to_log_file("createPeerClient: seederIP: "+ seederIP);
    // cout<<"seederPort "<<seederPort<<endl;
    write_to_log_file("createPeerClient: seederPort: "+ seederPort);
    // cout<<"filePath "<<filePath<<endl;
    write_to_log_file("createPeerClient: filePath: "+ filePath);
    // cout<<"destinationPath "<<destinationPath<<endl;
    write_to_log_file("createPeerClient: destinationPath "+ destinationPath);
    // cout<<"noofSeeders "<<noofSeeders<<endl;
    write_to_log_file("createPeerClient: noofSeeders "+ to_string(noofSeeders));
    // cout<<"mynoofchunks "<<mynoofchunks<<endl;
    write_to_log_file("createPeerClient: mynoofchunks "+ to_string(mynoofchunks));
    // cout<<"start "<<start<<endl;
    write_to_log_file("createPeerClient: start "+ to_string(start));

    string res=filePath+"|"+to_string(start)+"|"+to_string(mynoofchunks)+"|"+"end";
    size_t pos = filePath.rfind("/", filePath.length());
    if (pos != string::npos) {
        fileName=(filePath.substr(pos+1, filePath.length() - pos));
    }


    int peerClientSock=getClientSocket(seederIP,seederPort);
    if(peerClientSock==-1)
        pthread_exit(NULL);
    send(peerClientSock, res.c_str(), res.size() , 0);

    //reading sent data
    FILE *fp;
    fp = fopen((destinationPath+"/"+fileName).c_str(), "ab"); 
    fseek ( fp , start*CHUNK , SEEK_SET);
    char buffer[1024*512] = {0}; 
    while(1){
        // cout<<"Downloading.."<<seederPort<<endl;
        // write_to_log_file("Downloading.. from "+ seederIP+":"+seederPort);
        int valread = read( peerClientSock , buffer, 1024*512); 
        // cout<<valread<<endl;
        write_to_log_file("Downloded..Bytes.. from "+ seederIP+":"+seederPort+" "+ to_string(valread));
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
    cout<<"Download Completed from  "<<seederIP<<":"<<seederPort<<endl;
    write_to_log_file("Download Completed from "+ seederIP+":"+seederPort);
    close(peerClientSock);
    pthread_exit(NULL);
}

//function create peer client thread
void createPeerClientThread(seederIPPortInfo sd){
    // cout<<"Inside createPeerClientThread"<<endl;
    write_to_log_file("in createPeerClientThread");
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

}

//function assigns no. of chunks each peer contribute
void createAllPeerConnections(seederIPPortInfo seedersList[], int noofSeeders, MTorrent m){
    int fileSize = atoi(m.fileSize.c_str());
    int noofChunks=ceil(1.0*fileSize/(CHUNK));
    int chunksPerseeeder=(noofChunks/noofSeeders);
    int lastAdditionalChunks=noofChunks-chunksPerseeeder*noofSeeders;
    // cout<<" "<<noofChunks<<chunksPerseeeder<<" "<<lastAdditionalChunks<<endl;

    for (int i = 0; i < noofSeeders; ++i)
    {

        seedersList[i].mynoofchunks=chunksPerseeeder;
        seedersList[i].start=i*chunksPerseeeder;
        if(i==noofSeeders-1)
            seedersList[i].mynoofchunks+=lastAdditionalChunks;
        createPeerClientThread(seedersList[i]);
    }

}

//handler function to handle various requests - share,get,exit
void processCommand(int sock,string opcode, string tracker1, string tracker2, string filePath,string mFileName, string mFilePath, string cipAndPort, string destinationPath){
    write_to_log_file("in processCommand");
    if(opcode=="share"){
        write_to_log_file("processCommand: SHARE");
        char buffer[1024]={0};
        getcwd(buffer,1024);
        string pwd=buffer;

        string inFile=filePath;
        string outFile=pwd+"/"+mFileName;
        MTorrent m=createMtorrent(inFile,outFile, tracker1, tracker2);
        string res=opcode+"|"+cipAndPort+"|"+filePath+"|"+m.hashOfFile+"|"+"end";
        // cout<<"send client from share"<<endl;
        write_to_log_file("processCommand: Sending share data");
        send(sock , res.c_str() , res.size() , 0 );

    }
    if (opcode=="get")
    {
        write_to_log_file("processCommand: GET");
        MTorrent m=readMtorrent(mFilePath);
        string data,res;
        vector<string> v;
        int noofSeeders;
        string sndData=opcode+"|"+m.hashOfFile+"|"+"end";
        // cout<<"send client from get"<<endl;
        write_to_log_file("processCommand: Sending GET data");
        send(sock , sndData.c_str() , sndData.size()+10 , 0 ); 
        char buffer[1024] = {0}; 
        while(1){
            // cout<<"client"<<endl;
            int valread = read( sock , buffer, 1024);
            if (valread == 0)
            break;
            if (valread == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }
            data=buffer;
            res=res+buffer;
            char result[res.size()+2];
            strcpy(result, res.c_str());  
            v=tokenize(result,"|");    
            noofSeeders=(v.size()-1)/2;
            if(v[v.size()-1]=="end")
               break;

        }
        seederIPPortInfo seedersList[noofSeeders];
        for (unsigned int i = 0; i < v.size()-1; i+=2)
        {
            // cout<<v[i]<<endl;
            write_to_log_file("processCommand:GET: seederIPAndPort"+v[i]);
            string seederIPAndPort = v[i];
            int pos=seederIPAndPort.find(":");
            string seederIP = seederIPAndPort.substr(0, pos); 
            string seederPort = seederIPAndPort.substr(pos+1,seederIPAndPort.size()); 
            string filePath=v[i+1];
            // cout<<filePath<<endl;
            write_to_log_file("processCommand:GET: FilePath"+filePath);
            seederIPPortInfo sd;
            sd.seederIP=seederIP;
            sd.seederPort=seederPort;
            sd.filePath=filePath;
            sd.destinationPath=destinationPath;
            sd.m=m;
            sd.noofSeeders=noofSeeders;

            seedersList[i/2]=sd;
        }
        createAllPeerConnections(seedersList,noofSeeders,m);

    }
    if(opcode=="exit"){
        cout<<"Exiting client"<<endl;
        write_to_log_file("processCommand: EXIT: Exiting client");
        string exitData=opcode+"|"+"end";
        send(sock , exitData.c_str() , exitData.size() , 0 ); 
    }

}

//handles reading commands from user
int readCommands(int sock, string tracker1, string tracker2, string cipAndPort){
    write_to_log_file("in readCommands");
    string filePath, mFileName, destinationPath,opcode,mFilePath;
    // cout<<"Read Command"<<endl;
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
        write_to_log_file("readCommands: Invalid operation");
    }
    return 1;

}

//thread routine called on tracker connection thread creation
void *createTrackerSocket(void *t){
    // cout<<"Created Tracker Socket"<<endl;
    write_to_log_file("in createTrackerSocket");
    ClientThreadParam *ct=(ClientThreadParam *)t;
    int arg=ct->arg;
    char ** args=ct->args;
    int sock = 0; 
    struct sockaddr_in serv_addr; 

    if (arg < 5) {
         cout<<"No Tracker ip and port provided\n";
         exit(1);
    }   

    //client id port
    string cipAndPort=args[1];
    // cout<<cipAndPort<<endl;
    
    //getting tracker ip and port
    string idAndPort = args[2];
    int pos=idAndPort.find(":");
    string t1ip = idAndPort.substr(0, pos); 
    string t1port = idAndPort.substr(pos+1,idAndPort.size()); 

    int portno = atoi(t1port.c_str());
    // char buffer[CHUNK] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\nClient Socket creation error \n"); 
        exit(EXIT_FAILURE); 
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
        exit(EXIT_FAILURE); 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        exit(EXIT_FAILURE); 
    } 
    cout<<"Connected with tracker"<<endl;
    write_to_log_file("createTrackerSocket: connection Established with tracker");
    //breaking below loop on returning 0 when type exit
    while(readCommands(sock,args[2],args[3],cipAndPort));
    pthread_exit(NULL);
}

//thread routine called when this client become seeder and other client
//opening socket connection to this seeder
void * connection_handler(void *t){
    // cout<<"Uploading file"<<endl;
    write_to_log_file("in connection_handler: Uploading file");
    int sock=(int)t;
    char buffer[1024]={0};
    // cout<<"Connected peer "<<sock<<" "<<readbytes<<" "<<buffer <<endl;
    unsigned char inputB[1024*512]={0};
    string res;
    vector<string> v;
    while(1){
            int valread = read( sock , buffer, 1024);
            // cout<<valread<<endl; 
            if (valread == 0)
            break;
            if (valread == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }   
            string data=buffer;
            // cout<<buffer<<endl;
            write_to_log_file("in connection_handler: Seeder Data Stream "+data);
            res=res+buffer;
            char result[res.size()+2];
            strcpy(result, res.c_str());  
            v=tokenize(result,"|");    
            if(v[v.size()-1]=="end"){
                break;
            }
    }
    cout<<"Uploading.."<<endl;
    FILE *fp = fopen(v[0].c_str(),"rb");
    long start=atoi(v[1].c_str());
    fseek ( fp , start*CHUNK , SEEK_SET );
    long mynoofchunks=atoi(v[2].c_str());
    size_t size;
    while (mynoofchunks>0 && (size = fread( inputB,1, 1024*512,fp)) > 0) {
        // cout<<size<<" "<<mynoofchunks<<"\n";
        write_to_log_file("in connection_handler: Uploading file:Bytes and chunks "+to_string(size)+" "+to_string(mynoofchunks));
        send(sock , inputB , size, 0);     
        mynoofchunks--;
    } 
    cout<<"Uploaded file "<<v[0]<<endl;
    send(sock, '\0',1,0);
    pthread_exit(NULL);
}

//make this client server by opening a socket at its own ip and port
void *makeMyServer(void *t){
    // cout<<"Created makeMyServer"<<endl;
    write_to_log_file("in makeMyServer");
    ClientThreadParam *ct=(ClientThreadParam *)t;
    // int arg=ct->arg;
    char ** args=ct->args;
    string cipAndPort=args[1];
    // cout<<cipAndPort<<endl;
    //getting tracker ip and port
    string idAndPort = args[1];
    int pos=idAndPort.find(":");
    string clip = idAndPort.substr(0, pos); 
    string clport = idAndPort.substr(pos+1,idAndPort.size()); 
    // cout<<clip<<" "<<clport<<endl;

    int server_fd=getServerSocket(clip,clport);    
    struct sockaddr_in CAddress; 
    int new_socket;
    int addrlen = sizeof(CAddress); 
    while(true){
        // cout<<"Waiting for Other Peer"<<endl;
        write_to_log_file("Waiting for other peer to connect");
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
    write_to_log_file("in main");
     if (arg < 5) {
         cout<<"Missing arguments ie client clientIP,tracker1IP,tracker2IP,logfile\n";
         exit(1);
    }   

    log_file=args[4];

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
   
    if( pthread_create( &thrd2 , &thread_attr2 ,  makeMyServer , &t2) < 0)
    {
        perror("could not create thread");
        return 1;
    }
    while(1);
    return 0; 
} 
