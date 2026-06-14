#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "tipos.h"

/*  Gerenciamento da lista de playlists  */

/* Inicializa a lista de playlists (vazia) */
void playlist_lista_inicializar(ListaPlaylists *lp);

/* Cria uma nova playlist com o nome dado; retorna o NodoPlaylist ou NULL em falha */
NodoPlaylist *playlist_criar(ListaPlaylists *lp, const char *nome);

/*
 * Remove e libera uma playlist (inclusive todos os seus EntradaPlaylist).
 * NAO libera os NodoMusica — eles pertencem a biblioteca.
 * Retorna 1 em sucesso, 0 se nao encontrado.
 */
int playlist_remover(ListaPlaylists *lp, NodoPlaylist *alvo);

/* Busca playlist pelo nome (exato, case-sensitive); retorna ponteiro ou NULL */
NodoPlaylist *playlist_buscar(const ListaPlaylists *lp, const char *nome);

/* Lista todas as playlists com numero de musicas */
void playlist_listar(const ListaPlaylists *lp);

/*  Musicas dentro de uma playlist  */

/*
 * Adiciona um ponteiro para musica no fim da lista interna da playlist.
 * Retorna 1 em sucesso, 0 em falha ou se a musica ja estiver na playlist.
 */
int playlist_adicionar_musica(NodoPlaylist *pl, NodoMusica *musica);

/*
 * Remove a entrada que aponta para musica da lista interna.
 * Libera apenas o EntradaPlaylist, nunca o NodoMusica.
 * Retorna 1 em sucesso, 0 se nao encontrada.
 */
int playlist_remover_musica(NodoPlaylist *pl, NodoMusica *musica);

/* Lista as musicas de uma playlist numeradas */
void playlist_listar_musicas(const NodoPlaylist *pl);

/* Retorna o NodoMusica seguinte ao atual dentro da playlist; NULL se ultimo */
NodoMusica *playlist_proxima(const NodoPlaylist *pl, const NodoMusica *atual);

/* Retorna o NodoMusica anterior ao atual dentro da playlist; NULL se primeiro */
NodoMusica *playlist_anterior(const NodoPlaylist *pl, const NodoMusica *atual);

/* Retorna o primeiro NodoMusica da playlist; NULL se vazia */
NodoMusica *playlist_primeira(const NodoPlaylist *pl);

/*  Callback para biblioteca_remover  */
/*
 * Verifica se musica esta em qualquer playlist de lp.
 * Assinatura compativel com o parametro de biblioteca_remover.
 */
int playlist_em_alguma(const void *lista_playlists, const NodoMusica *musica);

/*  Liberacao total  */
/* Libera todas as playlists e todos os seus EntradaPlaylist */
void playlist_lista_liberar(ListaPlaylists *lp);

#endif /* PLAYLIST_H */
