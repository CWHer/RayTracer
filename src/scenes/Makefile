LINK.o = $(LINK.cc)
CXXFLAGS = -O2 -std=c++14 -Wall -fopenmp

all: first_ppm zoom_in final

first_ppm: first_ppm.o

zoom_in: zoom_in.o

final: final.o

clean:
	-rm -f first_ppm zoom_in final
	-rm -f *.o