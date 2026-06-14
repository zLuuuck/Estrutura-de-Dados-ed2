#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "../tipos.h"
#include "../pilha_func/pilha.h"
#include "../fila_func/fila.h"

/* Inicializa a biblioteca (lista dupla vazia) */
void biblioteca_inicializar(ListaDupla *lib);

/*
 * Cadastra uma nova musica na biblioteca.
 * Retorna ponteiro para o NodoMusica criado, ou NULL em falha.
 * duracao deve ser obtido via audio antes de chamar esta funcao.
 */
NodoMusica *biblioteca_cadastrar(ListaDupla *lib, const char *titulo, const char *artista, const char *caminho, int duracao);

/*
 * Remove uma musica da biblioteca pelo ponteiro.
 * Antes de remover, verifica se o nodo esta referenciado em:
 *   - historico (Pilha)
 *   - fila de reproducao (Fila)
 *   - qualquer playlist (ListaPlaylists)
 * Se estiver em uso, exibe mensagem e retorna 0.
 * Retorna 1 em sucesso.
 *
 * Nota: lista_playlists e passada como void* para evitar dependencia circular;
 * a verificacao real e feita em playlist_func via callback.
 */
int biblioteca_remover(ListaDupla *lib, NodoMusica *alvo, const Pilha *historico, const Fila *fila, int (*em_alguma_playlist)(const void *lp, const NodoMusica *m), const void *lista_playlists);

/* Busca por titulo (correspondencia parcial, case-insensitive); retorna primeiro achado ou NULL */
NodoMusica *biblioteca_buscar_titulo(const ListaDupla *lib, const char *titulo);

/* Busca por artista (correspondencia parcial, case-insensitive); retorna primeiro achado ou NULL */
NodoMusica *biblioteca_buscar_artista(const ListaDupla *lib, const char *artista);

/* Insere uma nova musica no inicio da lista */
NodoMusica *biblioteca_inserir_inicio(ListaDupla *lib, const char *titulo, const char *artista, const char *caminho, int duracao);

/* Insere uma nova musica em posicao arbitraria (1=inicio, tam+1=fim) */
NodoMusica *biblioteca_inserir_em_posicao(ListaDupla *lib, int pos, const char *titulo, const char *artista, const char *caminho, int duracao);

/* Lista todas as musicas numeradas */
void biblioteca_listar(const ListaDupla *lib);

/* Retorna o total de musicas cadastradas */
int biblioteca_tamanho(const ListaDupla *lib);

/* Libera todos os NodoMusica da biblioteca */
void biblioteca_liberar(ListaDupla *lib);

#endif /* BIBLIOTECA_H */
