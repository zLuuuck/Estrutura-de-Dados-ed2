#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nodo {
    char nome[50];
    int duracao; // duração em segundos
    struct nodo* prox;
} Nodo;

Nodo *criaNodo(char *nome, int duracao){

    Nodo *novo = (Nodo *) malloc(sizeof(Nodo));
    if (novo == NULL) {
        printf("Erro ao alocar memória para o novo nodo.\n");
        exit(1);
    }
    strncpy(novo->nome, nome, 49);
    novo->nome[49] = '\0';
    novo->duracao = duracao;
    novo->prox = NULL;
    return novo;
}

Nodo *inseririnicio(Nodo *head, char *nome, int duracao) {
    Nodo *novo = criaNodo(nome, duracao);
    novo->prox = head;
    return novo;
}

Nodo *inserirFim(Nodo *head, char *nome, int duracao) {
    Nodo *novo = criaNodo(nome, duracao);
    if (head == NULL) {
        return novo;
    }
    Nodo *temp = head;
    while (temp->prox != NULL) {
        temp = temp->prox;
    }
    temp->prox = novo;
    return head;
}

Nodo *buscar(Nodo *head, char *nome) {
    Nodo *temp = head;
    while (temp != NULL) {
        if (strcmp(temp->nome, nome) == 0) {
            return temp;
        }
        temp = temp->prox;
    }
    return NULL; // Não encontrado
}

Nodo *remover(Nodo *head, char *nome) {
    if (head == NULL) {
        printf("Lista vazia. Não há nada para remover.\n");
        return NULL;
    }
    if (strcmp(head->nome, nome) == 0) {
        Nodo *temp = head;
        head = head->prox;
        free(temp);
        return head;
    }
    Nodo *atual = head;
    while (atual->prox != NULL) {
        if (strcmp(atual->prox->nome, nome) == 0) {
            Nodo *temp = atual->prox;
            atual->prox = temp->prox;
            free(temp);
            return head;
        }
        atual = atual->prox;
    }
    printf("Música com nome %s não encontrada.\n", nome);
    return head;
}

void imprimirLista(Nodo *head) {
    Nodo *temp = head;
    while (temp != NULL) {
        printf("Nome: %s, Duração: %d segundos\n", temp->nome, temp->duracao);
        temp = temp->prox;
    }
}

void liberarLista(Nodo *head) {
    Nodo *temp;
    while (head != NULL) {
        temp = head;
        head = head->prox;
        free(temp);
    }
}

int main() {
    Nodo *lista = NULL;
    int opcao;
    char nome[50];
    int minutos, segundos;

    do {
        printf("\n=== PLAYER DE MUSICAS ===\n");
        printf("1. Adicionar musica\n");
        printf("2. Remover musica\n");
        printf("3. Buscar musica\n");
        printf("4. Listar musicas\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &opcao);
        // Limpa o buffer do teclado após o scanf
        while (getchar() != '\n');

        switch (opcao) {
            case 1:
                printf("Nome da musica: ");
                fgets(nome, 100, stdin);
                nome[strcspn(nome, "\n")] = '\0'; // remove o \n do fgets
                printf("Duracao (minutos): ");
                scanf("%d", &minutos);
                printf("Duracao (segundos): ");
                scanf("%d", &segundos);
                while (getchar() != '\n');
                lista = inserirFim(lista, nome, minutos * 60 + segundos);
                printf("Musica adicionada!\n");
                break;

            case 2:
                printf("Nome da musica a remover: ");
                fgets(nome, 100, stdin);
                nome[strcspn(nome, "\n")] = '\0';
                lista = remover(lista, nome);
                break;

            case 3:
                printf("Nome da musica a buscar: ");
                fgets(nome, 100, stdin);
                nome[strcspn(nome, "\n")] = '\0';
                Nodo *encontrado = buscar(lista, nome);
                if (encontrado)
                    printf("Encontrada: %s - %d:%02d\n", encontrado->nome,
                           encontrado->duracao / 60, encontrado->duracao % 60);
                else
                    printf("Musica nao encontrada.\n");
                break;

            case 4:
                imprimirLista(lista);
                break;

            case 0:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    liberarLista(lista);
    return 0;
}