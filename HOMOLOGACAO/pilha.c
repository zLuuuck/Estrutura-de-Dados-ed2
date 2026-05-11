#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pilha.h"

/* ── init ───────────────────────────────────────────────────────────────── */
void pilha_init(Pilha *p) {
    p->topo = NULL;
    p->tam  = 0;
}

/* ── push ───────────────────────────────────────────────────────────────── */
void pilha_push(Pilha *p, const NodoMusica *musica) {
    NodoPilha *novo = (NodoPilha *) malloc(sizeof(NodoPilha));
    if (novo == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória na pilha.\n");
        return;
    }

    strncpy(novo->titulo,  musica->titulo,  MAX_TITULO  - 1); novo->titulo[MAX_TITULO-1]   = '\0';
    strncpy(novo->artista, musica->artista, MAX_ARTISTA - 1); novo->artista[MAX_ARTISTA-1] = '\0';
    strncpy(novo->caminho, musica->caminho, MAX_CAMINHO - 1); novo->caminho[MAX_CAMINHO-1] = '\0';
    novo->duracao = musica->duracao;

    novo->prox = p->topo;  /* insere no topo — LIFO */
    p->topo    = novo;
    p->tam++;
}

/* ── pop ────────────────────────────────────────────────────────────────── */
int pilha_pop(Pilha *p, NodoPilha *saida) {
    if (pilha_vazia(p)) {
        printf("Histórico vazio.\n");
        return 0;
    }

    NodoPilha *temp = p->topo;

    if (saida != NULL) {
        strncpy(saida->titulo,  temp->titulo,  MAX_TITULO  - 1); saida->titulo[MAX_TITULO-1]   = '\0';
        strncpy(saida->artista, temp->artista, MAX_ARTISTA - 1); saida->artista[MAX_ARTISTA-1] = '\0';
        strncpy(saida->caminho, temp->caminho, MAX_CAMINHO - 1); saida->caminho[MAX_CAMINHO-1] = '\0';
        saida->duracao = temp->duracao;
        saida->prox    = NULL;
    }

    p->topo = temp->prox;
    free(temp);
    p->tam--;
    return 1;
}

/* ── peek ───────────────────────────────────────────────────────────────── */
int pilha_peek(const Pilha *p, NodoPilha *saida) {
    if (pilha_vazia(p)) return 0;

    if (saida != NULL) {
        strncpy(saida->titulo,  p->topo->titulo,  MAX_TITULO  - 1); saida->titulo[MAX_TITULO-1]   = '\0';
        strncpy(saida->artista, p->topo->artista, MAX_ARTISTA - 1); saida->artista[MAX_ARTISTA-1] = '\0';
        strncpy(saida->caminho, p->topo->caminho, MAX_CAMINHO - 1); saida->caminho[MAX_CAMINHO-1] = '\0';
        saida->duracao = p->topo->duracao;
        saida->prox    = NULL;
    }
    return 1;
}

/* ── vazia ──────────────────────────────────────────────────────────────── */
int pilha_vazia(const Pilha *p) {
    return p->topo == NULL;
}

/* ── listar ─────────────────────────────────────────────────────────────── */
void pilha_listar(const Pilha *p) {
    if (pilha_vazia(p)) {
        printf("  (histórico vazio)\n");
        return;
    }

    int i = 1;
    NodoPilha *temp = p->topo;
    printf("  %-4s %-30s %-25s\n", "#", "Título", "Artista");
    printf("  %s\n", "──────────────────────────────────────────────────────");
    while (temp != NULL) {
        printf("  %-4d %-30s %-25s\n", i++, temp->titulo, temp->artista);
        temp = temp->prox;
    }
}

/* ── liberar ────────────────────────────────────────────────────────────── */
void pilha_liberar(Pilha *p) {
    NodoPilha *temp;
    while (p->topo != NULL) {
        temp   = p->topo;
        p->topo = p->topo->prox;
        free(temp);
    }
    p->tam = 0;
}
