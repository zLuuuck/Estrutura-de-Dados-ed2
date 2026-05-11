#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "menu.h"
#include "player.h"

/* ── utilitários ─────────────────────────────────────────────────────────── */

static void limpar_tela(void) {
    printf("\033[2J\033[H");  /* ANSI: limpa tela e move cursor pro topo */
}

static void pausar(void) {
    printf("\n  Pressione ENTER para continuar...");
    while (getchar() != '\n');
}

/* Lê uma linha do stdin, remove o '\n' final */
static void ler_linha(const char *prompt, char *buf, int tam) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, tam, stdin) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}

static void cabecalho(const char *titulo) {
    limpar_tela();
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║  ♫  Music_Player  —  Melodia_Code   ║\n");
    printf("  ╚══════════════════════════════════════╝\n");
    printf("  >> %s\n\n", titulo);
}

/* ─────────────────────────────────────────────────────────────────────────
   MENU BIBLIOTECA
   ───────────────────────────────────────────────────────────────────────── */

static void menu_biblioteca(Contexto *ctx) {
    int op;
    char titulo[MAX_TITULO], artista[MAX_ARTISTA], caminho[MAX_CAMINHO];

    do {
        cabecalho("Biblioteca");
        printf("  1. Listar músicas\n");
        printf("  2. Adicionar música\n");
        printf("  3. Remover música\n");
        printf("  4. Buscar por título\n");
        printf("  5. Buscar por artista\n");
        printf("  0. Voltar\n\n");
        ler_linha("  Opção: ", titulo, sizeof(titulo));
        op = atoi(titulo);

        switch (op) {
            case 1:
                cabecalho("Biblioteca — Todas as Músicas");
                biblioteca_listar(&ctx->biblioteca);
                pausar();
                break;

            case 2:
                cabecalho("Adicionar Música");
                ler_linha("  Título  : ", titulo,   sizeof(titulo));
                ler_linha("  Artista : ", artista,  sizeof(artista));
                ler_linha("  Caminho : ", caminho,  sizeof(caminho));

                if (strlen(titulo) == 0 || strlen(caminho) == 0) {
                    printf("  Título e caminho são obrigatórios.\n");
                    pausar(); break;
                }

                printf("  Lendo duração via ffprobe...\n");
                NodoMusica *novo = biblioteca_criar_nodo(titulo, artista, caminho);
                if (novo == NULL) { pausar(); break; }

                if (novo->duracao == 0)
                    printf("  Aviso: duração não detectada (ffprobe falhou ou arquivo inválido).\n");
                else
                    printf("  Duração detectada: %02d:%02d\n", novo->duracao/60, novo->duracao%60);

                biblioteca_inserir_fim(&ctx->biblioteca, novo);
                printf("  Música adicionada com sucesso!\n");
                pausar();
                break;

            case 3:
                cabecalho("Remover Música");
                ler_linha("  Título a remover: ", titulo, sizeof(titulo));
                if (biblioteca_remover(&ctx->biblioteca, titulo))
                    printf("  Música removida.\n");
                pausar();
                break;

            case 4:
                cabecalho("Buscar por Título");
                ler_linha("  Título: ", titulo, sizeof(titulo));
                {
                    NodoMusica *r = biblioteca_buscar_titulo(&ctx->biblioteca, titulo);
                    if (r)
                        printf("  Encontrado: %s — %s (%02d:%02d) [%s]\n",
                               r->titulo, r->artista, r->duracao/60, r->duracao%60, r->caminho);
                    else
                        printf("  Não encontrado.\n");
                }
                pausar();
                break;

            case 5:
                cabecalho("Buscar por Artista");
                ler_linha("  Artista: ", artista, sizeof(artista));
                {
                    NodoMusica *r = biblioteca_buscar_artista(&ctx->biblioteca, artista);
                    if (r)
                        printf("  Encontrado: %s — %s (%02d:%02d)\n",
                               r->titulo, r->artista, r->duracao/60, r->duracao%60);
                    else
                        printf("  Não encontrado.\n");
                }
                pausar();
                break;
        }
    } while (op != 0);
}

/* ─────────────────────────────────────────────────────────────────────────
   MENU PLAYLISTS
   ───────────────────────────────────────────────────────────────────────── */

