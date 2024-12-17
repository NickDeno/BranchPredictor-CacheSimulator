all: predictors

predictors: predictors.o 
	g++ predictors.o -o predictors

predictors.o: predictors.cpp
	g++ -c -g predictors.cpp

clean:
	rm -f *.o predictors