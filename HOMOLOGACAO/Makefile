CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200809L
LDFLAGS = -lpthread

TARGET  = music_player

SRCS = main.c       \
       biblioteca.c \
       playlist.c   \
       pilha.c      \
       fila.c       \
       player.c     \
       menu.c

OBJS = $(SRCS:.c=.o)

# ── regra padrão ──────────────────────────────────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ── compilação de cada .c → .o ────────────────────────────────────────────
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ── dependências de cabeçalhos ────────────────────────────────────────────
main.o:        main.c       menu.h tipos.h
biblioteca.o:  biblioteca.c biblioteca.h player.h tipos.h
playlist.o:    playlist.c   playlist.h   tipos.h
pilha.o:       pilha.c      pilha.h      tipos.h
fila.o:        fila.c       fila.h       tipos.h
player.o:      player.c     player.h     pilha.h  tipos.h
menu.o:        menu.c       menu.h       biblioteca.h playlist.h \
               pilha.h fila.h player.h tipos.h

# ── limpeza ───────────────────────────────────────────────────────────────
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
