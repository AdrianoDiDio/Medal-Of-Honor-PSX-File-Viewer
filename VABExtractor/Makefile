CC= gcc
CFLAGS=-Wall

Obj = VABExtractor.o

LDFLAGS = -lsndfile

all: VABExtractor

debug: CFLAGS +=-D_DEBUG -g
debug: VABExtractor

VABExtractor: $(Obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(Obj) VABExtractor
