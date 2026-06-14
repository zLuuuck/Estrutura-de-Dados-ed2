# ============================================================
#  Makefile — Player de Musica (C99)
#  Compativel com:
#    - Linux nativo            : make
#    - Windows nativo (MinGW)  : mingw32-make
#    - Cross Linux → Windows   : make windows
# ============================================================

#  Deteccao de plataforma 
ifeq ($(OS),Windows_NT)
    TARGET  = player.exe
    LIBS    = -lole32 -lwinmm
    RM      = del /Q
    RMDIR   = rmdir /S /Q
    SLASH   = \\
    POSIX_DEF =
else
    TARGET  = player
    LIBS    = -lpthread -lm -ldl
    RM      = rm -f
    RMDIR   = rm -rf
    SLASH   = /
    POSIX_DEF = -D_POSIX_C_SOURCE=200112L
endif

#  Compilador e flags 
CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -g $(POSIX_DEF) \
          -I. \
          -Ibiblioteca_func \
          -Ipilha_func \
          -Ifila_func \
          -Iplaylist_func \
          -Iinput_func \
          -Iaudio_func \
          -Iplayer_func \
          -Imenu_func

#  Fontes e objetos 
SRCS = main.c \
       pilha_func/pilha.c \
       fila_func/fila.c \
       biblioteca_func/biblioteca.c \
       playlist_func/playlist.c \
       input_func/input.c \
       audio_func/audio.c \
       player_func/player.c \
       menu_func/menu.c

OBJS = $(SRCS:.c=.o)

#  Regras 
.PHONY: all clean windows clean-windows clean-all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo ""
	@echo "  Compilado com sucesso: $(TARGET)"
	@echo ""

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra especial para audio.c: compilado SEM -Wall -Wextra.
# Motivo: miniaudio.h e uma biblioteca de terceiros e dispara centenas de
# avisos com as flags de aviso extras. Compilar o resto do projeto com -Wall
# -Wextra e compilar so audio.c sem elas permite manter o rigor no nosso
# codigo sem poluir a saida com alertas de codigo que nao controlamos.
# Esse padrao (flag diferente por arquivo) foi sugerido pela IA ao ajudar
# com a integracao da miniaudio.
audio_func/audio.o: audio_func/audio.c audio_func/audio.h audio_func/miniaudio.h
	$(CC) -std=c99 -I. -Iaudio_func -c $< -o $@

#  Cross-compilation Linux → Windows (.exe) 
# Uso: make windows
# Requer: sudo apt install mingw-w64
windows:
	x86_64-w64-mingw32-gcc -std=c99 -Wall -Wextra -O2 \
	  -static-libgcc -static \
	  -I. -Ibiblioteca_func -Ipilha_func -Ifila_func \
	  -Iplaylist_func -Iinput_func -Iaudio_func \
	  -Iplayer_func -Imenu_func \
	  $(SRCS) \
	  -o player.exe \
	  -lole32 -lwinmm
	@echo ""
	@echo "  Compilado para Windows: player.exe"
	@echo ""

#  Limpeza 
# Linux: 'make clean'        → apaga .o e o binario nativo
# Windows: 'make clean-windows' (no Linux) → apaga player.exe gerado pelo cross
# Tudo: 'make clean-all'     → apaga ambos
clean:
ifeq ($(OS),Windows_NT)
	-$(RM) $(subst /,$(SLASH),$(OBJS)) $(TARGET)
else
	$(RM) $(OBJS) $(TARGET)
endif
	@echo "  Limpeza concluida."

clean-windows:
	$(RM) player.exe
	@echo "  player.exe removido."

clean-all: clean clean-windows
	@echo "  Tudo limpo."