static void menu_playlist_interna(Contexto *ctx, NodoPlaylist *pl) {
    int op;
    char buf[MAX_TITULO];

    do {
        cabecalho(pl->nome);
        playlist_listar_musicas(pl);
        printf("\n");
        printf("  1. Adicionar música da biblioteca\n");
        printf("  2. Remover música\n");
        printf("  3. Reproduzir playlist do início\n");
        printf("  4. Próxima música\n");
        printf("  5. Música anterior\n");
        printf("  6. Modo aleatório (shuffle)\n");
        printf("  0. Voltar\n\n");
        ler_linha("  Opção: ", buf, sizeof(buf));
        op = atoi(buf);

        switch (op) {
            case 1: {
                ler_linha("  Título da música: ", buf, sizeof(buf));
                NodoMusica *m = biblioteca_buscar_titulo(&ctx->biblioteca, buf);
                if (m == NULL) { printf("  Música não encontrada na biblioteca.\n"); pausar(); break; }
                playlist_add_musica(pl, m);
                printf("  Adicionada à playlist.\n");
                pausar();
                break;
            }
            case 2:
                ler_linha("  Título a remover: ", buf, sizeof(buf));
                playlist_rem_musica(pl, buf);
                pausar();
                break;

            case 3:
                ctx->playlist_ativa = pl;
                ctx->musica_cursor  = pl->musicas.inicio;
                if (ctx->musica_cursor == NULL) {
                    printf("  Playlist vazia.\n"); pausar(); break;
                }
                player_tocar(ctx->musica_cursor, &ctx->historico, &ctx->estado);
                ctx->musica_cursor = ctx->musica_cursor->prox;
                break;

            case 4:
                ctx->playlist_ativa = pl;
                ctx->musica_cursor  = playlist_proxima(pl, ctx->musica_cursor);
                if (ctx->musica_cursor == NULL) {
                    printf("  Fim da playlist.\n"); pausar(); break;
                }
                player_tocar(ctx->musica_cursor, &ctx->historico, &ctx->estado);
                break;

            case 5:
                ctx->playlist_ativa = pl;
                ctx->musica_cursor  = playlist_anterior(pl, ctx->musica_cursor);
                if (ctx->musica_cursor == NULL) {
                    printf("  Início da playlist.\n"); pausar(); break;
                }
                player_tocar(ctx->musica_cursor, &ctx->historico, &ctx->estado);
                break;

            case 6: {
                /* shuffle: escolhe aleatoriamente dentre as músicas da playlist */
                if (pl->musicas.tam == 0) { printf("  Playlist vazia.\n"); pausar(); break; }
                srand((unsigned) time(NULL));
                int idx = rand() % pl->musicas.tam;
                NodoMusica *m = pl->musicas.inicio;
                for (int i = 0; i < idx; i++) m = m->prox;
                ctx->musica_cursor = m;
                player_tocar(m, &ctx->historico, &ctx->estado);
                break;
            }
        }
    } while (op != 0);
}

static void menu_playlists(Contexto *ctx) {
    int op;
    char buf[MAX_NOME_PL];

    do {
        cabecalho("Playlists");
        playlists_listar(&ctx->playlists);
        printf("\n");
        printf("  1. Criar playlist\n");
        printf("  2. Abrir playlist\n");
        printf("  3. Remover playlist\n");
        printf("  0. Voltar\n\n");
        ler_linha("  Opção: ", buf, sizeof(buf));
        op = atoi(buf);

        switch (op) {
            case 1:
                ler_linha("  Nome da playlist: ", buf, sizeof(buf));
                if (strlen(buf) == 0) break;
                {
                    NodoPlaylist *pl = playlist_criar(buf);
                    playlists_inserir(&ctx->playlists, pl);
                    printf("  Playlist \"%s\" criada.\n", buf);
                }
                pausar();
                break;

            case 2:
                ler_linha("  Nome da playlist: ", buf, sizeof(buf));
                {
                    NodoPlaylist *pl = playlists_buscar(&ctx->playlists, buf);
                    if (pl == NULL) { printf("  Playlist não encontrada.\n"); pausar(); break; }
                    menu_playlist_interna(ctx, pl);
                }
                break;

            case 3:
                ler_linha("  Nome a remover: ", buf, sizeof(buf));
                if (playlists_remover(&ctx->playlists, buf))
                    printf("  Playlist removida.\n");
                pausar();
                break;
        }
    } while (op != 0);
}

/* ─────────────────────────────────────────────────────────────────────────
   MENU FILA DE REPRODUÇÃO
   ───────────────────────────────────────────────────────────────────────── */

