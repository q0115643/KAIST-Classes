CC=g++ -std=c++11

default: runfile

runfile.o: main.cpp
	$(CC) -c main.cpp -o runfile.o

runfile: runfile.o
	$(CC) runfile.o  -o runfile

clean:
	rm -f *.o
	rm -f runfile
