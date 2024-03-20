CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic -gdwarf-4 $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: keygen encrypt decrypt

keygen: keygen.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

encrypt: encrypt.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

decrypt: decrypt.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm -f keygen *.o decrypt *.o encrypt *.o ss *.priv ss *.pub

format:
	clang-format -i -style=file *.[ch]
