all: a

a: DLCpp.o test.o
	g++ -Wall DLCpp.o test.o -l sqlite3 -o a -g

DLCpp.o: DLCpp.h DLCpp.cpp
	g++ -Wall DLCpp.cpp DLCpp.h -c -g

teste.o: test.cpp
	g++ -Wall test.cpp -c -g

clean:
	rm test.o
	rm DLCpp.o