#ifndef TIPOS_H
#define TIPOS_H

/* Tamanhos máximos dos campos de texto */
#define MAX_TITULO   256
#define MAX_ARTISTA  256
#define MAX_CAMINHO  512
#define MAX_NOME_PL  256

/*  Nodo da biblioteca (lista duplamente encadeada)  */
typedef struct NodoMusica {
    char titulo[MAX_TITULO];
    char artista[MAX_ARTISTA];
    char caminho[MAX_CAMINHO];
    int  duracao;               /* duração em segundos, preenchida no cadastro */
    struct NodoMusica *prox;
    struct NodoMusica *ant;
} NodoMusica;

/* Lista duplamente encadeada de músicas (biblioteca ou playlist interna) */
typedef struct {
    NodoMusica *inicio;
    NodoMusica *fim;
    int tam;
} ListaDupla;

/*  Pilha (histórico)  */
/* Armazena PONTEIROS para NodoMusica da biblioteca, nunca cópias. */
typedef struct NodoPilha {
    NodoMusica       *musica;
    struct NodoPilha *prox;
} NodoPilha;

typedef struct {
    NodoPilha *topo;
    int tam;
} Pilha;

/*  Fila de reprodução  */
/* Armazena PONTEIROS para NodoMusica da biblioteca, nunca cópias. */
typedef struct NodoFila {
    NodoMusica      *musica;
    struct NodoFila *prox;
} NodoFila;

typedef struct {
    NodoFila *inicio;
    NodoFila *fim;
    int tam;
} Fila;

/*  Playlists  */
/*
 * Nodo invólucro para a lista interna de cada playlist.
 * Guarda um PONTEIRO para NodoMusica da biblioteca (nunca cópia) e tem seus
 * próprios prox/ant — necessário porque NodoMusica.prox/ant já são usados
 * pela cadeia da biblioteca; reutilizá-los aqui corromperia aquela lista.
 */
typedef struct EntradaPlaylist {
    NodoMusica             *musica;
    struct EntradaPlaylist *prox;
    struct EntradaPlaylist *ant;
} EntradaPlaylist;

typedef struct NodoPlaylist {
    char nome[MAX_NOME_PL];
    EntradaPlaylist *inicio_musicas; /* lista dupla interna (invólucros) */
    EntradaPlaylist *fim_musicas;
    int              tam_musicas;
    struct NodoPlaylist *prox;
    struct NodoPlaylist *ant;
} NodoPlaylist;

typedef struct {
    NodoPlaylist *inicio;
    NodoPlaylist *fim;
    int tam;
} ListaPlaylists;

/*  Estado do player  */
typedef struct {
    NodoMusica    *atual;            /* música em reprodução (pode ser NULL) */
    NodoPlaylist  *playlist_atual;   /* NULL = tocando da biblioteca geral  */
    int            modo_shuffle;     /* 0 = off, 1 = on */
} EstadoPlayer;

#endif /* TIPOS_H */
