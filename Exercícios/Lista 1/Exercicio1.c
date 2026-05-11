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
    
}