CC=g++
CFLAGS=-c -ggdb -Wall -I.
LDFLAGS=

SOURCES= TrieChild.cpp TrieChildren.cpp Trie.cpp main.cpp 

OBJECTS=$(SOURCES:.cpp=.o)

WORD_FINDER=wordfinder

all: $(SOURCES) $(WORD_FINDER) 
		
$(WORD_FINDER): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(WORD_FINDER)

