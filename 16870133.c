#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
typedef struct {
    char **linhas;
    int qtd;
    int capacidade;
} Texto;
typedef struct {
    char *palavra;
    int total_ocorrencias;

    int *linhas;      // vetor de números de linha
    int qtd_linhas;
    int cap_linhas;
} EntradaIndice;
EntradaIndice *cria_entrada(char *palavra, int linha) {
    EntradaIndice *e = malloc(sizeof(EntradaIndice));
    e->palavra = strdup(palavra);
    e->total_ocorrencias = 1;

    e->cap_linhas = 4;
    e->qtd_linhas = 1;
    e->linhas = malloc(e->cap_linhas * sizeof(int));
    e->linhas[0] = linha;

    return e;
}
typedef struct {
    EntradaIndice **entradas;
    int qtd;
    int capacidade;
} IndiceLista;
IndiceLista *cria_indice_lista() {
    IndiceLista *idx = malloc(sizeof(IndiceLista));
    idx->qtd = 0;
    idx->capacidade = 10;
    idx->entradas = malloc(idx->capacidade * sizeof(EntradaIndice *));
    return idx;
}
// protótipos
void normaliza_linha(char *s);
void normaliza_linha(char *s);

long indexa_texto_lista(Texto *texto, IndiceLista *idx);
long indexa_texto_avl(Texto *texto, IndiceAVL *idx);

EntradaIndice *avl_busca(NoAVL *raiz, char *palavra, long *comparacoes);
// fim dos protótipos
void entrada_adiciona_linha(EntradaIndice *e, int linha) {
    if (e->linhas[e->qtd_linhas - 1] == linha)
        return;

    if (e->qtd_linhas == e->cap_linhas) {
        e->cap_linhas *= 2;
        e->linhas = realloc(e->linhas, e->cap_linhas * sizeof(int));
    }

    e->linhas[e->qtd_linhas++] = linha;
}
void indice_lista_insere(IndiceLista *idx, char *palavra, int linha, long *comparacoes) {
    for (int i = 0; i < idx->qtd; i++) {
        (*comparacoes)++;
        if (strcmp(idx->entradas[i]->palavra, palavra) == 0) {
            idx->entradas[i]->total_ocorrencias++;
            entrada_adiciona_linha(idx->entradas[i], linha);
            return;
        }
    }

    if (idx->qtd == idx->capacidade) {
        idx->capacidade *= 2;
        idx->entradas = realloc(idx->entradas,
                                 idx->capacidade * sizeof(EntradaIndice *));
    }

    idx->entradas[idx->qtd++] = cria_entrada(palavra, linha);
}
long indexa_texto_lista(Texto *texto, IndiceLista *idx) {
    long comparacoes = 0;

    for (int i = 0; i < texto->qtd; i++) {
        char *copia = strdup(texto->linhas[i]);

        normaliza_linha(copia);

        char *palavra = strtok(copia, " "); 

        while (palavra != NULL) {
            if (strlen(palavra) > 0) {
                indice_lista_insere(idx, palavra, i + 1, &comparacoes);
            }
            palavra = strtok(NULL, " ");
        }

        free(copia);
    }

    return comparacoes;
}
typedef struct NoAVL {
    EntradaIndice *entrada;
    struct NoAVL *esq;
    struct NoAVL *dir;
    int altura;
} NoAVL;

