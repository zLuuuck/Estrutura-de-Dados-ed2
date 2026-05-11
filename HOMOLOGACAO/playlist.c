#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playlist.h"

/* ── init ───────────────────────────────────────────────────────────────── */
void playlists_init(ListaPlaylists *lp) {
    lp->inicio = NULL;
    lp->fim    = NULL;
    lp->tam    = 0;
}

/* ── criar playlist ─────────────────────────────────────────────────────── */
NodoPlaylist *playlist_criar(const char *nome) {
    NodoPlaylist *novo = (NodoPlaylist *) malloc(sizeof(NodoPlaylist));
    if (novo == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória para playlist.\n");
        return NULL;
    }
    strncpy(novo->nome, nome, MAX_NOME_PL - 1);
    novo->nome[MAX_NOME_PL - 1] = '\0';

    /* lista interna de músicas começa vazia */
    novo->musicas.inicio = NULL;
    novo->musicas.fim    = NULL;
    novo->musicas.tam    = 0;

    novo->prox = NULL;
    novo->ant  = NULL;
    return novo;
}

/* ── inserir playlist no fim da lista de playlists ──────────────────────── */
void playlists_inserir(ListaPlaylists *lp, NodoPlaylist *pl) {
    if (pl == NULL) return;

    pl->prox = NULL;
    pl->ant  = lp->fim;

    if (lp->fim != NULL)
        lp->fim->prox = pl;
    else
        lp->inicio = pl;

    lp->fim = pl;
    lp->tam++;
}

/* ── buscar por nome ────────────────────────────────────────────────────── */
NodoPlaylist *playlists_buscar(ListaPlaylists *lp, const char *nome) {
    NodoPlaylist *temp = lp->inicio;
    while (temp != NULL) {
        if (strncasecmp(temp->nome, nome, MAX_NOME_PL) == 0)
            return temp;
        temp = temp->prox;
    }
    return NULL;
}

/* ── remover playlist ───────────────────────────────────────────────────── */
int playlists_remover(ListaPlaylists *lp, const char *nome) {
    NodoPlaylist *alvo = playlists_buscar(lp, nome);
    if (alvo == NULL) {
        printf("Playlist \"%s\" não encontrada.\n", nome);
        return 0;
    }

    if (alvo->ant != NULL) alvo->ant->prox = alvo->prox;
    else                   lp->inicio      = alvo->prox;

    if (alvo->prox != NULL) alvo->prox->ant = alvo->ant;
    else                    lp->fim         = alvo->ant;

    /* libera músicas internas */
    NodoMusica *m = alvo->musicas.inicio, *tmp;
    while (m != NULL) { tmp = m->prox; free(m); m = tmp; }

    free(alvo);
    lp->tam--;
    return 1;
}

/* ── listar playlists ───────────────────────────────────────────────────── */
void playlists_listar(const ListaPlaylists *lp) {
    if (lp->inicio == NULL) {
        printf("  (nenhuma playlist criada)\n");
        return;
    }
    int i = 1;
    NodoPlaylist *temp = lp->inicio;
    printf("  %-4s %-30s %s\n", "#", "Nome", "Músicas");
    printf("  %s\n", "──────────────────────────────────────");
    while (temp != NULL) {
        printf("  %-4d %-30s %d\n", i++, temp->nome, temp->musicas.tam);
        temp = temp->prox;
    }
}

/* ── adicionar música a uma playlist ────────────────────────────────────── */
void playlist_add_musica(NodoPlaylist *pl, const NodoMusica *m) {
    NodoMusica *novo = (NodoMusica *) malloc(sizeof(NodoMusica));
    if (novo == NULL) { fprintf(stderr, "Erro: malloc falhou.\n"); return; }

    *novo = *m;   /* copia todos os campos */
    novo->prox = NULL;
    novo->ant  = pl->musicas.fim;

    if (pl->musicas.fim != NULL)
        pl->musicas.fim->prox = novo;
    else
        pl->musicas.inicio = novo;

    pl->musicas.fim = novo;
    pl->musicas.tam++;
}

/* ── remover música de uma playlist ─────────────────────────────────────── */
int playlist_rem_musica(NodoPlaylist *pl, const char *titulo) {
    NodoMusica *temp = pl->musicas.inicio;
    while (temp != NULL) {
        if (strncasecmp(temp->titulo, titulo, MAX_TITULO) == 0) {
            if (temp->ant != NULL) temp->ant->prox = temp->prox;
            else                   pl->musicas.inicio = temp->prox;

            if (temp->prox != NULL) temp->prox->ant = temp->ant;
            else                    pl->musicas.fim  = temp->ant;

            free(temp);
            pl->musicas.tam--;
            return 1;
        }
        temp = temp->prox;
    }
    printf("Música \"%s\" não está nesta playlist.\n", titulo);
    return 0;
}

/* ── listar músicas de uma playlist ─────────────────────────────────────── */
void playlist_listar_musicas(const NodoPlaylist *pl) {
    if (pl->musicas.inicio == NULL) {
        printf("  (playlist vazia)\n");
        return;
    }
    int i = 1;
    NodoMusica *temp = pl->musicas.inicio;
    printf("  %-4s %-30s %-25s %s\n", "#", "Título", "Artista", "Duração");
    printf("  %s\n", "────────────────────────────────────────────────────────────");
    while (temp != NULL) {
        int min = temp->duracao / 60;
        int seg = temp->duracao % 60;
        printf("  %-4d %-30s %-25s %02d:%02d\n",
               i++, temp->titulo, temp->artista, min, seg);
        temp = temp->prox;
    }
}

/* ── navegar: próxima / anterior ────────────────────────────────────────── */
NodoMusica *playlist_proxima(NodoPlaylist *pl, NodoMusica *atual) {
    if (atual == NULL) return pl->musicas.inicio;
    return atual->prox;   /* NULL se era o último */
}

NodoMusica *playlist_anterior(NodoPlaylist *pl, NodoMusica *atual) {
    if (atual == NULL) return pl->musicas.fim;
    return atual->ant;    /* NULL se era o primeiro */
}

/* ── liberar todas as playlists ─────────────────────────────────────────── */
void playlists_liberar(ListaPlaylists *lp) {
    NodoPlaylist *pl = lp->inicio, *tmp_pl;
    while (pl != NULL) {
        NodoMusica *m = pl->musicas.inicio, *tmp_m;
        while (m != NULL) { tmp_m = m->prox; free(m); m = tmp_m; }
        tmp_pl = pl->prox;
        free(pl);
        pl = tmp_pl;
    }
    lp->inicio = NULL;
    lp->fim    = NULL;
    lp->tam    = 0;
}
