#ifndef FILA_H
#define FILA_H

#include "tipos.h"

void  fila_init    (Fila *f);
void  fila_enqueue (Fila *f, const NodoMusica *musica); /* copia dados */
int   fila_dequeue (Fila *f, NodoFila *saida);          /* copia p/ saida, remove */
int   fila_peek    (const Fila *f, NodoFila *saida);    /* só lê frente */
int   fila_vazia   (const Fila *f);
void  fila_listar  (const Fila *f);
void  fila_liberar (Fila *f);

#endif /* FILA_H */
