#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#include "tipos.h"
#include "biblioteca_func/biblioteca.h"
#include "playlist_func/playlist.h"
#include "pilha_func/pilha.h"
#include "fila_func/fila.h"
#include "audio_func/audio.h"
#include "menu_func/menu.h"

int main(void)
{
    /* Suporte a acentuacao no terminal */
    setlocale(LC_ALL, "pt_BR.UTF-8");

    /* Semente para shuffle */
    srand((unsigned int)time(NULL));

    /*  Inicializa estruturas de dados */
    ListaDupla biblioteca;
    ListaPlaylists playlists;
    Pilha historico;
    Fila fila;
    EstadoPlayer estado;

    biblioteca_inicializar(&biblioteca);
    playlist_lista_inicializar(&playlists);
    pilha_inicializar(&historico);
    fila_inicializar(&fila);

    estado.atual = NULL;
    estado.playlist_atual = NULL;
    estado.modo_shuffle = 0;

    /*  Inicializa engine de audio  */
    if (!audio_init())
    {
        fprintf(stderr, "Erro fatal: nao foi possivel inicializar o audio.\n");
        return EXIT_FAILURE;
    }

    /*  Loop principal  */
    menu_principal(&biblioteca, &playlists, &historico, &fila, &estado);

    /*  Liberacao de recursos  */
    audio_shutdown();
    fila_liberar(&fila);
    pilha_liberar(&historico);
    playlist_lista_liberar(&playlists);
    biblioteca_liberar(&biblioteca); /* deve ser o ultimo: libera os NodoMusica */

    return EXIT_SUCCESS;
}
