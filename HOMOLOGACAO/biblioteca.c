#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biblioteca.h"
#include "player.h"   /* para player_obter_duracao() */

/* ── init ───────────────────────────────────────────────────────────────── */
void biblioteca_init(ListaDupla *lista) {
    lista->inicio = NULL;
    lista->fim    = NULL;
    lista->tam    = 0;
}

/* ── criar nodo ─────────────────────────────────────────────────────────── */
NodoMusica *biblioteca_criar_nodo(const char *titulo,
                                  const char *artista,
                                  const char *caminho) {
    NodoMusica *novo = (NodoMusica *) malloc(sizeof(NodoMusica));
    if (novo == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória para música.\n");
        return NULL;
    }

    strncpy(novo->titulo,  titulo,  MAX_TITULO  - 1); novo->titulo[MAX_TITULO-1]   = '\0';
    strncpy(novo->artista, artista, MAX_ARTISTA - 1); novo->artista[MAX_ARTISTA-1] = '\0';
    strncpy(novo->caminho, caminho, MAX_CAMINHO - 1); novo->caminho[MAX_CAMINHO-1] = '\0';

    novo->duracao = player_obter_duracao(caminho); /* ffprobe */
    novo->prox    = NULL;
    novo->ant     = NULL;

    return novo;
}

/* ── inserção no início ─────────────────────────────────────────────────── */
void biblioteca_inserir_inicio(ListaDupla *lista, NodoMusica *novo) {
    if (novo == NULL) return;

    novo->prox = lista->inicio;
    novo->ant  = NULL;

    if (lista->inicio != NULL)
        lista->inicio->ant = novo;
    else
        lista->fim = novo;   /* lista estava vazia */

    lista->inicio = novo;
    lista->tam++;
}

/* ── inserção no fim ────────────────────────────────────────────────────── */
void biblioteca_inserir_fim(ListaDupla *lista, NodoMusica *novo) {
    if (novo == NULL) return;

    novo->prox = NULL;
    novo->ant  = lista->fim;

    if (lista->fim != NULL)
        lista->fim->prox = novo;
    else
        lista->inicio = novo;  /* lista estava vazia */

    lista->fim = novo;
    lista->tam++;
}

/* ── busca por título ───────────────────────────────────────────────────── */
NodoMusica *biblioteca_buscar_titulo(ListaDupla *lista, const char *titulo) {
    NodoMusica *temp = lista->inicio;
    while (temp != NULL) {
        if (strncasecmp(temp->titulo, titulo, MAX_TITULO) == 0)
            return temp;
        temp = temp->prox;
    }
    return NULL;
}

/* ── busca por artista (retorna primeiro encontrado) ────────────────────── */
NodoMusica *biblioteca_buscar_artista(ListaDupla *lista, const char *artista) {
    NodoMusica *temp = lista->inicio;
    while (temp != NULL) {
        if (strncasecmp(temp->artista, artista, MAX_ARTISTA) == 0)
            return temp;
        temp = temp->prox;
    }
    return NULL;
}

/* ── remoção por título ─────────────────────────────────────────────────── */
int biblioteca_remover(ListaDupla *lista, const char *titulo) {
    NodoMusica *alvo = biblioteca_buscar_titulo(lista, titulo);
    if (alvo == NULL) {
        printf("Música \"%s\" não encontrada na biblioteca.\n", titulo);
        return 0;
    }

    /* Reconecta vizinhos */
    if (alvo->ant != NULL)
        alvo->ant->prox = alvo->prox;
    else
        lista->inicio = alvo->prox;   /* era o primeiro */

    if (alvo->prox != NULL)
        alvo->prox->ant = alvo->ant;
    else
        lista->fim = alvo->ant;       /* era o último */

    free(alvo);
    lista->tam--;
    return 1;
}

/* ── listagem ───────────────────────────────────────────────────────────── */
void biblioteca_listar(const ListaDupla *lista) {
    if (lista->inicio == NULL) {
        printf("  (biblioteca vazia)\n");
        return;
    }

    int i = 1;
    NodoMusica *temp = lista->inicio;
    printf("  %-4s %-30s %-25s %s\n", "#", "Título", "Artista", "Duração");
    printf("  %s\n", "─────────────────────────────────────────────────────────────");
    while (temp != NULL) {
        int min = temp->duracao / 60;
        int seg = temp->duracao % 60;
        printf("  %-4d %-30s %-25s %02d:%02d\n",
               i++, temp->titulo, temp->artista, min, seg);
        temp = temp->prox;
    }
}

/* ── liberar lista ──────────────────────────────────────────────────────── */
void biblioteca_liberar(ListaDupla *lista) {
    NodoMusica *temp;
    while (lista->inicio != NULL) {
        temp          = lista->inicio;
        lista->inicio = lista->inicio->prox;
        free(temp);
    }
    lista->fim = NULL;
    lista->tam = 0;
}
