#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playlist.h"

/*  Utilitarios internos  */

/* Busca a EntradaPlaylist que aponta para musica; retorna NULL se nao achou */
static EntradaPlaylist *entrada_buscar(const NodoPlaylist *pl, const NodoMusica *musica)
{
    EntradaPlaylist *e = pl->inicio_musicas;
    while (e)
    {
        if (e->musica == musica)
        {
            return e;
        }
        e = e->prox;
    }
    return NULL;
}

/* Remove e libera um EntradaPlaylist especifico da lista interna */
static void entrada_desencadear(NodoPlaylist *pl, EntradaPlaylist *e)
{
    if (e->ant)
    {
        e->ant->prox = e->prox;
    }
    else
    {
        pl->inicio_musicas = e->prox;
    }

    if (e->prox)
    {
        e->prox->ant = e->ant;
    }
    else
    {
        pl->fim_musicas = e->ant;
    }

    free(e);
    pl->tam_musicas--;
}

/* Libera todos os EntradaPlaylist de uma playlist sem tocar nos NodoMusica */
static void entradas_liberar(NodoPlaylist *pl)
{
    EntradaPlaylist *e = pl->inicio_musicas;
    while (e)
    {
        EntradaPlaylist *prox = e->prox;
        free(e);
        e = prox;
    }
    pl->inicio_musicas = NULL;
    pl->fim_musicas = NULL;
    pl->tam_musicas = 0;
}

/*  Lista de playlists  */

void playlist_lista_inicializar(ListaPlaylists *lp)
{
    lp->inicio = NULL;
    lp->fim = NULL;
    lp->tam = 0;
}

NodoPlaylist *playlist_criar(ListaPlaylists *lp, const char *nome)
{
    /* Impede nomes duplicados */
    if (playlist_buscar(lp, nome))
    {
        printf("  Ja existe uma playlist com esse nome.\n");
        return NULL;
    }

    NodoPlaylist *novo = (NodoPlaylist *)malloc(sizeof(NodoPlaylist));
    if (!novo)
    {
        fprintf(stderr, "Erro: sem memoria para criar playlist.\n");
        return NULL;
    }

    strncpy(novo->nome, nome, MAX_NOME_PL - 1);
    novo->nome[MAX_NOME_PL - 1] = '\0';
    novo->inicio_musicas = NULL;
    novo->fim_musicas = NULL;
    novo->tam_musicas = 0;
    novo->prox = NULL;
    novo->ant = NULL;

    /* Insere no fim da lista de playlists */
    if (!lp->inicio)
    {
        lp->inicio = novo;
        lp->fim = novo;
    }
    else
    {
        novo->ant = lp->fim;
        lp->fim->prox = novo;
        lp->fim = novo;
    }
    lp->tam++;
    return novo;
}

int playlist_remover(ListaPlaylists *lp, NodoPlaylist *alvo)
{
    if (!alvo)
    {
        return 0;
    }

    /* Desencadeia da lista de playlists */
    if (alvo->ant)
    {
        alvo->ant->prox = alvo->prox;
    }
    else
    {
        lp->inicio = alvo->prox;
    }

    if (alvo->prox)
    {
        alvo->prox->ant = alvo->ant;
    }
    else
    {
        lp->fim = alvo->ant;
    }

    /* Libera entradas internas e o proprio NodoPlaylist */
    entradas_liberar(alvo);
    free(alvo);
    lp->tam--;
    return 1;
}

