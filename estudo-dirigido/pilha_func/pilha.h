#ifndef PILHA_H
#define PILHA_H

#include "tipos.h"

/* Inicializa a pilha (zera os campos) */
void pilha_inicializar(Pilha *p);

/* Empilha um ponteiro para NodoMusica; retorna 1 em sucesso, 0 em falha */
int pilha_push(Pilha *p, NodoMusica *musica);

/* Desempilha e retorna o ponteiro do topo; retorna NULL se vazia */
NodoMusica *pilha_pop(Pilha *p);

/* Retorna o ponteiro do topo sem remover; retorna NULL se vazia */
NodoMusica *pilha_peek(const Pilha *p);

/* Retorna 1 se a pilha estiver vazia, 0 caso contrário */
int pilha_vazia(const Pilha *p);

/* Retorna o número de elementos na pilha */
int pilha_tamanho(const Pilha *p);

/* Verifica se um ponteiro de música já está na pilha (para evitar duplicatas no histórico) */
int pilha_contem(const Pilha *p, const NodoMusica *musica);

/* Exibe todas as músicas da pilha do topo para a base */
void pilha_listar(const Pilha *p);

/* Libera todos os nodos da pilha (não libera os NodoMusica, apenas os NodoPilha) */
void pilha_liberar(Pilha *p);

#endif /* PILHA_H */
