#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "menu.h"
#include "../biblioteca_func/biblioteca.h"
#include "../playlist_func/playlist.h"
#include "../pilha_func/pilha.h"
#include "../fila_func/fila.h"
#include "../player_func/player.h"
#include "../audio_func/audio.h"

/* ==========================================================================
 * Utilitarios internos
 * ========================================================================== */

/*
 * DIFICULDADE: buffer de entrada sujo apos scanf.
 *
 * O problema: scanf("%d", &val) consome o numero mas deixa o '\n' (e
 * qualquer outro caractere) no buffer do stdin. Na leitura seguinte, esse
 * '\n' residual e consumido imediatamente, fazendo fgets() ou getchar()
 * retornar uma string vazia sem esperar o usuario digitar nada.
 *
 * SOLUCAO: limpar_buffer() descarta todos os caracteres ate o proximo '\n'
 * ou EOF. Chamada apos cada scanf e dentro de aguardar_enter(), garante
 * que a proxima leitura comece sempre com o buffer vazio.
 *
 * Por que nao fflush(stdin)? Comportamento indefinido em POSIX — so
 * funciona de forma previsivel no Windows (MSVC). Aprendemos isso ao
 * pesquisar sobre o assunto e optamos pela solucao portavel com getchar().
 */
static void limpar_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* Le uma string com prompt; remove \n final; garante terminacao com '\0' */
static void ler_string(const char *prompt, char *buf, int tam)
{
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, tam, stdin))
    {
        int len = (int)strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';
    }
    else
    {
        buf[0] = '\0';
    }
}

/* Le um inteiro com prompt.
 * Retorna o valor lido, -1 em entrada invalida, ou 0 em EOF
 * (tratado como "Sair/Voltar" para desenrolar os menus sem travar). */
static int ler_inteiro(const char *prompt)
{
    printf("%s", prompt);
    fflush(stdout);
    int val;
    if (scanf("%d", &val) != 1)
    {
        if (feof(stdin))
            return 0; /* EOF — sai de todos os menus limpo */
        limpar_buffer();
        return -1;
    }
    limpar_buffer();
    return val;
}

/* Imprime separador visual */
static void cabecalho(const char *titulo)
{
    printf("\n");
    printf("  ================================================\n");
    printf("   %s\n", titulo);
    printf("  ================================================\n");
}

/* Pausa e espera o usuario pressionar Enter */
static void aguardar_enter(void)
{
    printf("\n  Pressione Enter para continuar...");
    fflush(stdout);
    limpar_buffer();
}

/* Selecao de musica da biblioteca por numero */
static NodoMusica *selecionar_musica(const ListaDupla *lib)
{
    biblioteca_listar(lib);
    if (!lib->inicio)
        return NULL;
    int n = ler_inteiro("  Numero da musica: ");
    if (n < 1 || n > lib->tam)
    {
        printf("  Numero invalido.\n");
        return NULL;
    }
    NodoMusica *m = lib->inicio;
    for (int i = 1; i < n; i++)
        m = m->prox;
    return m;
}

/* Selecao de playlist por numero */
static NodoPlaylist *selecionar_playlist_num(const ListaPlaylists *lp)
{
    playlist_listar(lp);
    if (!lp->inicio)
        return NULL;
    int n = ler_inteiro("  Numero da playlist: ");
    if (n < 1 || n > lp->tam)
    {
        printf("  Numero invalido.\n");
        return NULL;
    }
    NodoPlaylist *pl = lp->inicio;
    for (int i = 1; i < n; i++)
        pl = pl->prox;
    return pl;
}

/* Busca e exibe todas as ocorrencias (titulo ou artista) */
static void buscar_e_exibir(const ListaDupla *lib, const char *query, int por_titulo)
{
    int achou = 0;
    const NodoMusica *n = lib->inicio;
    while (n)
    {
        /* Comparacao case-insensitive via substring */
        const char *campo = por_titulo ? n->titulo : n->artista;
        /* Copia em minusculo para comparar */
        char campo_low[MAX_TITULO + MAX_ARTISTA];
        char query_low[MAX_TITULO + MAX_ARTISTA];
        int i;
        for (i = 0; campo[i] && i < (int)sizeof(campo_low) - 1; i++)
            campo_low[i] = (char)tolower((unsigned char)campo[i]);
        campo_low[i] = '\0';
        for (i = 0; query[i] && i < (int)sizeof(query_low) - 1; i++)
            query_low[i] = (char)tolower((unsigned char)query[i]);
        query_low[i] = '\0';

        if (strstr(campo_low, query_low))
        {
            if (!achou)
            {
                printf("  %-40s %-30s %s\n", "Titulo", "Artista", "Duracao");
                printf("  %-40s %-30s %s\n", "------", "-------", "-------");
            }
            int min = n->duracao / 60, seg = n->duracao % 60;
            printf("  %-40s %-30s %02d:%02d\n", n->titulo, n->artista, min, seg);
            achou = 1;
        }
        n = n->prox;
    }
    if (!achou)
        printf("  Nenhuma musica encontrada para \"%s\".\n", query);
}

