CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude
SRC = src/main.c src/steganography.c src/tui.c
TARGET = steg

all: $(TARGET)
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
clean:
	rm -f $(TARGET)