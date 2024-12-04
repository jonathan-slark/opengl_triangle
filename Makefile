.POSIX:

CC       = gcc
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -DDEBUG -DVK_USE_PLATFORM_WIN32_KHR
#CPPFLAGS = -D_POSIX_C_SOURCE=200809L -DVK_USE_PLATFORM_WIN32_KHR
CFLAGS   = -I/mingw64/include -std=c99 -pedantic -Wall -Wextra -g -O0
#CFLAGS   = -I/mingw64/include -std=c99 -pedantic -Wall -Wextra -O2
LDFLAGS  = -L/mingw64/lib -mwindows -lopengl32
GLSLC    = glslc

BIN = triangle.exe
SRC = al_win32.c triangle.c util.c
OBJ = $(SRC:.c=.o)

GLSL = shaders/vertex.glsl shaders/fragment.glsl
SPV  = $(GLSL:.glsl=.spv)

#all: $(BIN) $(SPV)
all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $<

%.spv: %.glsl
	$(GLSLC) $< -o $@

al_win32.o: al_win32.c al.h util.h
triangle.o: triangle.c al.h config.h util.h
util.o: util.c util.h

clean:
	@rm -f $(BIN) $(OBJ) $(SPV)

run:	all
	@./$(BIN)

.PHONY:	all clean run
