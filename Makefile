CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image gtk+-3.0`
LDFLAGS = -lSDL2 -lSDL2_ttf
LDLIBS = `pkg-config --libs sdl2 SDL2_image gtk+-3.0` -lm 

SRC = main.c grayscale.c solver.c ui.c detection.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

# END
