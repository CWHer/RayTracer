LINK.o = $(LINK.cc)
CXXFLAGS = -O2 -std=c++14 -Wall -fopenmp

all: bouncing_sphere simple_light earth_sphere sky night cornell_box cornell_smoke final

bouncing_sphere: bouncing_sphere.o

simple_light: simple_light.o

earth_sphere: earth_sphere.o

sky: sky.o

night: night.o

cornell_box: cornell_box.o

cornell_smoke: cornell_smoke.o

final: final.o

clean:
	-rm -f bouncing_sphere simple_light earth_sphere sky night
	-rm -f cornell_box cornell_smoke final *.o
