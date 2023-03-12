BIN=bin
CPPFLAGS=g++ -O3 -I.
LDFLAGS=g++
LDLIBS=-lrt
build_folder := $(shell mkdir -p $(BIN))

output:	$(BIN)/main.o $(BIN)/lwSerialPortLinux.o $(BIN)/platformLinux.o
	$(LDFLAGS) $(BIN)/main.o $(BIN)/lwSerialPortLinux.o $(BIN)/platformLinux.o -o $(BIN)/sample $(LDLIBS)

$(BIN)/main.o: ./src/main.cpp
	$(CPPFLAGS) -c ./src/main.cpp -o $(BIN)/main.o

$(BIN)/lwSerialPortLinux.o: ./src/linux/lwSerialPortLinux.cpp
	$(CPPFLAGS) -c ./src/linux/lwSerialPortLinux.cpp -o $(BIN)/lwSerialPortLinux.o

$(BIN)/platformLinux.o: ./src/linux/platformLinux.cpp
	$(CPPFLAGS) -c ./src/linux/platformLinux.cpp -o $(BIN)/platformLinux.o

clean:
	-rm -r $(BIN)