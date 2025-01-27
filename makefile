CC = gcc

SRC = music.c videodown.c

OUTPUT = program

CFLAGS = -Wall -g 
LDFLAGS = -lraylib -lm  # Add -lm to link the math library

all: $(OUTPUT)

$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SRC) $(LDFLAGS)

run: $(OUTPUT)
	./$(OUTPUT)

format:
	clang-format -i $(SRC)

clean:
	rm -f $(OUTPUT)
