all: cache-sim

cache-sim: cache-sim.o 
	g++ cache-sim.o -o cache-sim

cache-sim.o: cache-sim.cpp
	g++ -c -g cache-sim.cpp

clean:
	rm -f *.o cache-sim