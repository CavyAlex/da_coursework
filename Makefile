prog: Main.o Preprocessing.o Searching.o Struct.o
	g++ -std=c++11 -O2 -o prog Main.o Preprocessing.o Searching.o Struct.o
Main.o: Main.cpp Preprocessing.h Searching.h Struct.h
	g++ -c -std=c++11 -O2 Source1.cpp 
Preprocessing.o: Preprocessing.cpp Preprocessing.h Struct.h
	g++ -c -std=c++11 -O2 Preprocessing.cpp
Searching.o: Searching.cpp Searching.h Struct.h
	g++  -c -std=c++11 -O2 Searching.cpp 
Struct.o: Struct.cpp Struct.h
	g++ -c -std=c++11 -O2 Struct.cpp
clean:
	rm -f *.o prog