#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "player.h"
#include "../audio_func/audio.h"
#include "../input_func/input.h"

/*
 * DIFICULDADE: pausa portavel sem usleep().
 *
 * Precisavamos de uma espera de 200 ms entre cada iteracao do loop para
 * atualizar a barra de status 5x por segundo sem queimar CPU.
 *
 * usleep() parece a escolha obvia no Linux, mas exige _BSD_SOURCE ou
 * _XOPEN_SOURCE >= 500, e nao existe no Windows. Para compilar os dois
 * sistemas sem ifdefs espalhados, criamos dormir_ms() baseada em
 * nanosleep() (POSIX.1b, disponivelcom -D_POSIX_C_SOURCE=200112L) no
 * Linux, e Sleep() da windows.h no Windows.
 *
 * APRENDIZADO: nanosleep() recebe struct timespec com segundos e
 * nanossegundos; convertemos ms corretamente dividindo e usando o resto.
 */
#ifdef _WIN32
#  include <windows.h>
#  define SLEEP_MS(ms) Sleep(ms)
#else
static void dormir_ms(unsigned int ms) {
    struct timespec ts;
    ts.tv_sec  = (time_t)(ms / 1000u);
    ts.tv_nsec = (long)((ms % 1000u) * 1000000L);
    nanosleep(&ts, NULL);
}
#  define SLEEP_MS(ms) dormir_ms(ms)
#endif

#define SEEK_SEGUNDOS 5.0f

/* Utilitarios internos */

/* Formata segundos como "MM:SS" no buffer fornecido (minimo 6 bytes) */
static void formatar_tempo(float seg, char *buf, int tam) {
    int s = (int)seg;
    snprintf(buf, (size_t)tam, "%02d:%02d", s / 60, s % 60);
}

/* Escolhe uma musica aleatoria da biblioteca diferente da atual */
static NodoMusica *shuffle_proximo(const ListaDupla *bib, const NodoMusica *atual) {
    if (!bib->inicio) return NULL;
    if (bib->tam == 1) return bib->inicio; /* so uma musica */

    /* Conta quantas opcoes existem alem da atual */
    int total = bib->tam - (atual ? 1 : 0);
    if (total <= 0) return bib->inicio;

    int alvo = rand() % total;
    int idx  = 0;
    NodoMusica *n = bib->inicio;
    while (n) {
        if (n != atual) {
            if (idx == alvo) return n;
            idx++;
        }
        n = n->prox;
    }
    return bib->inicio; /* fallback */
}

/* Escolhe uma musica aleatoria DENTRO DA PLAYLIST diferente da atual */
static NodoMusica *playlist_shuffle_proximo(const NodoPlaylist *pl, const NodoMusica *atual) {
    if (!pl->inicio_musicas) return NULL;
    if (pl->tam_musicas == 1) return pl->inicio_musicas->musica;

    int total = pl->tam_musicas - (atual ? 1 : 0);
    if (total <= 0) return pl->inicio_musicas->musica;

    int alvo = rand() % total;
    int idx  = 0;
    const EntradaPlaylist *e = pl->inicio_musicas;
    while (e) {
        if (e->musica != atual) {
            if (idx == alvo) return e->musica;
            idx++;
        }
        e = e->prox;
    }
    return pl->inicio_musicas->musica; /* fallback */
}

/* Retorna a proxima musica da biblioteca (ciclico) */
static NodoMusica *biblioteca_proxima(const ListaDupla *bib, const NodoMusica *atual) {
    if (!bib->inicio) return NULL;
    if (!atual || !atual->prox) return bib->inicio; /* reinicia no inicio */
    return atual->prox;
}

/* Retorna a musica anterior da biblioteca (ciclico) */
static NodoMusica *biblioteca_anterior(const ListaDupla *bib, const NodoMusica *atual) {
    if (!bib->inicio) return NULL;
    if (!atual || !atual->ant) return bib->fim; /* vai para o fim */
    return atual->ant;
}

/* Exibe a barra de status no terminal (sobrescreve a linha atual) */
static void exibir_status(const EstadoPlayer *estado) {
    if (!estado->atual) return;

    float pos = audio_get_position_seconds();
    float dur = audio_get_duration_seconds();
    char  s_pos[8], s_dur[8];
    formatar_tempo(pos, s_pos, sizeof(s_pos));
    formatar_tempo(dur, s_dur, sizeof(s_dur));

    const char *status   = audio_is_playing() ? ">> TOCANDO" : "|| PAUSADO";
    const char *shuffle  = estado->modo_shuffle ? " [SHUFFLE]" : "";
    const char *contexto = estado->playlist_atual ? estado->playlist_atual->nome : "Biblioteca";

    /* Barra de progresso simples (20 caracteres) */
    char barra[22];
    memset(barra, '-', 20);
    barra[20] = '\0';
    if (dur > 0.0f) {
        int preenchido = (int)(pos / dur * 20.0f);
        if (preenchido > 20) preenchido = 20;
        for (int i = 0; i < preenchido; i++) barra[i] = '=';
    }

    /*
     * \r (carriage return) move o cursor para o inicio da linha atual sem
     * descer para a proxima — o printf seguinte sobrescreve o conteudo
     * anterior. fflush() e obrigatorio aqui: sem ele, o buffer do stdout
     * pode segurar os caracteres e a barra nao atualiza em tempo real.
     */
    printf("\r  %s%s | [%s] %s/%s | %s: %-30.30s  "
           "  k=pause j=+5s l=-5s p=prox o=ant s=shuf q=sair  ",
           status, shuffle,
           barra, s_pos, s_dur,
           contexto, estado->atual->titulo);
    fflush(stdout);
}

