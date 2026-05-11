#ifndef TIPOS_H
#define TIPOS_H

#define MAX_TITULO   256
#define MAX_ARTISTA  256
#define MAX_CAMINHO  512
#define MAX_NOME_PL  256
#define BARRA_LARGURA 40

/* ──────────────────────────────────────────────
   Nó da Biblioteca / Playlist (lista dupla)
   ────────────────────────────────────────────── */
typedef struct NodoMusica {
    char   titulo[MAX_TITULO];
    char   artista[MAX_ARTISTA];
    char   caminho[MAX_CAMINHO];   /* caminho absoluto/relativo do arquivo */
    int    duracao;                /* segundos, obtido via ffprobe          */

    struct NodoMusica *prox;
    struct NodoMusica *ant;
} NodoMusica;

/* ──────────────────────────────────────────────
   Lista duplamente encadeada genérica
   (usada para biblioteca e para cada playlist)
   ────────────────────────────────────────────── */
typedef struct {
    NodoMusica *inicio;
    NodoMusica *fim;
    int         tam;
} ListaDupla;

/* ──────────────────────────────────────────────
   Nó da Pilha de histórico  (lista simples)
   ────────────────────────────────────────────── */
typedef struct NodoPilha {
    char   titulo[MAX_TITULO];
    char   artista[MAX_ARTISTA];
    char   caminho[MAX_CAMINHO];
    int    duracao;

    struct NodoPilha *prox;
} NodoPilha;

typedef struct {
    NodoPilha *topo;
    int        tam;
} Pilha;

/* ──────────────────────────────────────────────
   Nó da Fila de reprodução  (lista simples)
   ────────────────────────────────────────────── */
typedef struct NodoFila {
    char   titulo[MAX_TITULO];
    char   artista[MAX_ARTISTA];
    char   caminho[MAX_CAMINHO];
    int    duracao;

    struct NodoFila *prox;
} NodoFila;

typedef struct {
    NodoFila *inicio;
    NodoFila *fim;
    int       tam;
} Fila;

/* ──────────────────────────────────────────────
   Nó de Playlist  (lista de playlists)
   ────────────────────────────────────────────── */
typedef struct NodoPlaylist {
    char       nome[MAX_NOME_PL];
    ListaDupla musicas;            /* lista dupla interna de músicas */

    struct NodoPlaylist *prox;
    struct NodoPlaylist *ant;
} NodoPlaylist;

typedef struct {
    NodoPlaylist *inicio;
    NodoPlaylist *fim;
    int           tam;
} ListaPlaylists;

/* ──────────────────────────────────────────────
   Estado global do player
   ────────────────────────────────────────────── */
typedef struct {
    int    tocando;          /* 1 = tocando, 0 = parado/pausado  */
    int    pausado;
    int    modo_shuffle;
    char   musica_atual[MAX_TITULO];
    char   artista_atual[MAX_ARTISTA];
    int    duracao_atual;    /* segundos */
    int    segundos_passados;
} EstadoPlayer;

#endif /* TIPOS_H */
