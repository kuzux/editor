OBJS:=build/main.o build/terminal.o build/input.o build/output.o build/abuf.o build/rows.o
BIN:=build/fled
CFLAGS:=-g -Wall -Werror -Wextra -pedantic -std=c99 -O2 -Iinclude/
PREFIX:=/usr/local/bin

all: $(BIN)

.PHONY: all clean install

$(BIN): $(OBJS)
	mkdir -p build
	$(CC) $(OBJS) -o $(BIN) 

build/%.o: src/%.c 
	mkdir -p build
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(OBJS) $(BIN)

install: $(BIN)
	cp $(BIN) $(PREFIX)
