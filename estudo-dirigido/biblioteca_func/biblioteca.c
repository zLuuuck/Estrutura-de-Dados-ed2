#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "biblioteca.h"

/*
 * Busca substring case-insensitive.
 *
 * APRENDIZADO: tolower() e definido para unsigned char; passar um char
 * diretamente e comportamento indefinido em plataformas onde char e signed
 * e o valor for negativo (ex: caracteres acentuados em Latin-1). Por isso
 * fazemos o cast para (unsigned char) antes de chamar tolower().
 * Copiamos para buffers temporarios para nao modificar as strings originais.
 */
static int contem_ignorando_case(const char *texto, const char *padrao)
{
    if (!texto || !padrao)
    {
        return 0;
    }

    /* copia em minusculo para nao alterar os originais */
    char t[MAX_TITULO + MAX_ARTISTA];
    char p[MAX_TITULO + MAX_ARTISTA];
    int i;

    for (i = 0; texto[i] && i < (int)sizeof(t) - 1; i++)
    {
        t[i] = (char)tolower((unsigned char)texto[i]);
    }
    t[i] = '\0';

    for (i = 0; padrao[i] && i < (int)sizeof(p) - 1; i++)
    {
        p[i] = (char)tolower((unsigned char)padrao[i]);
    }
    p[i] = '\0';

    return strstr(t, p) != NULL;
}

/* Inicializacao */
void biblioteca_inicializar(ListaDupla *lib)
{
    lib->inicio = NULL;
    lib->fim = NULL;
    lib->tam = 0;
}

/* Cadastro */
NodoMusica *biblioteca_cadastrar(ListaDupla *lib, const char *titulo, const char *artista, const char *caminho, int duracao)
{
    NodoMusica *novo = (NodoMusica *)malloc(sizeof(NodoMusica));
    if (!novo)
    {
        fprintf(stderr, "Erro: sem memoria para cadastrar musica.\n");
        return NULL;
    }

    strncpy(novo->titulo, titulo, MAX_TITULO - 1);
    novo->titulo[MAX_TITULO - 1] = '\0';
    strncpy(novo->artista, artista, MAX_ARTISTA - 1);
    novo->artista[MAX_ARTISTA - 1] = '\0';
    strncpy(novo->caminho, caminho, MAX_CAMINHO - 1);
    novo->caminho[MAX_CAMINHO - 1] = '\0';
    novo->duracao = duracao;
    novo->prox = NULL;
    novo->ant = NULL;

    /* Insere no fim da lista */
    if (!lib->inicio)
    {
        lib->inicio = novo;
        lib->fim = novo;
    }
    else
    {
        novo->ant = lib->fim;
        lib->fim->prox = novo;
        lib->fim = novo;
    }
    lib->tam++;
    return novo;
}

/*
 * DIFICULDADE: dependencia circular entre modulos (ajuda de IA).
 *
 * biblioteca_remover precisa saber se uma musica esta em alguma playlist
 * antes de libera-la. O problema: se biblioteca.c incluisse playlist.h
 * diretamente, e playlist.h ja inclui tipos.h (que inclui NodoMusica),
 * formaríamos um ciclo de includes dificil de resolver.
 *
 * SOLUCAO (ideia sugerida pela IA): passar a funcao de verificacao como
 * callback (ponteiro de funcao). Quem chama biblioteca_remover — no caso
 * o menu — fornece a funcao concreta (playlist_em_alguma) e o ponteiro
 * para a lista de playlists. Assim biblioteca.c nao depende de playlist.h
 * e os modulos ficam desacoplados.
 *
 * APRENDIZADO: o padrao callback em C (ponteiro de funcao + void* contexto)
 * e a forma classica de quebrar dependencias circulares sem usar linguagens
 * orientadas a objeto. O void* permite passar qualquer estrutura sem que
 * o modulo chamado precise conhecer o tipo concreto.
 *
 * GERENCIAMENTO DE MEMORIA: a biblioteca e a DONA dos NodoMusica. Pilha,
 * fila e playlists guardam apenas ponteiros para eles. Por isso verificamos
 * todos os referencias antes de liberar: soltar o nodo com ponteiros ativos
 * em outras estruturas causaria ponteiro invalido (uso apos free).
 */
int biblioteca_remover(ListaDupla *lib, NodoMusica *alvo, const Pilha *historico, const Fila *fila, int (*em_alguma_playlist)(const void *lp, const NodoMusica *m), const void *lista_playlists)
{
    if (!alvo)
    {
        return 0;
    }

    /* Verifica se esta no historico */
    if (historico && pilha_contem(historico, alvo))
    {
        printf("  Nao e possivel remover: musica esta no historico de reproducao.\n");
        return 0;
    }

    /* Verifica se esta na fila de reproducao */
    if (fila && fila_contem(fila, alvo))
    {
        printf("  Nao e possivel remover: musica esta na fila de reproducao.\n");
        return 0;
    }

    /* Verifica se esta em alguma playlist via callback — ve comentario acima */
    if (em_alguma_playlist && lista_playlists && em_alguma_playlist(lista_playlists, alvo))
    {
        printf("  Nao e possivel remover: musica esta em uma ou mais playlists.\n");
        return 0;
    }

    /* Desencadeia da lista dupla */
    if (alvo->ant)
    {
        alvo->ant->prox = alvo->prox;
    }
    else
    {
        lib->inicio = alvo->prox; /* era o primeiro */
    }

    if (alvo->prox)
    {
        alvo->prox->ant = alvo->ant;
    }
    else
    {
        lib->fim = alvo->ant; /* era o ultimo */
    }

    free(alvo);
    lib->tam--;
    return 1;
}

