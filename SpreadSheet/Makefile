CC=g++
CFLAGS=-c -ggdb -Wall -I.
LDFLAGS=

SOURCES= main.cpp SpreadSheet.cpp
OBJECTS=$(SOURCES:.cpp=.o)

SS_EVAL=kontagent_ss

all: $(SOURCES) $(SS_EVAL)
		
$(SS_EVAL): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm *.o $(SS_EVAL)

