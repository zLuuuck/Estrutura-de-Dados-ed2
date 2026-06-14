#include <stdio.h>
#include <stdlib.h>
#include "input.h"

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

/* Restaura o terminal — chamada pelo atexit() */
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
        return; /* ja configurado */

    /* Salva estado original */
    tcgetattr(STDIN_FILENO, &termios_original);

    /* Registra cleanup automatico ao encerrar o programa */
    atexit(input_modo_normal);

    /* Configura modo raw: sem canonical, sem eco, leitura imediata */
    struct termios raw = termios_original;
    raw.c_lflag &= (tcflag_t) ~(ICANON | ECHO); /* desliga canonical e eco  */
    raw.c_cc[VMIN] = 0;                         /* retorna sem esperar byte */
    raw.c_cc[VTIME] = 0;                        /* timeout zero             */

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    modo_raw_ativo = 1;
}

int input_tecla_disponivel(void)
{
    /* Verifica se ha dado disponivel em stdin sem bloquear */
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
        return -1;
    return (int)c;
}

#endif /* _WIN32 */
