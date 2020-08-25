CC= gcc
CFLAGS=-Wall

Obj = VABWalker.o

LDFLAGS = -lsndfile

all: VABWalker.o

debug: CFLAGS +=-D_DEBUG -g
debug: VABWalker

VABWalker: $(Obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(Obj) VABWalker
