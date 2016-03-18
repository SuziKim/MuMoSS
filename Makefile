CC = g++ -O2 -ggdb -std=c++11 -pthread -Wl,--no-as-needed -larmadillo -fopenmp
LIBS = `pkg-config --libs --cflags opencv`

all: MuMoSS

Utils.o: src/Utils.cpp src/Utils.hpp
	@$(CC) -o $@ -c src/Utils.cpp 
	
SiftExtractor.o: src/SiftExtractor.cpp src/SiftExtractor.hpp
	@$(CC) -o $@ -c src/SiftExtractor.cpp
	
SimpleLateFusion.o: src/SimpleLateFusion.cpp src/SimpleLateFusion.hpp
	@$(CC) -o $@ -c src/SimpleLateFusion.cpp
	
MD5.o: src/MD5.cpp src/MD5.hpp
	@$(CC) -o $@ -c src/MD5.cpp	
	
MuMoSS: Utils.o SiftExtractor.o SimpleLateFusion.o MD5.o
	@$(CC) src/main.cpp SiftExtractor.o Utils.o SimpleLateFusion.o MD5.o -o $@ $(LIBS)

clean:
	@rm -f MuMoSS src/*.o *.o
