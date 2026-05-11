#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "tipos.h"

void          playlists_init          (ListaPlaylists *lp);

/* Cria nova playlist com o nome dado */
NodoPlaylist *playlist_criar          (const char *nome);

void          playlists_inserir       (ListaPlaylists *lp, NodoPlaylist *pl);
NodoPlaylist *playlists_buscar        (ListaPlaylists *lp, const char *nome);
int           playlists_remover       (ListaPlaylists *lp, const char *nome);
void          playlists_listar        (const ListaPlaylists *lp);

/* Operações sobre músicas dentro de uma playlist */
void          playlist_add_musica     (NodoPlaylist *pl, const NodoMusica *m);
int           playlist_rem_musica     (NodoPlaylist *pl, const char *titulo);
void          playlist_listar_musicas (const NodoPlaylist *pl);

/* Navegar dentro de uma playlist (retorna nó ou NULL) */
NodoMusica   *playlist_proxima        (NodoPlaylist *pl, NodoMusica *atual);
NodoMusica   *playlist_anterior       (NodoPlaylist *pl, NodoMusica *atual);

void          playlists_liberar       (ListaPlaylists *lp);

#endif /* PLAYLIST_H */
