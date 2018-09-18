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
#include "classes.h"
using namespace std;
#define CHUNKSIZE 1024*512
typedef long long l;

#ifndef INC_UTILITY_H
#define INC_UTILITY_H
MTorrent createMtorrent(string inFile, string outFile, string track1, string track2);
string getSHA(int inF, int outF);
string getSHA(string hash);
MTorrent readMtorrent(string path);
#endif 


