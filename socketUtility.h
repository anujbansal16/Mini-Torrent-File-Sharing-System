/***********************************************************

OWNER: 	ANUJ BANSAL
ROLLNO. 2018201096
COPYRIGHT PROTECTED
***********************************************************/
#include<bits/stdc++.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
using namespace std;
// #define CHUNKSIZE 1024*512
typedef long long l;

#ifndef INC_SOCKETUTILITY_H
#define INC_SOCKETUTILITY_H

struct  ClientData
{
	string hash;
	string filePath;
	string clienAddr;
};

int getServerSocket(string ip,string port);
int getClientSocket(string ip,string port);
#endif 


