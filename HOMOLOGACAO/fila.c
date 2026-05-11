#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fila.h"

/* ── init ───────────────────────────────────────────────────────────────── */
void fila_init(Fila *f) {
    f->inicio = NULL;
    f->fim    = NULL;
    f->tam    = 0;
}

/* ── enqueue (insere no fim) ────────────────────────────────────────────── */
void fila_enqueue(Fila *f, const NodoMusica *musica) {
    NodoFila *novo = (NodoFila *) malloc(sizeof(NodoFila));
    if (novo == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória na fila.\n");
        return;
    }

    strncpy(novo->titulo,  musica->titulo,  MAX_TITULO  - 1); novo->titulo[MAX_TITULO-1]   = '\0';
    strncpy(novo->artista, musica->artista, MAX_ARTISTA - 1); novo->artista[MAX_ARTISTA-1] = '\0';
    strncpy(novo->caminho, musica->caminho, MAX_CAMINHO - 1); novo->caminho[MAX_CAMINHO-1] = '\0';
    novo->duracao = musica->duracao;
    novo->prox    = NULL;

    if (f->fim != NULL)
        f->fim->prox = novo;
    else
        f->inicio = novo;  /* fila estava vazia */

    f->fim = novo;
    f->tam++;
}

/* ── dequeue (remove do início) ─────────────────────────────────────────── */
int fila_dequeue(Fila *f, NodoFila *saida) {
    if (fila_vazia(f)) {
        printf("Fila de reprodução vazia.\n");
        return 0;
    }

    NodoFila *temp = f->inicio;

    if (saida != NULL) {
        strncpy(saida->titulo,  temp->titulo,  MAX_TITULO  - 1); saida->titulo[MAX_TITULO-1]   = '\0';
        strncpy(saida->artista, temp->artista, MAX_ARTISTA - 1); saida->artista[MAX_ARTISTA-1] = '\0';
        strncpy(saida->caminho, temp->caminho, MAX_CAMINHO - 1); saida->caminho[MAX_CAMINHO-1] = '\0';
        saida->duracao = temp->duracao;
        saida->prox    = NULL;
    }

    f->inicio = temp->prox;
    if (f->inicio == NULL)
        f->fim = NULL;  /* fila ficou vazia */

    free(temp);
    f->tam--;
    return 1;
}

/* ── peek ───────────────────────────────────────────────────────────────── */
int fila_peek(const Fila *f, NodoFila *saida) {
    if (fila_vazia(f)) return 0;

    if (saida != NULL) {
        strncpy(saida->titulo,  f->inicio->titulo,  MAX_TITULO  - 1); saida->titulo[MAX_TITULO-1]   = '\0';
        strncpy(saida->artista, f->inicio->artista, MAX_ARTISTA - 1); saida->artista[MAX_ARTISTA-1] = '\0';
        strncpy(saida->caminho, f->inicio->caminho, MAX_CAMINHO - 1); saida->caminho[MAX_CAMINHO-1] = '\0';
        saida->duracao = f->inicio->duracao;
        saida->prox    = NULL;
    }
    return 1;
}

/* ── vazia ──────────────────────────────────────────────────────────────── */
int fila_vazia(const Fila *f) {
    return f->inicio == NULL;
}

/* ── listar ─────────────────────────────────────────────────────────────── */
void fila_listar(const Fila *f) {
    if (fila_vazia(f)) {
        printf("  (fila de reprodução vazia)\n");
        return;
    }

    int i = 1;
    NodoFila *temp = f->inicio;
    printf("  %-4s %-30s %-25s %s\n", "#", "Título", "Artista", "Duração");
    printf("  %s\n", "──────────────────────────────────────────────────────────");
    while (temp != NULL) {
        int min = temp->duracao / 60;
        int seg = temp->duracao % 60;
        printf("  %-4d %-30s %-25s %02d:%02d\n",
               i++, temp->titulo, temp->artista, min, seg);
        temp = temp->prox;
    }
}

/* ── liberar ────────────────────────────────────────────────────────────── */
void fila_liberar(Fila *f) {
    NodoFila *temp;
    while (f->inicio != NULL) {
        temp      = f->inicio;
        f->inicio = f->inicio->prox;
        free(temp);
    }
    f->fim = NULL;
    f->tam = 0;
}
