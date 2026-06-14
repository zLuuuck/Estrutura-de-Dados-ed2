#ifndef PLAYER_H
#define PLAYER_H

#include "tipos.h"
#include "../pilha_func/pilha.h"
#include "../fila_func/fila.h"
#include "../playlist_func/playlist.h"

/*
 * Inicia a reproducao de uma musica especifica.
 * Carrega o audio, empilha no historico e atualiza estado->atual.
 * Retorna 1 em sucesso, 0 em falha de audio_load.
 */
int player_tocar(EstadoPlayer *estado,
                 NodoMusica   *musica,
                 Pilha        *historico);

/*
 * Loop de reproducao ao vivo.
 * Exibe barra de status e responde a teclas sem ENTER.
 *
 * Teclas:
 *   k  — pausar / retomar (toggle)
 *   j  — avancar 5 segundos
 *   l  — voltar 5 segundos
 *   p  — proxima musica (fila ou playlist ou biblioteca)
 *   o  — musica anterior (playlist ou biblioteca)
 *   s  — shuffle on/off
 *   q  — parar e voltar ao menu
 *
 * Parametros:
 *   estado     — estado atual do player (mutavel)
 *   historico  — pilha de historico (mutavel)
 *   fila       — fila de reproducao (mutavel)
 *   biblioteca — lista da biblioteca para shuffle/anterior sem playlist
 */
void player_loop(EstadoPlayer  *estado,
                 Pilha         *historico,
                 Fila          *fila,
                 ListaDupla    *biblioteca);

#endif /* PLAYER_H */
