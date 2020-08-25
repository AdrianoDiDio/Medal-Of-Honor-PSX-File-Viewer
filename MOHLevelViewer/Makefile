
CC= gcc
CFLAGS=-Wall

Obj = GLLoaderCore.o ShaderManager.o Vao.o TSP.o Tim.o BSD.o BSDTwoPlayers.o VRam.o MOHLevelViewer.o

LDFLAGS = -lSDL2 -lpng -lm

all: MOHLevelViewer

debug: CFLAGS +=-D_DEBUG -g
debug: MOHLevelViewer

MOHLevelViewer: $(Obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(Obj) MOHLevelViewer
