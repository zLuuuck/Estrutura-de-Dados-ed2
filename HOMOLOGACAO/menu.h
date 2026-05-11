#ifndef MENU_H
#define MENU_H

#include "tipos.h"
#include "biblioteca.h"
#include "playlist.h"
#include "pilha.h"
#include "fila.h"

/* Contexto completo passado para todas as funções de menu */
typedef struct {
    ListaDupla     biblioteca;
    ListaPlaylists playlists;
    Pilha          historico;
    Fila           fila_reprod;
    EstadoPlayer   estado;

    /* ponteiro para a playlist em uso e música atual nela */
    NodoPlaylist  *playlist_ativa;
    NodoMusica    *musica_cursor;   /* cursor de navegação na playlist ativa */
} Contexto;

void menu_principal(Contexto *ctx);

#endif /* MENU_H */
