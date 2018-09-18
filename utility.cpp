/***********************************************************

OWNER:  ANUJ BANSAL
ROLLNO. 2018201096
COPYRIGHT PROTECTED
***********************************************************/
#include"utility.h"
#include "classes.h"
#define CHUNKSIZE 1024*512
typedef long long l;

string getSHA(int inF, int outF){
  string result;
  char inputB[CHUNKSIZE];
    size_t size;
    char temp[2];
    unsigned char oSHABuf[20];
    while ((size = read(inF, inputB, CHUNKSIZE)) > 0) {
      SHA1((unsigned char *)inputB, size, oSHABuf);
      for (int i = 0; i < 20; i++) {
         sprintf(temp,"%02x", oSHABuf[i]);
         // cout<<temp<<" ";
         result+=temp;
         write(outF, temp, 2);
      } 
    }
    return result;

}

MTorrent createMtorrent(string inFile, string outFile){
  MTorrent m;
  struct stat info;   
  if(stat(inFile.c_str(), &info)==-1){
      perror(strerror(errno));
  }
  long fsize=info.st_size;
  string tracker1="10.42.0.394:4444\n";
  string tracker2="10.42.0.666:5555\n";
  string fileName=inFile+"\n";
  string fileSize=to_string(fsize)+"\n";
  int inF =open(inFile.c_str(),O_RDONLY, 0);  
  int outF = open(outFile.c_str(), O_WRONLY | O_CREAT , 0664);  
  write(outF, tracker1.c_str(), tracker1.size());
  write(outF, tracker2.c_str(), tracker2.size());
  write(outF, fileName.c_str(), fileName.size());
  write(outF, fileSize.c_str(), fileSize.size());
  string hash=getSHA(inF,outF);
  m.tracker1IP=tracker1;
  m.tracker2IP=tracker2;
  m.fileName=fileName;
  m.fileSize=fileSize;
  m.hashStr=hash;

  close(inF);
  close(outF);
  return m;
}








