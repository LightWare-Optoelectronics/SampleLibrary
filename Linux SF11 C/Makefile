build_folder := $(shell mkdir -p bin)

output: ./bin/main.o
	g++ ./bin/main.o -o ./bin/sf11_c_sample -lrt

./bin/main.o: main.cpp
	g++ -O3 -c main.cpp -o ./bin/main.o

clean: rm ./bin/*.o

