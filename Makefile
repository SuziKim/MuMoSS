CPPFLAGS := -O2 -std=c++11 -pthread -Wl,--no-as-needed -larmadillo -fopenmp -lssl -lcrypto
LDLIBS := `pkg-config --libs --cflags opencv`
OBJS = Utils.o InputOutput.o SiftExtractor.o SimpleLateFusion.o SimpleEarlyFusion.o
PROG = MuMoSS
CXX = g++

all: $(PROG)

$(PROG): $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) src/main.cpp -o $@ $(LDLIBS)

%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

clean:
	@rm -f $(PROG) *.o