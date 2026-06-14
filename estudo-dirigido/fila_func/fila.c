#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

/* Inicializa a fila zerada */
void fila_inicializar(Fila *f)
{
    f->inicio = NULL;
    f->fim = NULL;
    f->tam = 0;
}

/* Enfileira um ponteiro para NodoMusica no fim da fila */
int fila_enqueue(Fila *f, NodoMusica *musica)
{
    NodoFila *novo = (NodoFila *)malloc(sizeof(NodoFila));
    if (!novo)
    {
        fprintf(stderr, "Erro: sem memoria para enfileirar musica.\n");
        return 0;
    }
    novo->musica = musica;
    novo->prox = NULL;

    if (fila_vazia(f))
    {
        f->inicio = novo;
        f->fim = novo;
    }
    else
    {
        f->fim->prox = novo;
        f->fim = novo;
    }
    f->tam++;
    return 1;
}

/* Desenfileira e retorna o ponteiro da frente; NULL se vazia */
NodoMusica *fila_dequeue(Fila *f)
{
    if (fila_vazia(f))
    {
        return NULL;
    }

    NodoFila *removido = f->inicio;
    NodoMusica *musica = removido->musica;

    f->inicio = removido->prox;
    if (!f->inicio)
    {
        f->fim = NULL;
    }
    free(removido);
    f->tam--;
    return musica;
}

/* Retorna o ponteiro da frente sem remover */
NodoMusica *fila_peek(const Fila *f)
{
    if (fila_vazia(f))
    {
        return NULL;
    }
    return f->inicio->musica;
}

/* Retorna 1 se vazia */
int fila_vazia(const Fila *f)
{
    return f->inicio == NULL;
}

/* Retorna o tamanho atual */
int fila_tamanho(const Fila *f)
{
    return f->tam;
}

/* Verifica se um ponteiro de musica ja esta na fila */
int fila_contem(const Fila *f, const NodoMusica *musica)
{
    const NodoFila *atual = f->inicio;
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

/* Exibe a fila da frente para o fim */
void fila_listar(const Fila *f)
{
    if (fila_vazia(f))
    {
        printf("  Fila de reproducao vazia.\n");
        return;
    }
    const NodoFila *atual = f->inicio;
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

/* Libera os NodoFila (nunca os NodoMusica — eles pertencem a biblioteca) */
void fila_liberar(Fila *f)
{
    while (!fila_vazia(f))
    {
        fila_dequeue(f);
    }
}
