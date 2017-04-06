OBJS:=build/main.o build/terminal.o build/input.o build/output.o build/abuf.o
BIN:=build/editor
CFLAGS:=-Wall -Werror -Wextra -pedantic -std=c99 -O2 -Iinclude/

all: $(BIN)

.PHONY: all clean

$(BIN): $(OBJS)
	mkdir -p build
	$(CC) $(OBJS) -o $(BIN) 

build/%.o: src/%.c 
	mkdir -p build
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(OBJS) $(BIN)