# CC = Compiler
CC=g++

# CFLAGS = CompilatorFLAGS
CFLAGS=-c

all: akin

akin: akinator.o bin_tree.o stack.o
	$(CC) akinator.o bin_tree.o stack.o -o akin

akinator.o: akinator.cpp
	$(CC) $(CFLAGS) akinator.cpp

bin_tree.o: BinaryTree/bin_tree.cpp
	$(CC) $(CFLAGS) BinaryTree/bin_tree.cpp

stack.o: Stack/stack.cpp	
	$(CC) $(CFLAGS) Stack/stack.cpp

clean:
	rm -rf *.o akin
