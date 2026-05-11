#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
// EXERCÍCIO 4 - LISTA DUPLAMENTE ENCADEADA (player de músicas)
// Permite avançar e retroceder sem estruturas secundárias
// ============================================================

typedef struct nodo {
    char nome[100];
    int duracao; // em segundos
    struct nodo *prox;
    struct nodo *ant;
} Nodo;

typedef struct player {
    Nodo *inicio;
    Nodo *fim;
    Nodo *atual;  // ponteiro para a música que está tocando
    int tam;
} Player;

// ------------------------------------------------------------
// Funções auxiliares
// ------------------------------------------------------------

Nodo *criaNodo(char *nome, int duracao) {
    Nodo *novo = (Nodo *) malloc(sizeof(Nodo));
    if (novo == NULL) {
        printf("Erro ao alocar memoria!\n");
        exit(1);
    }
    strncpy(novo->nome, nome, 99);
    novo->nome[99] = '\0';
    novo->duracao = duracao;
    novo->prox = NULL;
    novo->ant = NULL;
    return novo;
}

void inicializarPlayer(Player *p) {
    p->inicio = NULL;
    p->fim = NULL;
    p->atual = NULL;
    p->tam = 0;
}

// ------------------------------------------------------------
// Inserção no fim
// ------------------------------------------------------------

void inserirMusica(Player *p, char *nome, int duracao) {
    Nodo *novo = criaNodo(nome, duracao);
    if (p->fim == NULL) {
        // Lista vazia
        p->inicio = novo;
        p->fim = novo;
        p->atual = novo; // primeira música adicionada vira a atual
    } else {
        novo->ant = p->fim;
        p->fim->prox = novo;
        p->fim = novo;
    }
    p->tam++;
}

// ------------------------------------------------------------
// Remoção por nome
// ------------------------------------------------------------

void removerMusica(Player *p, char *nome) {
    if (p->inicio == NULL) {
        printf("Player vazio.\n");
        return;
    }

    Nodo *temp = p->inicio;
    while (temp != NULL) {
        if (strcmp(temp->nome, nome) == 0) {
            // Atualiza ponteiro atual se for o nó sendo removido
            if (temp == p->atual) {
                // Tenta avançar; se não tiver próximo, recua
                if (temp->prox != NULL)
                    p->atual = temp->prox;
                else if (temp->ant != NULL)
                    p->atual = temp->ant;
                else
                    p->atual = NULL;
            }

            // Reconecta os vizinhos
            if (temp->ant != NULL)
                temp->ant->prox = temp->prox;
            else
                p->inicio = temp->prox; // era o primeiro

            if (temp->prox != NULL)
                temp->prox->ant = temp->ant;
            else
                p->fim = temp->ant; // era o último

            free(temp);
            p->tam--;
            printf("Musica \"%s\" removida.\n", nome);
            return;
        }
        temp = temp->prox;
    }
    printf("Musica \"%s\" nao encontrada.\n", nome);
}

// ------------------------------------------------------------
// Busca por nome
// ------------------------------------------------------------

Nodo *buscarMusica(Player *p, char *nome) {
    Nodo *temp = p->inicio;
    while (temp != NULL) {
        if (strcmp(temp->nome, nome) == 0)
            return temp;
        temp = temp->prox;
    }
    return NULL;
}

// ------------------------------------------------------------
// Navegação: avançar e retroceder
// ------------------------------------------------------------

void avancar(Player *p) {
    if (p->atual == NULL) {
        printf("Player vazio.\n");
        return;
    }
    if (p->atual->prox == NULL) {
        printf("Fim da playlist. Nao ha proxima musica.\n");
        return;
    }
    p->atual = p->atual->prox;
    printf("Avancou para: %s\n", p->atual->nome);
}

void retroceder(Player *p) {
    if (p->atual == NULL) {
        printf("Player vazio.\n");
        return;
    }
    if (p->atual->ant == NULL) {
        printf("Inicio da playlist. Nao ha musica anterior.\n");
        return;
    }
    p->atual = p->atual->ant;
    printf("Retrocedeu para: %s\n", p->atual->nome);
}

// ------------------------------------------------------------
// Impressão
// ------------------------------------------------------------

void imprimirPlayer(Player *p) {
    if (p->inicio == NULL) {
        printf("Player vazio.\n");
        return;
    }
    printf("--- Playlist (inicio -> fim) ---\n");
    Nodo *temp = p->inicio;
    int i = 1;
    while (temp != NULL) {
        // Marca a música atual com ">"
        printf("%s %d. %s - %d:%02d\n", (temp == p->atual) ? ">" : " ", i++, temp->nome, temp->duracao / 60, temp->duracao % 60);
        temp = temp->prox;
    }
    printf("Total: %d musicas\n", p->tam);
}

void imprimirAtual(Player *p) {
    if (p->atual == NULL) {
        printf("Nenhuma musica tocando.\n");
        return;
    }
    printf("Tocando agora: %s (%d:%02d)\n", p->atual->nome, p->atual->duracao / 60, p->atual->duracao % 60);
}

// ------------------------------------------------------------
// Liberar memória
// ------------------------------------------------------------

void liberarPlayer(Player *p) {
    Nodo *temp = p->inicio;
    while (temp != NULL) {
        Nodo *prox = temp->prox;
        free(temp);
        temp = prox;
    }
    p->inicio = NULL;
    p->fim = NULL;
    p->atual = NULL;
    p->tam = 0;
}

// ------------------------------------------------------------
// MAIN - Demonstração
// ------------------------------------------------------------

int main() {
    printf("=== EXERCICIO 4: Lista Duplamente Encadeada (Player) ===\n\n");

    Player player;
    inicializarPlayer(&player);

    inserirMusica(&player, "Bohemian Rhapsody", 354);
    inserirMusica(&player, "Hotel California", 391);
    inserirMusica(&player, "Stairway to Heaven", 482);
    inserirMusica(&player, "Smells Like Teen Spirit", 301);
    inserirMusica(&player, "Nothing Else Matters", 388);

    imprimirPlayer(&player);
    printf("\n");

    imprimirAtual(&player);
    avancar(&player);
    avancar(&player);
    imprimirAtual(&player);
    printf("\n");

    retroceder(&player);
    imprimirAtual(&player);
    printf("\n");

    printf("Buscando 'Stairway to Heaven': ");
    Nodo *encontrado = buscarMusica(&player, "Stairway to Heaven");
    if (encontrado)
        printf("Encontrada! Duracao: %d:%02d\n", encontrado->duracao / 60, encontrado->duracao % 60);

    printf("\nRemovendo 'Hotel California':\n");
    removerMusica(&player, "Hotel California");
    imprimirPlayer(&player);

    printf("\nRemovendo a musica atual ('Bohemian Rhapsody'):\n");
    // Força atual para o início para testar esse case
    player.atual = player.inicio;
    removerMusica(&player, "Bohemian Rhapsody");
    imprimirPlayer(&player);
    imprimirAtual(&player);

    // Testa limites de navegação
    printf("\n");
    retroceder(&player); // deve avisar que está no início
    // Navega até o fim
    avancar(&player);
    avancar(&player);
    avancar(&player); // deve avisar que está no fim

    liberarPlayer(&player);
    return 0;
}