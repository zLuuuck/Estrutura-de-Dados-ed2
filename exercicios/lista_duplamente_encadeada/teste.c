#include <stdio.h>
#include <stdlib.h>

typedef struct NodoDuplo
{
    int valor;
    struct NodoDuplo *ant;
    struct NodoDuplo *prox;
} NodoDuplo;

typedef struct
{
    NodoDuplo *inicio;
    NodoDuplo *fim;
    int tamanho;
} ListaDupla;

NodoDuplo *criarNodo(int valor)
{
    NodoDuplo *nodo = (NodoDuplo*)malloc(sizeof(NodoDuplo));
    if (nodo == NULL)
        return NULL;
    nodo->valor = valor;
    nodo->ant = NULL;
    nodo->prox = NULL;
    return nodo;
}

ListaDupla *criarLista()
{
    ListaDupla *lista = (ListaDupla*)malloc(sizeof(ListaDupla));
    if (lista == NULL)
        return NULL;
    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    return lista;
}

void inserirInicio(ListaDupla *lista, int valor)
{
    NodoDuplo *nodo = criarNodo(valor);
    if (nodo == NULL){
        printf("Erro ao criar nodo.\n");
        return;
    }

    if (lista->inicio == NULL)
    { // lista vazia
        lista->inicio = nodo;
        lista->fim = nodo;
    }
    else
    {
        nodo->prox = lista->inicio; // novo nodo aponta para o atual inicio
        lista->inicio->ant = nodo;  // atual inicio aponta para o novo njodo
        lista->inicio = nodo;       // inicio da lista atualizado
    }

    lista->tamanho++;
}

void inserirFim(ListaDupla *lista, int valor)
{
    NodoDuplo *nodo = criarNodo(valor);
    if (nodo == NULL){
        printf("Erro ao criar nodo.\n");
        return;
    }

    if (lista->fim == NULL)
    { // lista vazia
        lista->inicio = nodo;
        lista->fim = nodo;
    }
    else
    {
        nodo->ant = lista->fim;  // novo nodo aponta para o atual fim
        lista->fim->prox = nodo; // atual fim aponta para o novo nodo
        lista->fim = nodo;       // fim da lista atualizado
    }

    lista->tamanho++;
}

void imprimirLista(ListaDupla *lista)
{
    NodoDuplo *atual = lista->inicio;
    while (atual != NULL)
    {
        printf("%d ", atual->valor);
        atual = atual->prox;
    }
    printf("\n");
}

void liberarLista(ListaDupla *lista)
{
    NodoDuplo *atual = lista->inicio;
    while (atual != NULL)
    {
        NodoDuplo *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    free(lista);
}

int main()
{
    ListaDupla *lista = criarLista();
    if (lista == NULL)
        return 1;

    inserirFim(lista, 10);
    inserirFim(lista, 20);
    inserirFim(lista, 30);

    imprimirLista(lista); // 10 20 30

    liberarLista(lista);
    return 0;
}