static void menu_fila(Contexto *ctx) {
    int op;
    char buf[MAX_TITULO];

    do {
        cabecalho("Fila de Reprodução");
        fila_listar(&ctx->fila_reprod);
        printf("\n");
        printf("  1. Adicionar música à fila\n");
        printf("  2. Tocar próxima da fila\n");
        printf("  3. Ver próxima sem tocar\n");
        printf("  0. Voltar\n\n");
        ler_linha("  Opção: ", buf, sizeof(buf));
        op = atoi(buf);

        switch (op) {
            case 1:
                ler_linha("  Título da música: ", buf, sizeof(buf));
                {
                    NodoMusica *m = biblioteca_buscar_titulo(&ctx->biblioteca, buf);
                    if (m == NULL) { printf("  Música não encontrada.\n"); pausar(); break; }
                    fila_enqueue(&ctx->fila_reprod, m);
                    printf("  Adicionada à fila.\n");
                }
                pausar();
                break;

            case 2: {
                NodoFila saida;
                if (!fila_dequeue(&ctx->fila_reprod, &saida)) { pausar(); break; }
                /* converte NodoFila → NodoMusica temporário para tocar */
                NodoMusica tmp;
                strncpy(tmp.titulo,  saida.titulo,  MAX_TITULO  - 1); tmp.titulo[MAX_TITULO-1]   = '\0';
                strncpy(tmp.artista, saida.artista, MAX_ARTISTA - 1); tmp.artista[MAX_ARTISTA-1] = '\0';
                strncpy(tmp.caminho, saida.caminho, MAX_CAMINHO - 1); tmp.caminho[MAX_CAMINHO-1] = '\0';
                tmp.duracao = saida.duracao;
                tmp.prox = tmp.ant = NULL;
                player_tocar(&tmp, &ctx->historico, &ctx->estado);
                break;
            }

            case 3: {
                NodoFila saida;
                if (fila_peek(&ctx->fila_reprod, &saida))
                    printf("  Próxima: %s — %s\n", saida.titulo, saida.artista);
                else
                    printf("  Fila vazia.\n");
                pausar();
                break;
            }
        }
    } while (op != 0);
}

/* ─────────────────────────────────────────────────────────────────────────
   MENU HISTÓRICO
   ───────────────────────────────────────────────────────────────────────── */

static void menu_historico(Contexto *ctx) {
    int op;
    char buf[8];

    do {
        cabecalho("Histórico de Reprodução");
        pilha_listar(&ctx->historico);
        printf("\n");
        printf("  1. Tocar música mais recente do histórico\n");
        printf("  2. Remover topo do histórico\n");
        printf("  0. Voltar\n\n");
        ler_linha("  Opção: ", buf, sizeof(buf));
        op = atoi(buf);

        switch (op) {
            case 1: {
                NodoPilha topo;
                if (!pilha_peek(&ctx->historico, &topo)) { pausar(); break; }
                NodoMusica tmp;
                strncpy(tmp.titulo,  topo.titulo,  MAX_TITULO  - 1); tmp.titulo[MAX_TITULO-1]   = '\0';
                strncpy(tmp.artista, topo.artista, MAX_ARTISTA - 1); tmp.artista[MAX_ARTISTA-1] = '\0';
                strncpy(tmp.caminho, topo.caminho, MAX_CAMINHO - 1); tmp.caminho[MAX_CAMINHO-1] = '\0';
                tmp.duracao = topo.duracao;
                tmp.prox = tmp.ant = NULL;
                /* tocar re-empurra no histórico — comportamento intencional */
                player_tocar(&tmp, &ctx->historico, &ctx->estado);
                break;
            }
            case 2: {
                NodoPilha topo;
                if (pilha_pop(&ctx->historico, &topo))
                    printf("  Removido: %s\n", topo.titulo);
                pausar();
                break;
            }
        }
    } while (op != 0);
}

/* ─────────────────────────────────────────────────────────────────────────
   MENU PRINCIPAL
   ───────────────────────────────────────────────────────────────────────── */

void menu_principal(Contexto *ctx) {
    int op;
    char buf[8];

    do {
        cabecalho("Menu Principal");

        /* mini status */
        if (ctx->estado.tocando)
            printf("  ▶ Tocando: %s — %s\n\n", ctx->estado.musica_atual, ctx->estado.artista_atual);
        else
            printf("  ■ Parado\n\n");

        printf("  1. Biblioteca\n");
        printf("  2. Playlists\n");
        printf("  3. Fila de Reprodução\n");
        printf("  4. Histórico\n");
        printf("  5. Tocar música da biblioteca\n");
        printf("  0. Sair\n\n");
        ler_linha("  Opção: ", buf, sizeof(buf));
        op = atoi(buf);

        switch (op) {
            case 1: menu_biblioteca(ctx); break;
            case 2: menu_playlists(ctx);  break;
            case 3: menu_fila(ctx);       break;
            case 4: menu_historico(ctx);  break;

            case 5: {
                char titulo[MAX_TITULO];
                cabecalho("Tocar Música");
                biblioteca_listar(&ctx->biblioteca);
                printf("\n");
                ler_linha("  Título: ", titulo, sizeof(titulo));
                NodoMusica *m = biblioteca_buscar_titulo(&ctx->biblioteca, titulo);
                if (m == NULL) { printf("  Música não encontrada.\n"); pausar(); break; }
                player_tocar(m, &ctx->historico, &ctx->estado);
                break;
            }

            case 0:
                printf("\n  Até logo!\n\n");
                break;

            default:
                printf("  Opção inválida.\n");
                pausar();
        }
    } while (op != 0);
}
