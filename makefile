all: myTorrent tracker client
CC = g++
CFLAGS = -Wall -std=c++0x
DEPS =classes.h utility.h
OBJ = main.o classes.o utility.o
DEPS2 = socketUtility.h 
OBJ2 = tracker.o socketUtility.o classes.o utility.o
OBJ3 = client.o classes.o utility.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

myTorrent: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto

%.o: %.cpp $(DEPS2)
	$(CC) $(CFLAGS) -c -o $@ $<

tracker: $(OBJ2)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto -lpthread

client: $(OBJ3)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto

.PHONY : clean
clean :
	-rm *.o $(objects) myTorrent
	-rm *.o $(objects) tracker
	-rm *.o $(objects) client


