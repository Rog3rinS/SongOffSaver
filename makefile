CC = gcc

SRC = music.c

OUTPUT = music

CFLAGS = 
LDFLAGS = -lraylib

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC) $(LDFLAGS)

run: $(OUTPUT)
	./$(OUTPUT)

format:
	clang-format -i $(SRC)

clean:
	rm -f $(OUTPUT)
