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
void normaliza_linha(char *s);
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
typedef struct No {
    EntradaIndice *entrada;
    struct No *esq;
    struct No *dir;
} No;
typedef struct {
    No *raiz;
    int altura;
} IndiceArvore;
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
   ///else if (strcmp(argv[2], "arvore") == 0) {
    //    IndiceArvore *idx = cria_indice_arvore();
      //  indexa_texto_arvore(texto, idx);
     ///   imprime_info_arvore(...);
      ///  loop_busca_arvore(texto, idx);
    //} ///
    else {
        printf("Opcao invalida!\n");
    }

    // 4. liberar memória
}
