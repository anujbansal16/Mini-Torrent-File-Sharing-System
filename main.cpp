/***********************************************************

OWNER: 	ANUJ BANSAL
ROLLNO. 2018201096
COPYRIGHT PROTECTED
***********************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/syscall.h>
#include<iostream>
#include <sys/ioctl.h>
#include"utility.h"
#include"classes.h"

using namespace std;

int main(){	
	MTorrent m;
	string inFile="/home/anuj/Desktop/Semester1/2018201096-osAss2/dummy.pdf";
	string outFile="/home/anuj/Desktop/Semester1/2018201096-osAss2/dummy.mtorrent";
	m=createMtorrent(inFile,outFile);
 	
	return 0;
}

/*
AUTHOR:         ANUJ
DESCRIPTION:    Reset the indexing related parameters and move the cursor to first line
*/
