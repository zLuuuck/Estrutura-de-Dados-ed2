#ifndef PLAYER_H
#define PLAYER_H

#include "tipos.h"

/*
 * Obtém a duração em segundos de um arquivo de áudio usando ffprobe.
 * Retorna 0 se falhar (arquivo não encontrado, ffprobe ausente, etc.).
 */
int player_obter_duracao(const char *caminho);

/*
 * Toca uma música via ffplay (fork + exec).
 * Enquanto toca, uma thread exibe a barra de progresso no terminal.
 * Empurra a música no histórico (pilha) ao iniciar.
 * Bloqueia até a música terminar ou o usuário pressionar ENTER para parar.
 */
void player_tocar(const NodoMusica *musica, Pilha *historico, EstadoPlayer *estado);

/*
 * Exibe a barra de progresso atual.
 * Chamada internamente pela thread, mas exposta para testes.
 *   atual    : segundos já decorridos
 *   total    : duração total em segundos
 *   largura  : número de caracteres da barra
 */
void player_exibir_barra(int atual, int total, int largura);

#endif /* PLAYER_H */
