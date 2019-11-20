build_folder := $(shell mkdir -p bin)

output: ./bin/main.o
	g++ ./bin/main.o -o ./bin/sample -lrt

./bin/main.o: main.c
	g++ -O3 -I. -c main.c -o ./bin/main.o

clean: rm ./bin/*.o