/* Insercao no inicio */
NodoMusica *biblioteca_inserir_inicio(ListaDupla *lib, const char *titulo, const char *artista, const char *caminho, int duracao)
{
    NodoMusica *novo = (NodoMusica *)malloc(sizeof(NodoMusica));
    if (!novo)
    {
        fprintf(stderr, "Erro: sem memoria para inserir musica.\n");
        return NULL;
    }

    strncpy(novo->titulo, titulo, MAX_TITULO - 1);
    novo->titulo[MAX_TITULO - 1] = '\0';
    strncpy(novo->artista, artista, MAX_ARTISTA - 1);
    novo->artista[MAX_ARTISTA - 1] = '\0';
    strncpy(novo->caminho, caminho, MAX_CAMINHO - 1);
    novo->caminho[MAX_CAMINHO - 1] = '\0';
    novo->duracao = duracao;
    novo->prox = lib->inicio;
    novo->ant = NULL;

    if (!lib->inicio)
    {
        lib->inicio = novo;
        lib->fim = novo;
    }
    else
    {
        lib->inicio->ant = novo;
        lib->inicio = novo;
    }
    lib->tam++;
    return novo;
}

/* Insercao em posicao arbitraria (1 = inicio, tam+1 = fim) */
NodoMusica *biblioteca_inserir_em_posicao(ListaDupla *lib, int pos, const char *titulo, const char *artista, const char *caminho, int duracao)
{
    if (pos <= 1)
    {
        return biblioteca_inserir_inicio(lib, titulo, artista, caminho, duracao);
    }
    if (pos > lib->tam)
    {
        return biblioteca_cadastrar(lib, titulo, artista, caminho, duracao);
    }

    NodoMusica *novo = (NodoMusica *)malloc(sizeof(NodoMusica));
    if (!novo)
    {
        fprintf(stderr, "Erro: sem memoria para inserir musica.\n");
        return NULL;
    }

    strncpy(novo->titulo, titulo, MAX_TITULO - 1);
    novo->titulo[MAX_TITULO - 1] = '\0';
    strncpy(novo->artista, artista, MAX_ARTISTA - 1);
    novo->artista[MAX_ARTISTA - 1] = '\0';
    strncpy(novo->caminho, caminho, MAX_CAMINHO - 1);
    novo->caminho[MAX_CAMINHO - 1] = '\0';
    novo->duracao = duracao;

    /* Navega ate o no que ficara imediatamente antes do novo */
    NodoMusica *anterior = lib->inicio;
    for (int i = 1; i < pos - 1; i++)
    {
        anterior = anterior->prox;
    }

    NodoMusica *proximo = anterior->prox;
    novo->ant = anterior;
    novo->prox = proximo;
    anterior->prox = novo;
    if (proximo)
    {
        proximo->ant = novo;
    }
    else
    {
        lib->fim = novo;
    }

    lib->tam++;
    return novo;
}

/* Busca por titulo */
NodoMusica *biblioteca_buscar_titulo(const ListaDupla *lib, const char *titulo)
{
    NodoMusica *atual = lib->inicio;
    while (atual)
    {
        if (contem_ignorando_case(atual->titulo, titulo))
        {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

/* Busca por artista */
NodoMusica *biblioteca_buscar_artista(const ListaDupla *lib, const char *artista)
{
    NodoMusica *atual = lib->inicio;
    while (atual)
    {
        if (contem_ignorando_case(atual->artista, artista))
        {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

/* Listagem */
void biblioteca_listar(const ListaDupla *lib)
{
    if (!lib->inicio)
    {
        printf("  Biblioteca vazia.\n");
        return;
    }
    const NodoMusica *atual = lib->inicio;
    int num = 1;
    printf("  %-4s %-40s %-30s %s\n", "Num", "Titulo", "Artista", "Duracao");
    printf("  %-4s %-40s %-30s %s\n", "---", "------", "-------", "-------");
    while (atual)
    {
        int min = atual->duracao / 60;
        int seg = atual->duracao % 60;
        printf("  %-4d %-40s %-30s %02d:%02d\n", num, atual->titulo, atual->artista, min, seg);
        atual = atual->prox;
        num++;
    }
}

/* Tamanho */
int biblioteca_tamanho(const ListaDupla *lib)
{
    return lib->tam;
}

/* Liberacao */
void biblioteca_liberar(ListaDupla *lib)
{
    NodoMusica *atual = lib->inicio;
    while (atual)
    {
        NodoMusica *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    lib->inicio = NULL;
    lib->fim = NULL;
    lib->tam = 0;
}
