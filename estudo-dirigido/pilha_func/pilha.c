#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

/* Inicializa a pilha zerada */
void pilha_inicializar(Pilha *p)
{
    p->topo = NULL;
    p->tam = 0;
}

/* Empilha um ponteiro para NodoMusica no topo */
int pilha_push(Pilha *p, NodoMusica *musica)
{
    NodoPilha *novo = (NodoPilha *)malloc(sizeof(NodoPilha));
    if (!novo)
    {
        fprintf(stderr, "Erro: sem memoria para empilhar musica.\n");
        return 0;
    }
    novo->musica = musica;
    novo->prox = p->topo;
    p->topo = novo;
    p->tam++;
    return 1;
}

/* Desempilha e retorna o ponteiro do topo; NULL se vazia */
NodoMusica *pilha_pop(Pilha *p)
{
    if (pilha_vazia(p))
    {
        return NULL;
    }

    NodoPilha *removido = p->topo;
    NodoMusica *musica = removido->musica;

    p->topo = removido->prox;
    free(removido);
    p->tam--;
    return musica;
}

/* Retorna o ponteiro do topo sem remover */
NodoMusica *pilha_peek(const Pilha *p)
{
    if (pilha_vazia(p))
    {
        return NULL;
    }
    return p->topo->musica;
}

/* Retorna 1 se vazia */
int pilha_vazia(const Pilha *p)
{
    return p->topo == NULL;
}

/* Retorna o tamanho atual */
int pilha_tamanho(const Pilha *p)
{
    return p->tam;
}

/* Verifica se um ponteiro de musica ja esta na pilha */
int pilha_contem(const Pilha *p, const NodoMusica *musica)
{
    const NodoPilha *atual = p->topo;
    while (atual)
    {
        if (atual->musica == musica)
        {
            return 1;
        }
        atual = atual->prox;
    }
    return 0;
}

/* Exibe o historico do mais recente ao mais antigo */
void pilha_listar(const Pilha *p)
{
    if (pilha_vazia(p))
    {
        printf("  Historico vazio.\n");
        return;
    }
    const NodoPilha *atual = p->topo;
    int pos = 1;
    printf("  %-4s %-40s %s\n", "Pos", "Titulo", "Artista");
    printf("  %-4s %-40s %s\n", "---", "------", "-------");
    while (atual)
    {
        printf("  %-4d %-40s %s\n", pos, atual->musica->titulo, atual->musica->artista);
        atual = atual->prox;
        pos++;
    }
}

/* Libera os NodoPilha (nunca os NodoMusica — eles pertencem a biblioteca) */
void pilha_liberar(Pilha *p)
{
    while (!pilha_vazia(p))
    {
        pilha_pop(p);
    }
}
