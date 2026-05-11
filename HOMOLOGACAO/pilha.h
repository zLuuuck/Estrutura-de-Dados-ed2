#ifndef PILHA_H
#define PILHA_H

#include "tipos.h"

void  pilha_init   (Pilha *p);
void  pilha_push   (Pilha *p, const NodoMusica *musica);  /* copia dados */
int   pilha_pop    (Pilha *p, NodoPilha *saida);          /* copia p/ saida, remove */
int   pilha_peek   (const Pilha *p, NodoPilha *saida);    /* só lê topo */
int   pilha_vazia  (const Pilha *p);
void  pilha_listar (const Pilha *p);
void  pilha_liberar(Pilha *p);

#endif /* PILHA_H */
