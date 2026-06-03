CXX = g++
CXXFLAGS = -Wall -O3 -I C:\eigen3\eigen-3.4.0 -I/usr/include/eigen3/ -I/usr/local/include/eigen3

hide: hide.cpp Entities.cpp
	$(CXX) $(CXXFLAGS) hide.cpp Entities.cpp -o hide

tetra:
	./hide tetra-3.nff tetra-3.ppm

gears:
	./hide gears.nff gears.ppm

balls:
	./hide balls.nff balls.ppm

clean:
	rm *~
	rm *.o
