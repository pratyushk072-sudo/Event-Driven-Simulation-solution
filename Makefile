CC = gcc
CFLAGS = -Wall -O2 -Iinclude
SRC = src/qSim.c
OUT = bin/qSim

all: $(OUT)

$(OUT): $(SRC) include/qSim.h
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) -lm

clean:
	rm -f $(OUT) output/*.txt
