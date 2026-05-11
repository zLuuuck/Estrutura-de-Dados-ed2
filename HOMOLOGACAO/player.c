#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include "player.h"
#include "pilha.h"

/* ── estrutura passada para a thread de progresso ────────────────────────── */
typedef struct {
    int   duracao;        /* total em segundos          */
    int   *parar;         /* flag compartilhada (0/1)   */
    EstadoPlayer *estado; /* estado global              */
} ArgsProgresso;

/* ── pid do processo ffplay (global neste módulo) ────────────────────────── */
static pid_t pid_ffplay = -1;

/* ─────────────────────────────────────────────────────────────────────────
   player_obter_duracao
   Chama: ffprobe -v error -show_entries format=duration
                  -of default=noprint_wrappers=1:nokey=1 <arquivo>
   ───────────────────────────────────────────────────────────────────────── */
int player_obter_duracao(const char *caminho) {
    char cmd[MAX_CAMINHO + 128];
    snprintf(cmd, sizeof(cmd),
             "ffprobe -v error -show_entries format=duration "
             "-of default=noprint_wrappers=1:nokey=1 \"%s\" 2>/dev/null",
             caminho);

    FILE *fp = popen(cmd, "r");
    if (fp == NULL) return 0;

    double seg = 0.0;
    if (fscanf(fp, "%lf", &seg) != 1) seg = 0.0;
    pclose(fp);

    return (int) seg;
}

/* ─────────────────────────────────────────────────────────────────────────
   player_exibir_barra
   Formato:  [=========>          ] 45% 02:15 / 05:00
   ───────────────────────────────────────────────────────────────────────── */
void player_exibir_barra(int atual, int total, int largura) {
    if (total <= 0) total = 1;
    if (atual > total) atual = total;

    int preenchido = (atual * largura) / total;
    int percentual = (atual * 100) / total;

    int min_a = atual / 60, seg_a = atual % 60;
    int min_t = total / 60, seg_t = total % 60;

    printf("\r  [");
    for (int i = 0; i < largura; i++) {
        if (i < preenchido)          putchar('=');
        else if (i == preenchido)    putchar('>');
        else                         putchar(' ');
    }
    printf("] %3d%%  %02d:%02d / %02d:%02d   ",
           percentual, min_a, seg_a, min_t, seg_t);
    fflush(stdout);
}

/* ─────────────────────────────────────────────────────────────────────────
   Thread de progresso — roda em paralelo enquanto ffplay toca
   ───────────────────────────────────────────────────────────────────────── */
static void *thread_progresso(void *arg) {
    ArgsProgresso *a = (ArgsProgresso *) arg;

    for (int t = 0; t <= a->duracao; t++) {
        if (*(a->parar)) break;

        player_exibir_barra(t, a->duracao, BARRA_LARGURA);
        a->estado->segundos_passados = t;

        sleep(1);
    }
    /* garante barra em 100% ao terminar naturalmente */
    if (!(*(a->parar)))
        player_exibir_barra(a->duracao, a->duracao, BARRA_LARGURA);

    return NULL;
}

/* ─────────────────────────────────────────────────────────────────────────
   player_tocar
   ───────────────────────────────────────────────────────────────────────── */
void player_tocar(const NodoMusica *musica, Pilha *historico, EstadoPlayer *estado) {
    if (musica == NULL) { printf("Nenhuma música selecionada.\n"); return; }

    /* ── atualiza estado ─────────────────────────────────────────────────── */
    strncpy(estado->musica_atual,  musica->titulo,  MAX_TITULO  - 1);
    strncpy(estado->artista_atual, musica->artista, MAX_ARTISTA - 1);
    estado->duracao_atual     = musica->duracao;
    estado->segundos_passados = 0;
    estado->tocando           = 1;
    estado->pausado           = 0;

    /* ── empurra no histórico ────────────────────────────────────────────── */
    pilha_push(historico, musica);

    /* ── exibe info ──────────────────────────────────────────────────────── */
    printf("\n");
    printf("  ♪ Tocando: %s — %s\n", musica->titulo, musica->artista);
    printf("  Duração : %02d:%02d\n\n", musica->duracao / 60, musica->duracao % 60);
    printf("  Pressione ENTER para parar.\n\n");

    /* ── fork: processo filho executa ffplay ─────────────────────────────── */
    int parar_thread = 0;
    pid_ffplay = fork();

    if (pid_ffplay < 0) {
        perror("fork");
        estado->tocando = 0;
        return;
    }

    if (pid_ffplay == 0) {
        /* filho: redireciona stdout/stderr para /dev/null para não poluir terminal */
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        execlp("ffplay", "ffplay", "-nodisp", "-autoexit",
               "-loglevel", "quiet", musica->caminho, (char *) NULL);
        /* se chegar aqui, exec falhou */
        exit(1);
    }

    /* ── pai: inicia thread de progresso ─────────────────────────────────── */
    ArgsProgresso args = {
        .duracao = musica->duracao,
        .parar   = &parar_thread,
        .estado  = estado
    };

    pthread_t tid;
    pthread_create(&tid, NULL, thread_progresso, &args);

    /* ── aguarda ENTER ou fim natural do ffplay ───────────────────────────── */
    /* Usa select para aguardar stdin sem bloquear completamente */
    fd_set fds;
    struct timeval tv;
    int status;
    int terminado = 0;

    while (!terminado) {
        /* verifica se o filho (ffplay) já terminou */
        pid_t resultado = waitpid(pid_ffplay, &status, WNOHANG);
        if (resultado == pid_ffplay) {
            terminado = 1;
            break;
        }

        /* verifica se o usuário pressionou ENTER */
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        tv.tv_sec  = 0;
        tv.tv_usec = 200000; /* 200 ms */

        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            /* consumir o Enter */
            char c;
            if (read(STDIN_FILENO, &c, 1) >= 0) {
                /* para o ffplay */
                kill(pid_ffplay, SIGTERM);
                waitpid(pid_ffplay, &status, 0);
                terminado = 1;
            }
        }
    }

    /* ── para a thread de progresso ──────────────────────────────────────── */
    parar_thread = 1;
    pthread_join(tid, NULL);

    pid_ffplay = -1;
    estado->tocando = 0;

    printf("\n\n  ── Reprodução encerrada ──\n\n");
}
