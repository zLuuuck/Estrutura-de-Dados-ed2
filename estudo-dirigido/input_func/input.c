#include <stdio.h>
#include <stdlib.h>
#include "input.h"

/*
 * DIFICULDADE: teclado nao bloqueante multiplataforma.
 *
 * O problema: o loop de reproducao precisa ler teclas sem travar a execucao
 * (sem esperar o usuario pressionar Enter). Os dois sistemas operacionais
 * resolvem isso de formas completamente diferentes:
 *
 *   Windows: a <conio.h> ja fornece _kbhit() (verifica se ha tecla) e
 *            _getch() (le sem eco e sem Enter), sem nenhuma configuracao.
 *
 *   Linux: o terminal opera em modo canonico por padrao — os caracteres
 *          ficam no buffer ate o Enter ser pressionado. Para ler tecla
 *          por tecla, precisamos colocar o terminal em modo raw com
 *          tcsetattr (POSIX), desligando ICANON e ECHO. Depois usamos
 *          select() com timeout zero para checar se ha dado disponivel
 *          sem bloquear.
 *
 * AJUDA DE IA: a API do tcsetattr e os campos da struct termios nao sao
 * intuitivos. Consultamos a IA para entender quais flags desligar (ICANON,
 * ECHO), o que sao VMIN e VTIME, e como usar select() com timeout {0,0}.
 *
 * APRENDIZADO: select() e uma chamada POSIX que monitora descritores de
 * arquivo com timeout configuravel. Com tv = {0, 0} ele retorna
 * imediatamente, indicando so se ha dado disponivel — ideal para polling
 * dentro de um loop que nao pode bloquear.
 */

/* ==========================================================================
 * Windows — usa <conio.h> nativamente; nenhuma configuracao necessaria.
 * Linux/macOS — usa <termios.h> para colocar o terminal em modo raw.
 * ========================================================================== */

#ifdef _WIN32
/* Implementacao Windows */
#include <conio.h>

void input_modo_raw(void) { /* no-op: conio.h nao precisa de setup */ }
void input_modo_normal(void) { /* no-op */ }

int input_tecla_disponivel(void)
{
    return _kbhit();
}

int input_ler_tecla(void)
{
    return _getch();
}

#else
/* Implementacao Linux/macOS */
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>

/* Salva as configuracoes originais do terminal para restaurar depois */
static struct termios termios_original;
static int modo_raw_ativo = 0;

/*
 * DIFICULDADE: restauracao segura do terminal.
 * Se o programa fechar de forma abrupta (Ctrl+C, exit() direto, sinal)
 * com o terminal em modo raw, o usuario fica com o terminal inutilizavel:
 * sem eco, sem Enter funcionando. Registrar esta funcao via atexit() garante
 * que ela sera chamada em qualquer caminho de saida do processo.
 */
void input_modo_normal(void)
{
    if (modo_raw_ativo)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &termios_original);
        modo_raw_ativo = 0;
    }
}

void input_modo_raw(void)
{
    if (modo_raw_ativo)
    {
        return; /* ja configurado */
    }

    /* Salva estado original para restaurar depois */
    tcgetattr(STDIN_FILENO, &termios_original);

    /*
     * Registra o cleanup automatico: se o programa sair por qualquer
     * motivo (return do main, exit(), sinal tratado), o terminal volta
     * ao modo normal. Aprendemos isso apos perceber que sair com Ctrl+C
     * deixava o terminal quebrado na sessao inteira.
     */
    atexit(input_modo_normal);

    /*
     * Modo raw: desliga modo canonico (ICANON) e eco (ECHO).
     * ICANON: sem ele, read() retorna cada byte imediatamente, sem
     *         esperar o Enter.
     * ECHO:   sem ele, as teclas pressionadas nao aparecem na tela
     *         (necessario para nao poluir a barra de status).
     * VMIN=0, VTIME=0: read() retorna imediatamente, mesmo sem dados.
     */
    struct termios raw = termios_original;
    raw.c_lflag &= (tcflag_t) ~(ICANON | ECHO); /* desliga canonical e eco  */
    raw.c_cc[VMIN] = 0;                         /* retorna sem esperar byte */
    raw.c_cc[VTIME] = 0;                        /* timeout zero             */

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    modo_raw_ativo = 1;
}

int input_tecla_disponivel(void)
{
    /*
     * select() com timeout {0,0} verifica se ha dado legivel em stdin
     * sem bloquear: retorna > 0 se ha, 0 se nao ha, -1 em erro.
     * Esse e o padrao POSIX para polling nao bloqueante de I/O.
     */
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

int input_ler_tecla(void)
{
    unsigned char c = 0;
    if (read(STDIN_FILENO, &c, 1) != 1)
    {
        return -1;
    }
    return (int)c;
}

#endif /* _WIN32 */
