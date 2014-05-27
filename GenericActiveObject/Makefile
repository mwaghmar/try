CC=g++
INCLUDES=-I.
CFLAGS=-c -DACE_HAS_THREADS -std=c++0x -ggdb -Wall $(INCLUDES)
LDFLAGS=
LIBS=-L/usr/lib64 -lACE

SOURCES= main.cpp Peer.cpp Scheduler.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXE=main
OTHER_SRC=other_main.cpp Peer.cpp
OTHER_OBJECTS=$(OTHER_SRC:.cpp=.o)
OTHER_EXE=other_test

all: $(SOURCES) $(EXE) $(OTHER_SRC) $(OTHER_EXE)
		
$(EXE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS) -lpthread

$(OTHER_EXE): $(OTHER_OBJECTS) 
		$(CC) $(LDFLAGS) $(OTHER_OBJECTS) -o $@ -lpthread $(LIBS)

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm *.o $(EXE) $(OTHER_EXE)

