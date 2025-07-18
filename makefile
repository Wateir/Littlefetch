CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lalpm
BIN_DIR = ${HOME}/.local/bin
TARGET = ltfetch
SRC = main.c

.PHONY: all build install clean

all: build

build: $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

install: build
	mkdir -p $(BIN_DIR)
	cp $(TARGET) $(BIN_DIR)/

clean:
	rm -f $(TARGET) $(BIN_DIR)/$(TARGET)

