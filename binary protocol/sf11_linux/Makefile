build_folder := $(shell mkdir -p bin)

output: ./bin/main.o ./bin/lwnx.o
	gcc ./bin/main.o ./bin/lwnx.o -o ./bin/sample -lrt

./bin/main.o: main.c lwnx.h
	gcc -O3 -I. -c main.c -o ./bin/main.o

./bin/lwnx.o: lwnx.c lwnx.h
	gcc -O3 -I. -c lwnx.c -o ./bin/lwnx.o

clean: rm ./bin/*.o

