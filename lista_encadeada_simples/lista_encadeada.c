#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
// ESTRUTURAS BASE
// ============================================================

typedef struct nodo
{
    char nome[100];
    int duracao; // em segundos
    struct nodo *prox;
} Nodo;

typedef struct pilha
{
    Nodo *topo;
    int tam;
} Pilha;

typedef struct fila
{
    Nodo *inicio;
    Nodo *fim;
    int tam;
} Fila;

// ============================================================
// FUNÇÕES AUXILIARES
// ============================================================

Nodo *criaNodo(char *nome, int duracao)
{
    Nodo *novo = (Nodo *)malloc(sizeof(Nodo));
    if (novo == NULL)
    {
        printf("Erro ao alocar memoria!\n");
        exit(1);
    }
    strncpy(novo->nome, nome, 99);
    novo->nome[99] = '\0';
    novo->duracao = duracao;
    novo->prox = NULL;
    return novo;
}

// ============================================================
// EXERCÍCIO 1 - LISTA ENCADEADA SIMPLES
// ============================================================

// Insere no início da lista; retorna novo head
Nodo *inserirInicio(Nodo *head, char *nome, int duracao)
{
    Nodo *novo = criaNodo(nome, duracao);
    novo->prox = head;
    return novo;
}

// Insere no fim da lista; retorna head (inalterado se lista não era vazia)
Nodo *inserirFim(Nodo *head, char *nome, int duracao)
{
    Nodo *novo = criaNodo(nome, duracao);
    if (head == NULL)
        return novo;
    Nodo *temp = head;
    while (temp->prox != NULL)
        temp = temp->prox;
    temp->prox = novo;
    return head;
}

// Busca por nome; retorna ponteiro para o nó ou NULL
Nodo *buscar(Nodo *head, char *nome)
{
    Nodo *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->nome, nome) == 0)
            return temp;
        temp = temp->prox;
    }
    return NULL;
}

// Remove nó com o nome dado; retorna novo head
Nodo *remover(Nodo *head, char *nome)
{
    if (head == NULL)
    {
        printf("Lista vazia.\n");
        return NULL;
    }
    // Remoção do nó raiz
    if (strcmp(head->nome, nome) == 0)
    {
        Nodo *temp = head;
        head = head->prox;
        free(temp);
        return head;
    }
    // Remoção de qualquer outro nó
    Nodo *atual = head;
    while (atual->prox != NULL)
    {
        if (strcmp(atual->prox->nome, nome) == 0)
        {
            Nodo *temp = atual->prox;
            atual->prox = temp->prox;
            free(temp);
            return head;
        }
        atual = atual->prox;
    }
    printf("Musica \"%s\" nao encontrada.\n", nome);
    return head;
}

// Imprime todos os nós da lista
void imprimirLista(Nodo *head)
{
    if (head == NULL)
    {
        printf("Lista vazia.\n");
        return;
    }
    Nodo *temp = head;
    int i = 1;
    while (temp != NULL)
    {
        printf("%d. %s - %d:%02d\n", i++, temp->nome, temp->duracao / 60, temp->duracao % 60);
        temp = temp->prox;
    }
}

// Libera toda a lista
void liberarLista(Nodo *head)
{
    while (head != NULL)
    {
        Nodo *temp = head;
        head = head->prox;
        free(temp);
    }
}

// ============================================================
// EXERCÍCIO 2 - PILHA ENCADEADA (músicas que já tocaram)
// ============================================================

void inicializarPilha(Pilha *p)
{
    p->topo = NULL;
    p->tam = 0;
}

// Push: insere no topo
void push(Pilha *p, char *nome, int duracao)
{
    Nodo *novo = criaNodo(nome, duracao);
    novo->prox = p->topo;
    p->topo = novo;
    p->tam++;
}

// Pop: remove do topo e retorna os dados pelo parâmetro
int pop(Pilha *p, char *nomeOut, int *duracaoOut)
{
    if (p->topo == NULL)
    {
        printf("Pilha vazia.\n");
        return 0;
    }
    Nodo *temp = p->topo;
    strncpy(nomeOut, temp->nome, 99);
    *duracaoOut = temp->duracao;
    p->topo = temp->prox;
    free(temp);
    p->tam--;
    return 1;
}

void imprimirPilha(Pilha *p)
{
    if (p->topo == NULL)
    {
        printf("Pilha vazia.\n");
        return;
    }
    printf("--- Historico (topo = mais recente) ---\n");
    imprimirLista(p->topo);
    printf("Total: %d musicas\n", p->tam);
}

// Busca na pilha por nome
Nodo *buscarPilha(Pilha *p, char *nome)
{
    return buscar(p->topo, nome);
}

// Remove da pilha por nome (não necessariamente o topo)
void removerPilha(Pilha *p, char *nome)
{
    p->topo = remover(p->topo, nome);
    if (p->tam > 0)
        p->tam--;
}

void liberarPilha(Pilha *p)
{
    liberarLista(p->topo);
    p->topo = NULL;
    p->tam = 0;
}

// ============================================================
// EXERCÍCIO 3 - FILA ENCADEADA (playlist do usuário)
// ============================================================