NodoPlaylist *playlist_buscar(const ListaPlaylists *lp, const char *nome)
{
    NodoPlaylist *atual = lp->inicio;
    while (atual)
    {
        if (strcmp(atual->nome, nome) == 0)
        {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

void playlist_listar(const ListaPlaylists *lp)
{
    if (!lp->inicio)
    {
        printf("  Nenhuma playlist cadastrada.\n");
        return;
    }
    NodoPlaylist *atual = lp->inicio;
    int num = 1;
    printf("  %-4s %-40s %s\n", "Num", "Nome", "Musicas");
    printf("  %-4s %-40s %s\n", "---", "----", "-------");
    while (atual)
    {
        printf("  %-4d %-40s %d\n", num, atual->nome, atual->tam_musicas);
        atual = atual->prox;
        num++;
    }
}

/*  Musicas dentro de uma playlist  */

int playlist_adicionar_musica(NodoPlaylist *pl, NodoMusica *musica)
{
    if (!pl || !musica)
    {
        return 0;
    }

    /* Impede duplicatas na mesma playlist */
    if (entrada_buscar(pl, musica))
    {
        printf("  Musica ja esta nessa playlist.\n");
        return 0;
    }

    EntradaPlaylist *nova = (EntradaPlaylist *)malloc(sizeof(EntradaPlaylist));
    if (!nova)
    {
        fprintf(stderr, "Erro: sem memoria para adicionar musica na playlist.\n");
        return 0;
    }

    nova->musica = musica;
    nova->prox = NULL;
    nova->ant = NULL;

    /* Insere no fim da lista interna */
    if (!pl->inicio_musicas)
    {
        pl->inicio_musicas = nova;
        pl->fim_musicas = nova;
    }
    else
    {
        nova->ant = pl->fim_musicas;
        pl->fim_musicas->prox = nova;
        pl->fim_musicas = nova;
    }
    pl->tam_musicas++;
    return 1;
}

int playlist_remover_musica(NodoPlaylist *pl, NodoMusica *musica)
{
    if (!pl || !musica)
    {
        return 0;
    }

    EntradaPlaylist *e = entrada_buscar(pl, musica);
    if (!e)
    {
        printf("  Musica nao encontrada nessa playlist.\n");
        return 0;
    }
    entrada_desencadear(pl, e);
    return 1;
}

void playlist_listar_musicas(const NodoPlaylist *pl)
{
    if (!pl->inicio_musicas)
    {
        printf("  Playlist vazia.\n");
        return;
    }
    const EntradaPlaylist *e = pl->inicio_musicas;
    int num = 1;
    printf("  %-4s %-40s %-30s %s\n", "Num", "Titulo", "Artista", "Duracao");
    printf("  %-4s %-40s %-30s %s\n", "---", "------", "-------", "-------");
    while (e)
    {
        int min = e->musica->duracao / 60;
        int seg = e->musica->duracao % 60;
        printf("  %-4d %-40s %-30s %02d:%02d\n", num, e->musica->titulo, e->musica->artista, min, seg);
        e = e->prox;
        num++;
    }
}

NodoMusica *playlist_proxima(const NodoPlaylist *pl, const NodoMusica *atual)
{
    const EntradaPlaylist *e = pl->inicio_musicas;
    while (e)
    {
        if (e->musica == atual)
        {
            if (e->prox)
            {
                return e->prox->musica;
            }
            else
            {
                return NULL;
            }
        }
        e = e->prox;
    }
    return NULL; /* atual nao pertence a esta playlist */
}

NodoMusica *playlist_anterior(const NodoPlaylist *pl, const NodoMusica *atual)
{
    const EntradaPlaylist *e = pl->inicio_musicas;
    while (e)
    {
        if (e->musica == atual)
        {
            if (e->ant)
            {
                return e->ant->musica;
            }
            else
            {
                return NULL;
            }
        }
        e = e->prox;
    }
    return NULL; /* atual nao pertence a esta playlist */
}

NodoMusica *playlist_primeira(const NodoPlaylist *pl)
{
    if (!pl->inicio_musicas)
    {
        return NULL;
    }
    return pl->inicio_musicas->musica;
}

/*  Callback para biblioteca_remover  */

int playlist_em_alguma(const void *lista_playlists, const NodoMusica *musica)
{
    const ListaPlaylists *lp = (const ListaPlaylists *)lista_playlists;
    const NodoPlaylist *pl = lp->inicio;
    while (pl)
    {
        if (entrada_buscar(pl, musica))
        {
            return 1;
        }
        pl = pl->prox;
    }
    return 0;
}

/*  Liberacao total  */

void playlist_lista_liberar(ListaPlaylists *lp)
{
    NodoPlaylist *atual = lp->inicio;
    while (atual)
    {
        NodoPlaylist *prox = atual->prox;
        entradas_liberar(atual);
        free(atual);
        atual = prox;
    }
    lp->inicio = NULL;
    lp->fim = NULL;
    lp->tam = 0;
}
