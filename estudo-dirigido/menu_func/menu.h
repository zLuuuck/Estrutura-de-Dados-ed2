#ifndef MENU_H
#define MENU_H

#include "tipos.h"
#include "../pilha_func/pilha.h"
#include "../fila_func/fila.h"
#include "../playlist_func/playlist.h"

/*
 * Ponto de entrada unico para todos os menus.
 * Recebe ponteiros para todas as estruturas globais e executa o loop principal.
 * So retorna quando o usuario escolher "Sair".
 */
void menu_principal(ListaDupla     *biblioteca,
                    ListaPlaylists *playlists,
                    Pilha          *historico,
                    Fila           *fila,
                    EstadoPlayer   *estado);

#endif /* MENU_H */
