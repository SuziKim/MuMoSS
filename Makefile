CC = g++ -ggdb -std=c++11 -pthread -Wl,--no-as-needed
LIBS = `pkg-config --libs --cflags opencv`

all: program

Utils.o: src/Utils.cpp src/Utils.hpp
	@$(CC) -o $@ -c src/Utils.cpp 
	
SiftExtractor.o: src/SiftExtractor.cpp src/SiftExtractor.hpp
	@$(CC) -o $@ -c src/SiftExtractor.cpp
	
SimpleLateFusion.o: src/SimpleLateFusion.cpp src/SimpleLateFusion.hpp
	@$(CC) -o $@ -c src/SimpleLateFusion.cpp
	
MD5.o: src/MD5.cpp src/MD5.hpp
	@$(CC) -o $@ -c src/MD5.cpp	
	
program: Utils.o SiftExtractor.o SimpleLateFusion.o MD5.o
	@$(CC) src/main.cpp SiftExtractor.o Utils.o SimpleLateFusion.o MD5.o -o program $(LIBS)

clean:
	@rm -f program src/*.o *.o
