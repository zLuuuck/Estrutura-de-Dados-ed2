#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "tipos.h"

/* Inicializa lista vazia */
void biblioteca_init(ListaDupla *lista);

/* Cria e retorna um novo NodoMusica alocado dinamicamente.
   Chama ffprobe para preencher duracao automaticamente.
   Retorna NULL se alocação falhar. */
NodoMusica *biblioteca_criar_nodo(const char *titulo,
                                  const char *artista,
                                  const char *caminho);

/* Inserções */
void biblioteca_inserir_inicio(ListaDupla *lista, NodoMusica *novo);
void biblioteca_inserir_fim   (ListaDupla *lista, NodoMusica *novo);

/* Busca — retorna ponteiro para o nó ou NULL */
NodoMusica *biblioteca_buscar_titulo (ListaDupla *lista, const char *titulo);
NodoMusica *biblioteca_buscar_artista(ListaDupla *lista, const char *artista);

/* Remoção por título — faz free do nó */
int biblioteca_remover(ListaDupla *lista, const char *titulo);

/* Listagem */
void biblioteca_listar(const ListaDupla *lista);

/* Libera toda a lista */
void biblioteca_liberar(ListaDupla *lista);

#endif /* BIBLIOTECA_H */
