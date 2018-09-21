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

string getSHA(string hash){
  string result;
  char inputB[CHUNKSIZE];
    unsigned char oSHABuf[20];
    char temp[2];
    SHA1((unsigned char *)hash.c_str(), hash.size(), oSHABuf);
      for (int i = 0; i < 20; i++) {
          // printf("%02x", oSHABuf[i]);
         sprintf(temp,"%02x", oSHABuf[i]);
         result+=temp;
    }
    return result;

}


MTorrent createMtorrent(string inFile, string outFile, string track1, string track2){
  MTorrent m;
  struct stat info;   
  if(stat(inFile.c_str(), &info)==-1){
      perror(strerror(errno));
  }
  long fsize=info.st_size;
  string tracker1=track1+"\n";
  string tracker2=track2+"\n";
  string fileName=inFile+"\n";
  string fileSize=to_string(fsize)+"\n";
  int inF =open(inFile.c_str(),O_RDONLY, 0);  
  if(inF<0){
    cout<<"FAILURE: FILE_NOT_FOUND"<<endl;
    exit(0);
  }
  int outF = open(outFile.c_str(), O_WRONLY | O_CREAT , 0664);  
  write(outF, tracker1.c_str(), tracker1.size());
  write(outF, tracker2.c_str(), tracker2.size());
  write(outF, fileName.c_str(), fileName.size());
  write(outF, fileSize.c_str(), fileSize.size());
  string hash=getSHA(inF,outF);
  close(outF);

  string hash2=getSHA(hash);
  m.tracker1IP=tracker1;
  m.tracker2IP=tracker2;
  m.fileName=fileName;
  m.fileSize=fileSize;
  m.hashStr=hash;
  m.hashOfFile=hash2;

  close(inF);
  return m;
}

MTorrent readMtorrent(string path){
  MTorrent m;
  std::vector<string> v;
  ifstream in(path);
    if(!in) {
        cout << "FAILURE: MTORRENT_FILE_NOT_FOUND "<<endl;
        exit(1);
    }   
    char str[1024*512];
    int count=0;
    while(in) {
        in.getline(str, 1024*512);
        if(in) {
            // cout<<str<<endl;
            v.push_back(str);
        }
    }
    
      m.tracker1IP=v[0];
      m.tracker2IP=v[1];
      m.fileName=v[2];
      m.fileSize=v[3];
      m.hashStr=v[4];
      // cout<<m.hashStr<<endl;
      m.hashOfFile=getSHA(m.hashStr);
      // cout<<m.hashOfFile<<endl;
      in.close();
    
    return m;
}








