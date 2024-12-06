.POSIX:

# MSYS2
CC        = gcc
CPPFLAGS  = -D_POSIX_C_SOURCE=200809L
#CPPFLAGS = -D_POSIX_C_SOURCE=200809L -DNDEBUG
CFLAGS    = -std=c99 -pedantic -Wall -Wextra -g -O0
#CFLAGS   = -std=c99 -pedantic -Wall -Wextra -O2
LDFLAGS   = -mwindows -lopengl32 -lglfw3
GLSLC     = glslc

BIN = triangle.exe
SRC = triangle.c
OBJ = $(SRC:.c=.o)

GLSL = shaders/vertex.glsl shaders/fragment.glsl
SPV  = $(GLSL:.glsl=.spv)

all: $(BIN) $(SPV)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

%.spv: %.glsl
	$(GLSLC) $< -o $@

triangle.o: triangle.c glad.h config.h

clean:
	@rm -f $(BIN) $(OBJ) $(SPV)

run:	all
	@./$(BIN)

.PHONY:	all clean run
