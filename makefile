CC       = gcc
CFLAGS   = -Wall -Wextra -O2
LDFLAGS  = -lalpm
BIN_DIR  = $(HOME)/.local/bin
TARGET   = prog
SRC      = main.c
OBJ      = $(SRC:.c=.o)

.PHONY: all build install uninstall clean

all: build

build: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET)
	mkdir -p "$(DESTDIR)$(BIN_DIR)"
	cp "$(TARGET)" "$(DESTDIR)$(BIN_DIR)/$(TARGET)"

uninstall:
	rm -f "$(DESTDIR)$(BIN_DIR)/$(TARGET)"

clean:
	rm -f $(TARGET) $(OBJ)
