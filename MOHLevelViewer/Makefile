
CC= gcc
CFLAGS=-Wall

Obj = ShaderManager.o VAO.o TSP.o TIM.o Font.o BSD.o BSDTwoPlayers.o VRAM.o MOHLevelViewer.o


ifeq ($(OS),Windows_NT)
	CFLAGS += -Wl,-subsystem,windows
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglew32 -lpng -lm
else
	LDFLAGS = -lSDL2 -lGL -lGLEW -lpng -lm
endif

all: release

debug: CFLAGS +=-D_DEBUG -g
debug: MOHLevelViewer

release: CFLAGS += -O3
release: MOHLevelViewer

MOHLevelViewer: $(Obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(Obj) MOHLevelViewer