void inicializarFila(Fila *f)
{
    f->inicio = NULL;
    f->fim = NULL;
    f->tam = 0;
}

// Enqueue: insere no fim
void enqueue(Fila *f, char *nome, int duracao)
{
    Nodo *novo = criaNodo(nome, duracao);
    if (f->fim == NULL)
    {
        // Fila estava vazia
        f->inicio = novo;
        f->fim = novo;
    }
    else
    {
        f->fim->prox = novo;
        f->fim = novo;
    }
    f->tam++;
}

// Dequeue: remove do início e retorna os dados pelo parâmetro
int dequeue(Fila *f, char *nomeOut, int *duracaoOut)
{
    if (f->inicio == NULL)
    {
        printf("Fila vazia.\n");
        return 0;
    }
    Nodo *temp = f->inicio;
    strncpy(nomeOut, temp->nome, 99);
    *duracaoOut = temp->duracao;
    f->inicio = temp->prox;
    // Se a fila ficou vazia, atualiza o ponteiro fim
    if (f->inicio == NULL)
        f->fim = NULL;
    free(temp);
    f->tam--;
    return 1;
}

void imprimirFila(Fila *f)
{
    if (f->inicio == NULL)
    {
        printf("Fila vazia.\n");
        return;
    }
    printf("--- Playlist (inicio -> fim) ---\n");
    imprimirLista(f->inicio);
    printf("Total: %d musicas\n", f->tam);
}

Nodo *buscarFila(Fila *f, char *nome)
{
    return buscar(f->inicio, nome);
}

// Remove da fila por nome (não necessariamente o início)
void removerFila(Fila *f, char *nome)
{
    // Precisa atualizar f->fim se o nó removido for o último
    if (f->inicio == NULL)
        return;

    // Caso especial: remoção do início
    if (strcmp(f->inicio->nome, nome) == 0)
    {
        Nodo *temp = f->inicio;
        f->inicio = temp->prox;
        if (f->inicio == NULL)
            f->fim = NULL;
        free(temp);
        f->tam--;
        return;
    }

    Nodo *atual = f->inicio;
    while (atual->prox != NULL)
    {
        if (strcmp(atual->prox->nome, nome) == 0)
        {
            Nodo *temp = atual->prox;
            atual->prox = temp->prox;
            // Se removeu o último, atualiza fim
            if (temp == f->fim)
                f->fim = atual;
            free(temp);
            f->tam--;
            return;
        }
        atual = atual->prox;
    }
    printf("Musica \"%s\" nao encontrada na fila.\n", nome);
}

void liberarFila(Fila *f)
{
    liberarLista(f->inicio);
    f->inicio = NULL;
    f->fim = NULL;
    f->tam = 0;
}

int main()
{
    // --- Exercício 1: Lista ---
    printf("=== EXERCICIO 1: Lista Encadeada ===\n");
    Nodo *lista = NULL;
    lista = inserirFim(lista, "Bohemian Rhapsody", 354);
    lista = inserirFim(lista, "Hotel California", 391);
    lista = inserirFim(lista, "Stairway to Heaven", 482);
    lista = inserirInicio(lista, "Come As You Are", 219);
    imprimirLista(lista);

    printf("\nBuscando 'Hotel California': ");
    Nodo *encontrado = buscar(lista, "Hotel California");
    if (encontrado)
        printf("Encontrada! Duracao: %d:%02d\n", encontrado->duracao / 60, encontrado->duracao % 60);

    printf("\nRemovendo 'Hotel California':\n");
    lista = remover(lista, "Hotel California");
    imprimirLista(lista);
    liberarLista(lista);

    // --- Exercício 2: Pilha ---
    printf("\n=== EXERCICIO 2: Pilha (historico) ===\n");
    Pilha historico;
    inicializarPilha(&historico);
    push(&historico, "Come As You Are", 219);
    push(&historico, "Bohemian Rhapsody", 354);
    push(&historico, "Stairway to Heaven", 482);
    imprimirPilha(&historico);

    char nomePopado[100];
    int durPopada;
    pop(&historico, nomePopado, &durPopada);
    printf("\nPop: %s (%d:%02d)\n", nomePopado, durPopada / 60, durPopada % 60);
    imprimirPilha(&historico);
    liberarPilha(&historico);

    // --- Exercício 3: Fila ---
    printf("\n=== EXERCICIO 3: Fila (playlist) ===\n");
    Fila playlist;
    inicializarFila(&playlist);
    enqueue(&playlist, "Hotel California", 391);
    enqueue(&playlist, "Smells Like Teen Spirit", 301);
    enqueue(&playlist, "Nothing Else Matters", 388);
    imprimirFila(&playlist);

    char nomeDeq[100];
    int durDeq;
    dequeue(&playlist, nomeDeq, &durDeq);
    printf("\nDequeue: %s (%d:%02d)\n", nomeDeq, durDeq / 60, durDeq % 60);
    imprimirFila(&playlist);

    printf("\nRemovendo 'Nothing Else Matters' da fila:\n");
    removerFila(&playlist, "Nothing Else Matters");
    imprimirFila(&playlist);
    liberarFila(&playlist);

    return 0;
}