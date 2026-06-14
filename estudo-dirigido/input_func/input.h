#ifndef INPUT_H
#define INPUT_H

/*
 * Coloca o terminal em modo nao-canonico (raw) sem eco.
 * No Windows e no-op (conio.h ja funciona sem configuracao).
 * Deve ser chamada antes do loop de reproducao ao vivo.
 */
void input_modo_raw(void);

/*
 * Restaura o terminal ao modo original.
 * Deve ser chamada ao sair do loop de reproducao.
 * Registrada automaticamente via atexit() por input_modo_raw().
 */
void input_modo_normal(void);

/*
 * Retorna 1 se houver uma tecla disponivel para leitura, 0 caso contrario.
 * Nao bloqueia.
 */
int input_tecla_disponivel(void);

/*
 * Le e retorna o caractere disponivel.
 * Deve ser chamada somente apos input_tecla_disponivel() retornar 1.
 * Nao bloqueia, nao exibe eco.
 */
int input_ler_tecla(void);

#endif /* INPUT_H */
