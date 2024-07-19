CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: keygen main

keygen: keygen.o
	$(CC) -o $@ $^ $(LFLAGS)

main: main.o
	$(CC) -o $@ $^ $(LFLAGS)

createdFiles:
	rm -f encrypted.txt decrypted.txt

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean: cleanfiles cleankeys createdFiles

cleanfiles:
	rm -f keygen main *.o

cleankeys:
	rm -f *.pub *.priv

format:
	clang-format -i -style=file *.[ch]