/* API publica */

int player_tocar(EstadoPlayer *estado,
                 NodoMusica   *musica,
                 Pilha        *historico) {
    if (!musica) return 0;

    /* Carrega o arquivo de audio */
    if (!audio_load(musica->caminho)) {
        printf("\n  Erro: nao foi possivel carregar '%s'.\n", musica->caminho);
        return 0;
    }

    estado->atual = musica;
    audio_play();

    /* Empilha no historico (evita duplicata consecutiva no topo) */
    if (!pilha_vazia(historico) && pilha_peek(historico) == musica) {
        /* mesma musica: nao duplica */
    } else {
        pilha_push(historico, musica);
    }

    return 1;
}

void player_loop(EstadoPlayer  *estado, Pilha *historico, Fila *fila, ListaDupla *biblioteca) {
    if (!estado->atual) return;

    input_modo_raw();
    printf("\n"); /* linha em branco antes da barra de status */

    while (1) {
        /* Avanco automatico: chegou ao fim? */
        if (audio_is_at_end()) {
            NodoMusica *proxima = NULL;

            /*
             * Ordem de prioridade para escolha da proxima musica:
             *   1. Fila de reproducao (FIFO explicita do usuario)
             *   2. Playlist atual (sequencial ou shuffle dentro dela)
             *   3. Biblioteca geral (sequencial ou shuffle global)
             * Essa hierarquia garante que musicas enfileiradas manualmente
             * sempre tocam antes da progressao automatica da playlist ou da
             * biblioteca — comportamento esperado num player real.
             */
            if (!fila_vazia(fila)) {
                proxima = fila_dequeue(fila);
            } else if (estado->playlist_atual) {
                if (estado->modo_shuffle)
                    proxima = playlist_shuffle_proximo(estado->playlist_atual, estado->atual);
                else
                    proxima = playlist_proxima(estado->playlist_atual, estado->atual);
            } else {
                if (estado->modo_shuffle)
                    proxima = shuffle_proximo(biblioteca, estado->atual);
                else
                    proxima = biblioteca_proxima(biblioteca, estado->atual);
            }

            if (proxima) {
                player_tocar(estado, proxima, historico);
            } else {
                printf("\n  Fim da fila de reproducao.\n");
                break;
            }
        }

        /* Leitura de tecla sem bloqueio */
        if (input_tecla_disponivel()) {
            int tecla = input_ler_tecla();

            switch (tecla) {

            case 'k': /* pausar / retomar */
                if (audio_is_playing()) audio_pause();
                else                   audio_resume();
                break;

            case 'j': /* avancar 5s */
                audio_seek_relative(SEEK_SEGUNDOS);
                break;

            case 'l': /* voltar 5s */
                audio_seek_relative(-SEEK_SEGUNDOS);
                break;

            case 'p': /* proxima musica */
            {
                NodoMusica *proxima = NULL;
                if (!fila_vazia(fila)) {
                    proxima = fila_dequeue(fila);
                } else if (estado->playlist_atual) {
                    proxima = estado->modo_shuffle
                        ? playlist_shuffle_proximo(estado->playlist_atual, estado->atual)
                        : playlist_proxima(estado->playlist_atual, estado->atual);
                } else {
                    proxima = estado->modo_shuffle
                        ? shuffle_proximo(biblioteca, estado->atual)
                        : biblioteca_proxima(biblioteca, estado->atual);
                }
                if (proxima) player_tocar(estado, proxima, historico);
                break;
            }

            case 'o': /* musica anterior */
            {
                NodoMusica *anterior = NULL;
                if (estado->playlist_atual) {
                    anterior = playlist_anterior(estado->playlist_atual, estado->atual);
                } else {
                    anterior = biblioteca_anterior(biblioteca, estado->atual);
                }
                if (anterior) player_tocar(estado, anterior, historico);
                break;
            }

            case 's': /* toggle shuffle */
                estado->modo_shuffle = !estado->modo_shuffle;
                break;

            case 'q': /* voltar ao menu */
                audio_stop();
                estado->atual = NULL;
                printf("\n");
                input_modo_normal();
                return;

            default:
                break;
            }
        }

        exibir_status(estado);
        SLEEP_MS(200); /* atualiza 5x por segundo */
    }

    /* Saida por fim natural da reproducao */
    audio_stop();
    estado->atual = NULL;
    printf("\n");
    input_modo_normal();
}
