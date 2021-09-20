CC=clang
CFLAGS=-Wall -g

ODIR=.
_OBJ =  src/type/compact.o src/type/fixed_int.o src/util/hex.o src/scale.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ src/test/compact.c src/test/fixed_int.c src/test/test.c $(CFLAGS)

cli: $(OBJ)
	$(CC) -o $@ $^ src/cli.c $(CFLAGS)

all: cli test

.PHONY: clean

clean:
	rm -f test cli src/*.o src/test/*.o src/type/*.o src/util/*.o *~
