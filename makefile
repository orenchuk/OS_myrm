all: make 

make: main.cpp
	g++ -std=c++14 main.cpp -o myrm -lboost_system -lboost_filesystem -lboost_system

.PHONY: clean 

clean:
	rm myrm