typedef struct {
    NoAVL *raiz;
} IndiceAVL;
int altura_no(NoAVL *n) {
    return n ? n->altura : -1;
}
int max(int a, int b) {
    return (a > b) ? a : b;
}
void atualiza_altura(NoAVL *n) {
    n->altura = max(altura_no(n->esq), altura_no(n->dir)) + 1;
}
int fator_balanceamento(NoAVL *n) {
    return altura_no(n->esq) - altura_no(n->dir);
}
NoAVL *rotacao_direita(NoAVL *y) {
    NoAVL *x = y->esq;
    NoAVL *T2 = x->dir;

    x->dir = y;
    y->esq = T2;

    atualiza_altura(y);
    atualiza_altura(x);

    return x;
}
NoAVL *rotacao_esquerda(NoAVL *x) {
    NoAVL *y = x->dir;
    NoAVL *T2 = y->esq;

    y->esq = x;
    x->dir = T2;

    atualiza_altura(x);
    atualiza_altura(y);

    return y;
}
NoAVL *rotacao_LR(NoAVL *n) {
    n->esq = rotacao_esquerda(n->esq);
    return rotacao_direita(n);
}
NoAVL *rotacao_RL(NoAVL *n) {
    n->dir = rotacao_direita(n->dir);
    return rotacao_esquerda(n);
}
NoAVL *avl_insere_rec(NoAVL *raiz, char *palavra, int linha, long *comparacoes) {
    if (raiz == NULL) {
        NoAVL *novo = malloc(sizeof(NoAVL));
        novo->entrada = cria_entrada(palavra, linha);
        novo->esq = novo->dir = NULL;
        novo->altura = 0;
        return novo;
    }

    (*comparacoes)++;
    int cmp = strcmp(palavra, raiz->entrada->palavra);

    if (cmp == 0) {
        raiz->entrada->total_ocorrencias++;
        entrada_adiciona_linha(raiz->entrada, linha);
        return raiz;
    }
    else if (cmp < 0) {
        raiz->esq = avl_insere_rec(raiz->esq, palavra, linha, comparacoes);
    }
    else {
        raiz->dir = avl_insere_rec(raiz->dir, palavra, linha, comparacoes);
    }

    // Atualiza altura
    atualiza_altura(raiz);

    // Verifica balanceamento
    int fb = fator_balanceamento(raiz);

    // LL
    if (fb > 1 && strcmp(palavra, raiz->esq->entrada->palavra) < 0)
        return rotacao_direita(raiz);

    // RR
    if (fb < -1 && strcmp(palavra, raiz->dir->entrada->palavra) > 0)
        return rotacao_esquerda(raiz);

    // LR
    if (fb > 1 && strcmp(palavra, raiz->esq->entrada->palavra) > 0)
        return rotacao_LR(raiz);

    // RL
    if (fb < -1 && strcmp(palavra, raiz->dir->entrada->palavra) < 0)
        return rotacao_RL(raiz);

    return raiz;
}
IndiceAVL *cria_indice_avl() {
    IndiceAVL *idx = malloc(sizeof(IndiceAVL));
    idx->raiz = NULL;
    return idx;
}

void indice_avl_insere(IndiceAVL *idx, char *palavra, int linha, long *comparacoes) {
    idx->raiz = avl_insere_rec(idx->raiz, palavra, linha, comparacoes);
}
void normaliza_linha(char *s) {
    for (int i = 0; s[i]; i++) {
        if (ispunct((unsigned char)s[i]) && s[i] != '-') {
            s[i] = ' ';
        } else if (s[i] == '-') {
            s[i] = ' ';
        } else {
            s[i] = tolower((unsigned char)s[i]);
        }
    }
}
long indexa_texto_avl(Texto *texto, IndiceAVL *idx) {
    long comparacoes = 0;

    for (int i = 0; i < texto->qtd; i++) {
        char *copia = strdup(texto->linhas[i]);
        normaliza_linha(copia);

        char *palavra = strtok(copia, " ");
        while (palavra) {
            if (strlen(palavra) > 0)
                indice_avl_insere(idx, palavra, i + 1, &comparacoes);

            palavra = strtok(NULL, " ");
        }
        free(copia);
    }
    return comparacoes;
}
EntradaIndice *avl_busca(NoAVL *raiz, char *palavra, long *comparacoes) {
    if (!raiz) return NULL;

    (*comparacoes)++;
    int cmp = strcmp(palavra, raiz->entrada->palavra);

    if (cmp == 0)
        return raiz->entrada;
    else if (cmp < 0)
        return avl_busca(raiz->esq, palavra, comparacoes);
    else
        return avl_busca(raiz->dir, palavra, comparacoes);
}
int altura_avl(NoAVL *raiz) {
    return raiz ? raiz->altura : -1;
}
int conta_nos(NoAVL *raiz) {
    if (!raiz) return 0;
    return 1 + conta_nos(raiz->esq) + conta_nos(raiz->dir);
}

