# Mini-Torrent-File-Sharing-System
Mini torrent

Notes:
  * Type **exit** to disconnect a client.
  <!--* Do not press **Ctrl+C** else server get disconnected , instead type **exit**-->
  * Do not close the client window directly.
  * Provide absolute paths wherever the path is required.(in share functionality only give mtorrent file name which gets created in present working directory)
  * Assuming seeders always have complete file
  * Algorithm divides total number of chunks amongs the number of seeders(ie if 2 seeders are there then each contribute half number of chunks) 
  <!--* **No spaces anywhere in filename/directoryname**-->
<!--Working Files:
  * classes.h utility.h socketUtility.h 
  * main.cpp classes.cpp utility.cpp
  * tracker.cpp client.cpp socketUtility.cpp-->

Examples:
  * ./client 127.0.0.1:8082 127.0.0.1:8081 127.0.0.1:8081 mylogs.txt
  * ./tracker 127.0.0.1:8081 127.0.0.1:8081 seedList.txt mylogs.txt	
  * share /home/anuj/Desktop/Git-Stuff/MiniTorrent/dummy.pdf dummy6.mtorrent
  * share /home/anuj/Desktop/Semester1/2018201096-osAss2/phli.mp3 phli.mtorrent
  * get /home/anuj/Desktop/Git-Stuff/MiniTorrent/phli.mtorrent /home/anuj/Desktop
  * get /home/anuj/Desktop/Git-Stuff/MiniTorrent/dummy6.mtorrent /home/anuj/Desktop
