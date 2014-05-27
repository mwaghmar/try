CC=g++
CFLAGS=-c -ggdb -Wall -I.
LDFLAGS=

SOURCES= ReceivedHeader.cpp MailHeaderParser.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)

HEADER_READER=readheaders

all: $(SOURCES) $(HEADER_READER) 
		
$(HEADER_READER): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm *.o $(HEADER_READER)

