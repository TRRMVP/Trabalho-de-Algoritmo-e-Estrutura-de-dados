EP – Algoritmos e Estruturas de Dados I
Índice de Palavras com Lista Sequencial e Árvore AVL

Autores:
- Giuseppe – Nº USP XXXXXXXX
- Isabella Sousa – Nº USP 16941052

--------------------------------------------------------------------
COMPILAÇÃO
--------------------------------------------------------------------

O programa foi desenvolvido em linguagem C, seguindo o padrão C99,
e testado no ambiente Windows (PowerShell / Visual Studio Code).

Para compilar utilizando o GCC, execute o comando abaixo no diretório
onde se encontra o arquivo fonte:

    gcc -std=c99 -Wall -Wextra -O2 -o arquivoEP arquivoEP.c

O comando acima gera o executável "arquivoEP.exe" no ambiente Windows.

--------------------------------------------------------------------
EXECUÇÃO
--------------------------------------------------------------------

O programa recebe dois parâmetros pela linha de comando:

    arquivoEP <arquivo_texto> <lista|arvore>

Onde:
- <arquivo_texto> é o arquivo de entrada contendo o texto a ser indexado
- <lista> cria o índice utilizando lista sequencial
- <arvore> cria o índice utilizando árvore AVL

--------------------------------------------------------------------
EXECUÇÃO NO WINDOWS (POWERSHELL)
--------------------------------------------------------------------

No PowerShell, para executar um programa localizado no diretório atual,
é necessário utilizar o operador "&", pois, por padrão, o PowerShell
não executa arquivos locais diretamente.

Exemplo:

    & .\arquivoEP.exe texto.txt arvore

A mensagem de uso exibida pelo programa é:

    Uso: <caminho_do_executavel> <arquivo> <lista|arvore>

--------------------------------------------------------------------
FORMATO DO ARQUIVO DE TEXTO
--------------------------------------------------------------------

O arquivo de entrada deve estar codificado em ASCII ou UTF-8 sem BOM.
Arquivos em UTF-16 (padrão do PowerShell) podem causar problemas na leitura
do texto utilizando funções padrão da linguagem C.

Exemplo de criação correta do arquivo de texto no PowerShell:

    1..300 | ForEach-Object { "palavra$($_)" } |
    Set-Content texto.txt -Encoding utf8

--------------------------------------------------------------------
COMANDOS INTERATIVOS
--------------------------------------------------------------------

Após a indexação do texto, o programa entra em modo interativo.

Comandos disponíveis:

    busca <palavra>   realiza a busca da palavra no índice
    fim               encerra a execução do programa

--------------------------------------------------------------------
SAÍDA
--------------------------------------------------------------------

O programa exibe:

- Número total de palavras distintas
- Altura da árvore AVL (quando a estrutura escolhida for árvore)
- Número de comparações realizadas durante as operações de busca
- Linhas do texto em que a palavra ocorre
