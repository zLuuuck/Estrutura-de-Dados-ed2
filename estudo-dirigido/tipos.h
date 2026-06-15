#ifndef TIPOS_H
#define TIPOS_H

/* Tamanhos máximos dos campos de texto */
#define MAX_TITULO 256
#define MAX_ARTISTA 256
#define MAX_CAMINHO 512
#define MAX_NOME_PL 256

/*  Nodo da biblioteca (lista duplamente encadeada)  */
typedef struct NodoMusica
{
    char titulo[MAX_TITULO];
    char artista[MAX_ARTISTA];
    char caminho[MAX_CAMINHO];
    int duracao; /* duração em segundos, preenchida no cadastro */
    struct NodoMusica *prox;
    struct NodoMusica *ant;
} NodoMusica;

/* Lista duplamente encadeada de músicas (biblioteca ou playlist interna) */
typedef struct
{
    NodoMusica *inicio;
    NodoMusica *fim;
    int tam;
} ListaDupla;

/*
 * PROPRIEDADE DE MEMORIA: a biblioteca e a DONA de todos os NodoMusica.
 * Pilha, fila e playlists armazenam apenas PONTEIROS para eles, nunca copias.
 *
 * Regra derivada disso: uma musica so pode ser removida da biblioteca se
 * nenhuma outra estrutura ainda a referencie. Soltar o nodo com referencias
 * ativas deixaria ponteiros pendentes (dangling pointers) apontando para
 * memoria ja liberada — comportamento indefinido em C.
 *
 * Essa separacao entre dono e referenciador foi o conceito que mais exigiu
 * atencao durante o desenvolvimento: cada funcao de liberacao (pilha_liberar,
 * fila_liberar, playlist_lista_liberar) libera apenas seus proprios nodos de
 * encadeamento, jamais os NodoMusica apontados.
 */

/*  Pilha (historico)  */
/* Armazena PONTEIROS para NodoMusica da biblioteca, nunca copias. */
typedef struct NodoPilha
{
    NodoMusica *musica;
    struct NodoPilha *prox;
} NodoPilha;

typedef struct
{
    NodoPilha *topo;
    int tam;
} Pilha;

/*  Fila de reprodução  */
/* Armazena PONTEIROS para NodoMusica da biblioteca, nunca cópias. */
typedef struct NodoFila
{
    NodoMusica *musica;
    struct NodoFila *prox;
} NodoFila;

typedef struct
{
    NodoFila *inicio;
    NodoFila *fim;
    int tam;
} Fila;

/*  Playlists  */
/*
 * DIFICULDADE: wrapper EntradaPlaylist.
 *
 * Problema: queremos que a mesma musica possa aparecer em varias playlists
 * ao mesmo tempo. A solucao ingenua seria usar os campos prox/ant do proprio
 * NodoMusica para encadear as playlists — mas esses ponteiros ja pertencem
 * a cadeia da biblioteca. Reutiliza-los aqui corromperia a lista da
 * biblioteca inteira (dois "donos" do mesmo par de ponteiros).
 *
 * SOLUCAO: EntradaPlaylist e um involucro (wrapper) com seus proprios
 * prox/ant e um ponteiro para o NodoMusica correto. Assim:
 *   - A mesma musica pode estar em N playlists ao mesmo tempo.
 *   - Cada playlist e uma lista dupla independente de EntradaPlaylist.
 *   - A lista da biblioteca nao e afetada.
 *
 * APRENDIZADO: quando uma estrutura precisa aparecer em multiplos contextos
 * de encadeamento simultaneamente em C, o padrao e usar nos involucro
 * separados em vez de reutilizar os ponteiros do no original.
 */
typedef struct EntradaPlaylist
{
    NodoMusica *musica;
    struct EntradaPlaylist *prox;
    struct EntradaPlaylist *ant;
} EntradaPlaylist;

typedef struct NodoPlaylist
{
    char nome[MAX_NOME_PL];
    EntradaPlaylist *inicio_musicas; /* lista dupla interna (invólucros) */
    EntradaPlaylist *fim_musicas;
    int tam_musicas;
    struct NodoPlaylist *prox;
    struct NodoPlaylist *ant;
} NodoPlaylist;

typedef struct
{
    NodoPlaylist *inicio;
    NodoPlaylist *fim;
    int tam;
} ListaPlaylists;

/*  Estado do player  */
typedef struct
{
    NodoMusica *atual;            /* música em reprodução (pode ser NULL) */
    NodoPlaylist *playlist_atual; /* NULL = tocando da biblioteca geral  */
    int modo_shuffle;             /* 0 = off, 1 = on */
} EstadoPlayer;

#endif /* TIPOS_H */