/* Coleta campos da nova musica e verifica o arquivo de audio.
 * Retorna 1 em sucesso e preenche titulo, artista, caminho e duracao. */
static int coletar_dados_musica(char *titulo, char *artista, char *caminho, int *duracao)
{
    ler_string("  Titulo  : ", titulo, MAX_TITULO);
    ler_string("  Artista : ", artista, MAX_ARTISTA);
    ler_string("  Caminho : ", caminho, MAX_CAMINHO);
    if (titulo[0] == '\0' || caminho[0] == '\0')
    {
        printf("  Titulo e caminho sao obrigatorios.\n");
        return 0;
    }
    printf("  Verificando arquivo...\n");
    if (!audio_load(caminho))
    {
        printf("  Erro: arquivo nao encontrado ou formato invalido.\n");
        return 0;
    }
    *duracao = (int)audio_get_duration_seconds();
    audio_stop();
    return 1;
}

/* ==========================================================================
 * Menu: Biblioteca
 * ========================================================================== */
static void menu_biblioteca(ListaDupla *lib, ListaPlaylists *playlists, Pilha *historico, Fila *fila)
{
    int opcao;
    do
    {
        cabecalho("BIBLIOTECA");
        printf("  1. Listar musicas\n");
        printf("  2. Cadastrar musica (no fim)\n");
        printf("  3. Buscar por titulo\n");
        printf("  4. Buscar por artista\n");
        printf("  5. Remover musica\n");
        printf("  6. Inserir musica no inicio\n");
        printf("  7. Inserir musica em posicao\n");
        printf("  0. Voltar\n");
        opcao = ler_inteiro("  Opcao: ");

        switch (opcao)
        {

        case 1:
            cabecalho("LISTA DE MUSICAS");
            biblioteca_listar(lib);
            aguardar_enter();
            break;

        case 2:
        {
            cabecalho("CADASTRAR MUSICA");
            char titulo[MAX_TITULO], artista[MAX_ARTISTA], caminho[MAX_CAMINHO];
            ler_string("  Titulo  : ", titulo, MAX_TITULO);
            ler_string("  Artista : ", artista, MAX_ARTISTA);
            ler_string("  Caminho : ", caminho, MAX_CAMINHO);

            if (titulo[0] == '\0' || caminho[0] == '\0')
            {
                printf("  Titulo e caminho sao obrigatorios.\n");
                aguardar_enter();
                break;
            }

            /* Carrega o arquivo para obter a duracao */
            printf("  Verificando arquivo...\n");
            if (!audio_load(caminho))
            {
                printf("  Erro: arquivo nao encontrado ou formato invalido.\n");
                aguardar_enter();
                break;
            }
            int duracao = (int)audio_get_duration_seconds();
            audio_stop();

            NodoMusica *novo = biblioteca_cadastrar(lib, titulo, artista, caminho, duracao);
            if (novo)
            {
                int min = duracao / 60, seg = duracao % 60;
                printf("  Musica cadastrada! Duracao: %02d:%02d\n", min, seg);
            }
            aguardar_enter();
            break;
        }

        case 3:
        {
            cabecalho("BUSCAR POR TITULO");
            char query[MAX_TITULO];
            ler_string("  Titulo (parcial): ", query, MAX_TITULO);
            buscar_e_exibir(lib, query, 1);
            aguardar_enter();
            break;
        }

        case 4:
        {
            cabecalho("BUSCAR POR ARTISTA");
            char query[MAX_ARTISTA];
            ler_string("  Artista (parcial): ", query, MAX_ARTISTA);
            buscar_e_exibir(lib, query, 0);
            aguardar_enter();
            break;
        }

        case 5:
        {
            cabecalho("REMOVER MUSICA");
            NodoMusica *alvo = selecionar_musica(lib);
            if (!alvo)
            {
                aguardar_enter();
                break;
            }
            printf("  Remover \"%s\" de \"%s\"? (s/n): ", alvo->titulo, alvo->artista);
            char conf[4];
            ler_string("", conf, sizeof(conf));
            if (conf[0] == 's' || conf[0] == 'S')
            {
                /*
                 * Aqui e onde o callback e concretizado: passamos
                 * playlist_em_alguma (funcao de playlist_func) e o ponteiro
                 * para a lista de playlists. biblioteca_remover nao precisa
                 * saber nada sobre playlist_func — so chama o ponteiro de
                 * funcao recebido. Esse e o ponto de uniao que permite manter
                 * os dois modulos desacoplados (ver comentario em biblioteca.c).
                 */
                int ok = biblioteca_remover(lib, alvo, historico, fila,
                                            playlist_em_alguma, playlists);
                if (ok)
                    printf("  Musica removida.\n");
            }
            else
            {
                printf("  Operacao cancelada.\n");
            }
            aguardar_enter();
            break;
        }

        case 6:
        {
            cabecalho("INSERIR MUSICA NO INICIO");
            char titulo[MAX_TITULO], artista[MAX_ARTISTA], caminho[MAX_CAMINHO];
            int duracao;
            if (!coletar_dados_musica(titulo, artista, caminho, &duracao))
            {
                aguardar_enter();
                break;
            }
            NodoMusica *novo = biblioteca_inserir_inicio(lib, titulo, artista, caminho, duracao);
            if (novo)
            {
                int min = duracao / 60, seg = duracao % 60;
                printf("  Musica inserida no inicio! Duracao: %02d:%02d\n", min, seg);
            }
            aguardar_enter();
            break;
        }

        case 7:
        {
            cabecalho("INSERIR MUSICA EM POSICAO");
            if (lib->tam == 0)
            {
                printf("  Biblioteca vazia — use a opcao 2 para cadastrar a primeira musica.\n");
                aguardar_enter();
                break;
            }
            printf("  Biblioteca tem %d musica(s). Posicao valida: 1 a %d\n", lib->tam, lib->tam + 1);
            int pos = ler_inteiro("  Posicao desejada: ");
            if (pos < 1 || pos > lib->tam + 1)
            {
                printf("  Posicao invalida.\n");
                aguardar_enter();
                break;
            }
            char titulo[MAX_TITULO], artista[MAX_ARTISTA], caminho[MAX_CAMINHO];
            int duracao;
            if (!coletar_dados_musica(titulo, artista, caminho, &duracao))
            {
                aguardar_enter();
                break;
            }
            NodoMusica *novo = biblioteca_inserir_em_posicao(lib, pos, titulo, artista, caminho, duracao);
            if (novo)
            {
                int min = duracao / 60, seg = duracao % 60;
                printf("  Musica inserida na posicao %d! Duracao: %02d:%02d\n", pos, min, seg);
            }
            aguardar_enter();
            break;
        }

        case 0:
            break;
        default:
            printf("  Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* ==========================================================================
 * Menu: Gerenciar uma playlist especifica
 * ========================================================================== */
static void menu_playlist_interna(NodoPlaylist *pl,
                                  ListaDupla *biblioteca,
                                  EstadoPlayer *estado,
                                  Pilha *historico,
                                  Fila *fila)
{
    int opcao;
    char titulo_menu[MAX_NOME_PL + 16];
    snprintf(titulo_menu, sizeof(titulo_menu), "PLAYLIST: %s", pl->nome);

    do
    {
        cabecalho(titulo_menu);
        printf("  Musicas: %d\n\n", pl->tam_musicas);
        printf("  1. Listar musicas\n");
        printf("  2. Adicionar musica\n");
        printf("  3. Remover musica\n");
        printf("  4. Tocar playlist\n");
        printf("  0. Voltar\n");
        opcao = ler_inteiro("  Opcao: ");

        switch (opcao)
        {

        case 1:
            cabecalho(titulo_menu);
            playlist_listar_musicas(pl);
            aguardar_enter();
            break;

        case 2:
        {
            cabecalho("ADICIONAR MUSICA A PLAYLIST");
            NodoMusica *m = selecionar_musica(biblioteca);
            if (!m)
            {
                aguardar_enter();
                break;
            }
            if (playlist_adicionar_musica(pl, m))
                printf("  \"%s\" adicionada a playlist.\n", m->titulo);
            aguardar_enter();
            break;
        }

        case 3:
        {
            cabecalho("REMOVER MUSICA DA PLAYLIST");
            playlist_listar_musicas(pl);
            if (!pl->inicio_musicas)
            {
                aguardar_enter();
                break;
            }

            /* Selecao por numero dentro da playlist */
            int n = ler_inteiro("  Numero da musica: ");
            if (n < 1 || n > pl->tam_musicas)
            {
                printf("  Numero invalido.\n");
                aguardar_enter();
                break;
            }
            EntradaPlaylist *e = pl->inicio_musicas;
            for (int i = 1; i < n; i++)
                e = e->prox;
            if (playlist_remover_musica(pl, e->musica))
                printf("  Musica removida da playlist.\n");
            aguardar_enter();
            break;
        }

        case 4:
        {
            /* Toca a playlist a partir da primeira musica */
            NodoMusica *primeira = playlist_primeira(pl);
            if (!primeira)
            {
                printf("  Playlist vazia, nada a tocar.\n");
                aguardar_enter();
                break;
            }
            estado->playlist_atual = pl;
            if (player_tocar(estado, primeira, historico))
                player_loop(estado, historico, fila, biblioteca);
            estado->playlist_atual = NULL;
            estado->atual = NULL;
            break;
        }

        case 0:
            break;
        default:
            printf("  Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* ==========================================================================
 * Menu: Playlists
 * ========================================================================== */
static void menu_playlists(ListaPlaylists *playlists,
                           ListaDupla *biblioteca,
                           EstadoPlayer *estado,
                           Pilha *historico,
                           Fila *fila)
{
    int opcao;
    do
    {
        cabecalho("PLAYLISTS");
        printf("  1. Listar playlists\n");
        printf("  2. Criar playlist\n");
        printf("  3. Remover playlist\n");
        printf("  4. Gerenciar / tocar playlist\n");
        printf("  0. Voltar\n");
        opcao = ler_inteiro("  Opcao: ");

        switch (opcao)
        {

        case 1:
            cabecalho("LISTA DE PLAYLISTS");
            playlist_listar(playlists);
            aguardar_enter();
            break;

        case 2:
        {
            cabecalho("CRIAR PLAYLIST");
            char nome[MAX_NOME_PL];
            ler_string("  Nome da playlist: ", nome, MAX_NOME_PL);
            if (nome[0] == '\0')
            {
                printf("  Nome invalido.\n");
                aguardar_enter();
                break;
            }
            NodoPlaylist *nova = playlist_criar(playlists, nome);
            if (nova)
                printf("  Playlist \"%s\" criada.\n", nova->nome);
            aguardar_enter();
            break;
        }

        case 3:
        {
            cabecalho("REMOVER PLAYLIST");
            NodoPlaylist *pl = selecionar_playlist_num(playlists);
            if (!pl)
            {
                aguardar_enter();
                break;
            }
            printf("  Remover playlist \"%s\"? (s/n): ", pl->nome);
            char conf[4];
            ler_string("", conf, sizeof(conf));
            if (conf[0] == 's' || conf[0] == 'S')
            {
                if (playlist_remover(playlists, pl))
                    printf("  Playlist removida.\n");
            }
            else
            {
                printf("  Operacao cancelada.\n");
            }
            aguardar_enter();
            break;
        }

        case 4:
        {
            cabecalho("SELECIONAR PLAYLIST");
            NodoPlaylist *pl = selecionar_playlist_num(playlists);
            if (!pl)
            {
                aguardar_enter();
                break;
            }
            menu_playlist_interna(pl, biblioteca, estado, historico, fila);
            break;
        }

        case 0:
            break;
        default:
            printf("  Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* ==========================================================================
 * Menu: Historico
 * ========================================================================== */
static void menu_historico(Pilha *historico,
                           EstadoPlayer *estado,
                           Fila *fila,
                           ListaDupla *biblioteca)
{
    int opcao;
    do
    {
        cabecalho("HISTORICO");
        printf("  1. Ver historico\n");
        printf("  2. Tocar musica mais recente\n");
        printf("  3. Limpar historico\n");
        printf("  0. Voltar\n");
        opcao = ler_inteiro("  Opcao: ");

        switch (opcao)
        {

        case 1:
            cabecalho("HISTORICO DE REPRODUCAO");
            pilha_listar(historico);
            aguardar_enter();
            break;

        case 2:
        {
            NodoMusica *topo = pilha_peek(historico);
            if (!topo)
            {
                printf("  Historico vazio.\n");
                aguardar_enter();
                break;
            }
            printf("  Tocando: %s — %s\n", topo->titulo, topo->artista);
            estado->playlist_atual = NULL;
            if (player_tocar(estado, topo, historico))
                player_loop(estado, historico, fila, biblioteca);
            estado->atual = NULL;
            break;
        }

        case 3:
            pilha_liberar(historico);
            pilha_inicializar(historico);
            printf("  Historico limpo.\n");
            aguardar_enter();
            break;

        case 0:
            break;
        default:
            printf("  Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* ==========================================================================
 * Menu: Fila de reproducao
 * ========================================================================== */
static void menu_fila(Fila *fila,
                      ListaDupla *biblioteca,
                      EstadoPlayer *estado,
                      Pilha *historico)
{
    int opcao;
    do
    {
        cabecalho("FILA DE REPRODUCAO");
        printf("  Musicas na fila: %d\n\n", fila_tamanho(fila));
        printf("  1. Ver fila\n");
        printf("  2. Adicionar musica a fila\n");
        printf("  3. Tocar proxima da fila\n");
        printf("  4. Esvaziar fila\n");
        printf("  0. Voltar\n");
        opcao = ler_inteiro("  Opcao: ");

        switch (opcao)
        {

        case 1:
            cabecalho("FILA DE REPRODUCAO");
            fila_listar(fila);
            aguardar_enter();
            break;

        case 2:
        {
            cabecalho("ADICIONAR A FILA");
            NodoMusica *m = selecionar_musica(biblioteca);
            if (!m)
            {
                aguardar_enter();
                break;
            }
            if (fila_enqueue(fila, m))
                printf("  \"%s\" adicionada a fila (posicao %d).\n", m->titulo, fila_tamanho(fila));
            aguardar_enter();
            break;
        }

        case 3:
        {
            NodoMusica *m = fila_dequeue(fila);
            if (!m)
            {
                printf("  Fila vazia.\n");
                aguardar_enter();
                break;
            }
            printf("  Tocando da fila: %s — %s\n", m->titulo, m->artista);
            estado->playlist_atual = NULL;
            if (player_tocar(estado, m, historico))
                player_loop(estado, historico, fila, biblioteca);
            estado->atual = NULL;
            break;
        }

        case 4:
            fila_liberar(fila);
            fila_inicializar(fila);
            printf("  Fila esvaziada.\n");
            aguardar_enter();
            break;

        case 0:
            break;
        default:
            printf("  Opcao invalida.\n");
        }
    } while (opcao != 0);
}

/* ==========================================================================
 * Menu principal
 * ========================================================================== */
void menu_principal(ListaDupla *biblioteca,
                    ListaPlaylists *playlists,
                    Pilha *historico,
                    Fila *fila,
                    EstadoPlayer *estado)
{
    int opcao;
    do
    {
        cabecalho("PLAYER DE MUSICA");
        printf("Musicas: %-4d  Playlists: %-4d  Historico: %-4d  Fila: %-4d\n\n", biblioteca_tamanho(biblioteca), playlists->tam, pilha_tamanho(historico), fila_tamanho(fila));
        printf("  1. Biblioteca\n");
        printf("  2. Playlists\n");
        printf("  3. Tocar musica da biblioteca\n");
        printf("  4. Historico\n");
        printf("  5. Fila de reproducao\n");
        printf("  0. Sair\n");
        opcao = ler_inteiro("  Opcao: ");

        switch (opcao)
        {

        case 1:
            menu_biblioteca(biblioteca, playlists, historico, fila);
            break;

        case 2:
            menu_playlists(playlists, biblioteca, estado, historico, fila);
            break;

        case 3:
        {
            cabecalho("TOCAR MUSICA");
            NodoMusica *m = selecionar_musica(biblioteca);
            if (!m)
            {
                aguardar_enter();
                break;
            }
            printf("  Tocando: %s — %s\n", m->titulo, m->artista);
            estado->playlist_atual = NULL;
            if (player_tocar(estado, m, historico))
                player_loop(estado, historico, fila, biblioteca);
            estado->atual = NULL;
            break;
        }

        case 4:
            menu_historico(historico, estado, fila, biblioteca);
            break;

        case 5:
            menu_fila(fila, biblioteca, estado, historico);
            break;

        case 0:
            printf("\n  Ate logo!\n\n");
            break;

        default:
            printf("  Opcao invalida.\n");
        }
    } while (opcao != 0);
}
