#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

int main(void) {
    Contexto ctx;

    /* inicializa todas as estruturas */
    biblioteca_init  (&ctx.biblioteca);
    playlists_init   (&ctx.playlists);
    pilha_init       (&ctx.historico);
    fila_init        (&ctx.fila_reprod);

    ctx.estado.tocando           = 0;
    ctx.estado.pausado           = 0;
    ctx.estado.modo_shuffle      = 0;
    ctx.estado.duracao_atual     = 0;
    ctx.estado.segundos_passados = 0;
    ctx.estado.musica_atual[0]   = '\0';
    ctx.estado.artista_atual[0]  = '\0';

    ctx.playlist_ativa = NULL;
    ctx.musica_cursor  = NULL;

    /* loop principal */
    menu_principal(&ctx);

    /* libera toda a memória antes de sair */
    biblioteca_liberar (&ctx.biblioteca);
    playlists_liberar  (&ctx.playlists);
    pilha_liberar      (&ctx.historico);
    fila_liberar       (&ctx.fila_reprod);

    return 0;
}
