#ifndef FILA_H
#define FILA_H

#include "../tipos.h"

/* Inicializa a fila (zera os campos) */
void fila_inicializar(Fila *f);

/* Enfileira um ponteiro para NodoMusica no fim; retorna 1 em sucesso, 0 em falha */
int fila_enqueue(Fila *f, NodoMusica *musica);

/* Desenfileira e retorna o ponteiro da frente; retorna NULL se vazia */
NodoMusica *fila_dequeue(Fila *f);

/* Retorna o ponteiro da frente sem remover; retorna NULL se vazia */
NodoMusica *fila_peek(const Fila *f);

/* Retorna 1 se a fila estiver vazia, 0 caso contrário */
int fila_vazia(const Fila *f);

/* Retorna o número de elementos na fila */
int fila_tamanho(const Fila *f);

/* Verifica se um ponteiro de música já está na fila */
int fila_contem(const Fila *f, const NodoMusica *musica);

/* Exibe as músicas da fila da frente para o fim */
void fila_listar(const Fila *f);

/* Libera todos os nodos da fila (não libera os NodoMusica, apenas os NodoFila) */
void fila_liberar(Fila *f);

#endif /* FILA_H */
