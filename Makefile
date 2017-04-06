OBJS:=main.o terminal.o
BIN:=editor
CFLAGS:=-Wall -Werror -Wextra -pedantic -std=c99 -O2 -Iinclude/

all: $(BIN)

.PHONY: all clean

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN) 

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(OBJS) $(BIN)