void imprime_info_avl(IndiceAVL *idx) {
    int total = conta_nos(idx->raiz);
    printf("Total de palavras distintas: %d\n", total);
    printf("Altura da AVL: %d\n", altura_avl(idx->raiz));
}
void loop_busca_avl(Texto *texto, IndiceAVL *idx) {
    char comando[256];

    while (1) {
        printf("> ");
        if (!fgets(comando, sizeof(comando), stdin))
            break;

        comando[strcspn(comando, "\n")] = '\0';

        if (strcmp(comando, "fim") == 0) {
            break;
        }

        if (strncmp(comando, "busca ", 6) == 0) {
            char *palavra = comando + 6;
            normaliza_linha(palavra);

            long comparacoes = 0;
            EntradaIndice *e =
                avl_busca(idx->raiz, palavra, &comparacoes);

            if (e) {
                imprime_resultado_busca(texto, e);
            } else {
                printf("Palavra '%s' nao encontrada.\n", palavra);
            }

            printf("Numero de comparacoes: %ld\n", comparacoes);
        }
        else {
            printf("Opcao invalida!\n");
        }
    }
}
void imprime_info_lista(IndiceLista *idx) {
    printf("Total de palavras distintas: %d\n", idx->qtd);
}
Texto *cria_texto() {
    Texto *t = malloc(sizeof(Texto));
    t->qtd = 0;
    t->capacidade = 10;
    t->linhas = malloc(t->capacidade * sizeof(char *));
    return t;
}
void texto_adiciona_linha(Texto *t, char *linha) {
    if (t->qtd == t->capacidade) {
        t->capacidade *= 2;
        t->linhas = realloc(t->linhas, t->capacidade * sizeof(char *));
    }

    t->linhas[t->qtd] = strdup(linha); // copia a linha
    t->qtd++;
}
Texto *carrega_texto(char *nome_arquivo) {

    FILE *in = fopen(nome_arquivo, "r");
    if (!in) return NULL;

    Texto *texto = cria_texto();

    char *linha = NULL;
    size_t tamanho = 0;
    ssize_t lidos;

    while ((lidos = getline(&linha, &tamanho, in)) != -1) {

        // remove '\n'
        if (lidos > 0 && linha[lidos - 1] == '\n')
            linha[lidos - 1] = '\0';

        texto_adiciona_linha(texto, linha);
    }

    free(linha);
    fclose(in);

    return texto;
}
EntradaIndice *indice_lista_busca(IndiceLista *idx, char *palavra, long *comparacoes) {
    for (int i = 0; i < idx->qtd; i++) {
        (*comparacoes)++;
        if (strcmp(idx->entradas[i]->palavra, palavra) == 0) {
            return idx->entradas[i];
        }
    }
    return NULL;
}
void imprime_resultado_busca(Texto *texto, EntradaIndice *e) {
    printf("Existem %d ocorrências da palavra '%s' na(s) seguinte(s) linha(s):\n",
           e->total_ocorrencias, e->palavra);

    for (int i = 0; i < e->qtd_linhas; i++) {
        int linha = e->linhas[i];
        printf("%05d: %s\n", linha, texto->linhas[linha - 1]);
    }
}
void loop_busca_lista(Texto *texto, IndiceLista *idx) {
    char comando[256];

    while (1) {
        printf("> ");
        if (!fgets(comando, sizeof(comando), stdin))
            break;

        // remove '\n'
        comando[strcspn(comando, "\n")] = '\0';

        if (strcmp(comando, "fim") == 0) {
            break;
        }

        if (strncmp(comando, "busca ", 6) == 0) {
            char *palavra = comando + 6;

            // normaliza palavra
            normaliza_linha(palavra);

            long comparacoes = 0;
            EntradaIndice *e = indice_lista_busca(idx, palavra, &comparacoes);

            if (e) {
                imprime_resultado_busca(texto, e);
            } else {
                printf("Palavra '%s' nao encontrada.\n", palavra);
            }

            printf("Numero de comparacoes: %ld\n", comparacoes);
        }
        else {
            printf("Opcao invalida!\n");
        }
    }
}
int main(int argc, char *argv[]) {

    // 1. validar parâmetros
    // argv[1] = arquivo
    // argv[2] = lista | arvore

    // 2. carregar texto
    if (argc != 3) {
        printf("Uso: %s <arquivo> <lista|arvore>\n", argv[0]);
        return 1;
    }
    Texto *texto = carrega_texto(argv[1]);
    if (!texto) {
        printf("Erro ao carregar arquivo.\n");
        return 1;
    }

    // 3. criar índice
    if (strcmp(argv[2], "lista") == 0) {
        IndiceLista *idx = cria_indice_lista();
        indexa_texto_lista(texto, idx);
        imprime_info_lista(idx);
        loop_busca_lista(texto, idx);
    }
    else if (strcmp(argv[2], "arvore") == 0) {
            IndiceAVL *idx = cria_indice_arvore();
            indexa_texto_avl(texto, idx);
            imprime_info_avl(idx);
            loop_busca_avl(texto, idx);
    } 
    else {
        printf("Opcao invalida!\n");
    }

    // 4. liberar memória
}
