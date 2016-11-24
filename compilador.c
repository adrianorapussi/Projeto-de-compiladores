//
// Created by Adriano Rapussi
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * -----------------------------------------------------------------------
 * Funcoes usadas
 * -----------------------------------------------------------------------
 */

void analiseLexica(void);

int stringsIdentificadas(char *id);

void removerLinhaComentario();

void escreverArquivo(char *tk);

void lerArquivo(char *nomeDoArquivo);

void geradorTokens(char *tk);

void analiseSintatica(void);

void lerArquivoToken(void);

void guardarReferenciaTokens(char *key, char *value);

char *separarIdOuValorDeTokens(char *vetorTokens, int tipoDeRetorno);

bool isAnaliseSintaticaWithoutError(void);

void nonTerminalStart(void);

void nonTerminalError(void);

bool nonTerminalRefuse(void);

bool nonTerminalAccept(void);

void pushLog(char *funcao, char *esperado);

bool popLog(void);

bool match(char *tk, char *word);

void update(int newvalue);

void increment(void);

void empilha(int vstart, int vcurrent);

void desempilha(void);

int getCurrentPosOnStack(void);

bool pegarProximo(char *word);

bool isParametro(int idxLexemaNomeMetodo);

bool isVariavel(void);

bool isDeclaracaoMetodo(void);

bool isClasse(void);

bool isChamadaFuncao(void);

bool isInstanciaDeClasse(void);

bool isAtribuicao(void);

bool isMultiplicacao(void);

bool isAdicao(void);

bool isSubtracao(void);

bool isEquivalencia(void);

bool isFuncaoOuProcedimento(void);

bool isMainDeClasse(void);

const char* getRecognizedTokenValueByIndex(int idx);

const char* getRecognizedTokenKeyByIndex(int idx);

bool isStringsEqual(const char *string1, char *string2);

int addStructID(char *id);

void setStructIDType(char vtype, int idx);

void setStrucIDMethodInputType(char vInputType, int idx);

char getStructIDType(int idx);

char getStructIDMethodInputType(int idx);

char *getStructIDName(int idx);

int salvarTokenIdEmLexema(int idxTokenPosition);

void printStructID();

void pushPilhaSemantica(int indiceTokenComeco, int indiceTokenFinal);

bool regraAritmetica(int primeiroToken, int ultimoToken);

bool regraParametro(int primeiroToken, int ultimoToken);

bool validarSemantica(int primeiroToken, int ultimoToken);

void analiseSemantica(void);

/*
 * -------------------------------------------------------------------------------------------------------------------
 * Constantes
 * -------------------------------------------------------------------------------------------------------------------
 */
#define BUFFER 4196
char stringLexical[BUFFER];
char vetorStringsIdentificadas[128][30];
int indiceVetorStringsIdentificadas = 0;
static char tokenFile[] = "outTokens.txt";
static char programFile[] = "programFile.txt";

char stringSintatic[BUFFER];
char matrizIdToken[2048][30];
char matrizValorToken[2048][30];
int indiceTokens = 0;

int indicePilha = 0;

struct Pilha {
    int inicio;
    int atual;
};
struct Pilha pilha[128];

int indicePilhaLog = 0;

/**
 * Pilha para mensagens de erro
 */
struct PilhaLog {
    char mensagem[2048];
    int atual;
};
struct PilhaLog pilhaLog[128];

struct StructID {
    char ID[128][30]; //128 posicoes com 30 caracteres cada
    char type[128]; //(i)nt, (b)oolean, (c)lass
    char methodInputType[128]; //(i)nt (b)oolean
    int idx; //indice corrente;
};
struct StructID structID;

struct PilhaSemantica {
    int idxTokenStart[256];
    int idxTokenEnd[256];
    int idx;
};
struct PilhaSemantica pilhaSemantica;

/**
 * -------------------------------------------------------------------------------------------------------------------
 * Funcao principal
 * -------------------------------------------------------------------------------------------------------------------
 */

int main() {
    analiseLexica();
    analiseSintatica();
    analiseSemantica();
    return 0;
}


//---------------------------------------------------------------------------------------------------------------------
//Analisador Lexico
//---------------------------------------------------------------------------------------------------------------------

/**
 * Executa o fluxo de analise lexica
 */
void analiseLexica(void) {
    lerArquivo(programFile);
    removerLinhaComentario();
    char *vetorString;
    vetorString = strtok(stringLexical, " ");
    while (vetorString != NULL) {
        geradorTokens(vetorString);
        vetorString = strtok(NULL, " ");
    }
}

/**
 * -------------------------------------------------------------------------------------------------------------------
 * Valida strings
 * -------------------------------------------------------------------------------------------------------------------
 */
int stringsIdentificadas(char *id) {
    int i = 0;
    for (i = 0; i < indiceVetorStringsIdentificadas; i++) {
        if (strcmp(vetorStringsIdentificadas[i], id) == 0) {
            i = i + 1;
            return i;
        }
    }
    strcpy(vetorStringsIdentificadas[indiceVetorStringsIdentificadas], id);
    i = indiceVetorStringsIdentificadas;
    indiceVetorStringsIdentificadas++;
    i = i + 1;
    return i;
}

/**
 * -------------------------------------------------------------------------------------------------------------------
 * Funcao que remove as linhas de comentario do codigo
 * -------------------------------------------------------------------------------------------------------------------
 */
void removerLinhaComentario() {
    char stringWithoutComments[BUFFER];
    int i;
    int indexStringWithoutComments = 0;
    bool ignorar = false;
    for (i = 1; i < strlen(stringLexical); i++) {
        if (stringLexical[i] == '*' && stringLexical[i - 1] == '/') {
            ignorar = true;
            indexStringWithoutComments--;
            continue;
        }
        if (stringLexical[i] == '/' && stringLexical[i - 1] == '*' && ignorar == true) {
            ignorar = false;
            continue;
        }
        if (ignorar == false) {
            stringWithoutComments[indexStringWithoutComments] = stringLexical[i];
            stringWithoutComments[indexStringWithoutComments + 1] = '\0';
            indexStringWithoutComments++;
        }
    }
    strcpy(stringLexical, stringWithoutComments);
}

/**
 * -------------------------------------------------------------------------------------------------------------------
 * Funcao que grava a string recebida em um arquivo output de nome representado pela variavel TOKEN_FILE
 * -------------------------------------------------------------------------------------------------------------------
 */
void escreverArquivo(char *tk) {
    FILE *fp;
    fp = fopen(tokenFile, "a");
    fprintf(fp, "%s\n", tk);
    fclose(fp);
}

/**
 * -------------------------------------------------------------------------------------------------------------------
 * Funcao que realiza a leitura do arquivo leitura.txt e armazena o conteudo do arquivo em um buffer
 * -------------------------------------------------------------------------------------------------------------------
 */
void lerArquivo(char *nomeDoArquivo) {
    if (nomeDoArquivo == NULL) {
        printf("ERRO ao ler arquivo");
        return;
    }
    FILE *fp;
    char linhaCaracter;
    fp = fopen(nomeDoArquivo, "r");
    bool ignorarLinha = false;
    //boolean importado de stdbool.h
    while (true) {
        linhaCaracter = (char) fgetc(fp);
        if (feof(fp)) {
            break;
        } else if (linhaCaracter != '\n' && ignorarLinha == false) {
            if (linhaCaracter == '/' && stringLexical[strlen(stringLexical) - 1] == '/') {
                // Caso identifique o inicio de um comentario (duas barras verticais em sequencia)
                // ignora a linha
                stringLexical[strlen(stringLexical) - 1] = '\0';
                ignorarLinha = true;
                continue;
            }
            stringLexical[strlen(stringLexical)] = linhaCaracter;
            stringLexical[strlen(stringLexical) + 1] = '\0';
        } else if (ignorarLinha == true) {
            if (linhaCaracter == '\n') {
                //quando encontrar \n com ignorarlinha ligado concatena espaço
                strcat(stringLexical, " ");
                ignorarLinha = false;
            } else {
                //Caracteres que devem ser ignorados
                continue;
            }
        } else {
            strcat(stringLexical, " ");
        }
    }
    fclose(fp);
}

/**
 * -------------------------------------------------------------------------------------------------------------------
 * Analisa a linha e determina o tipo de token a ser gravado no arquivo de saida
 * -------------------------------------------------------------------------------------------------------------------
 */
void geradorTokens(char *tk) {
    printf("%s \n", tk);
    // PALAVRAS RESERVADAS
    if (strcmp("if", tk) == 0) {
        escreverArquivo("<IF>");
        return;
    } else if (strcmp("while", tk) == 0) {
        escreverArquivo("<WHILE>");
        return;
    } else if (strcmp("this", tk) == 0) {
        escreverArquivo("<THIS>");
        return;
    } else if (strcmp("new", tk) == 0) {
        escreverArquivo("<NEW>");
        return;
    } else if (strcmp("else", tk) == 0) {
        escreverArquivo("<ELSE>");
        return;
    } else if (strcmp("System.out.println", tk) == 0) {
        escreverArquivo("<SYSOUTPRINTLN>");
        return;
    } else if (strcmp("boolean", tk) == 0) {
        escreverArquivo("<BOOLEAN>");
        return;
    } else if (strcmp("class", tk) == 0) {
        escreverArquivo("<CLASS>");
        return;
    } else if (strcmp("extends", tk) == 0) {
        escreverArquivo("<EXTENDS>");
        return;
    } else if (strcmp("public", tk) == 0) {
        escreverArquivo("<PUBLIC>");
        return;
    } else if (strcmp("static", tk) == 0) {
        escreverArquivo("<STATIC>");
        return;
    } else if (strcmp("void", tk) == 0) {
        escreverArquivo("<VOID>");
        return;
    } else if (strcmp("main", tk) == 0) {
        escreverArquivo("<MAIN>");
        return;
    } else if (strcmp("return", tk) == 0) {
        escreverArquivo("<RETURN>");
        return;
    } else if (strcmp("int", tk) == 0) {
        escreverArquivo("<INT>");
        return;
    }
        // PONTUACAO
    else if (strcmp("(", tk) == 0) {
        escreverArquivo("<ABRE_PARENTESES>");
        return;
    } else if (strcmp(")", tk) == 0) {
        escreverArquivo("<FECHA_PARENTESES>");
        return;
    } else if (strcmp("[", tk) == 0) {
        escreverArquivo("<ABRE_COLCHETE>");
        return;
    } else if (strcmp("]", tk) == 0) {
        escreverArquivo("<FECHA_COLCHETE>");
        return;
    } else if (strcmp("{", tk) == 0) {
        escreverArquivo("<ABRE_CHAVES>");
        return;
    } else if (strcmp("}", tk) == 0) {
        escreverArquivo("<FECHA_CHAVES>");
        return;
    } else if (strcmp(".", tk) == 0) {
        escreverArquivo("<PONTO>");
        return;
    } else if (strcmp(",", tk) == 0) {
        escreverArquivo("<VIRGULA>");
        return;
    } else if (strcmp(";", tk) == 0) {
        escreverArquivo("<PONTOVIRGULA>");
        return;
    }
        // OPERADORES
    else if (strcmp("==", tk) == 0) {
        escreverArquivo("<EQUIVALENTE>");
        return;
    } else if (strcmp("&&", tk) == 0) {
        escreverArquivo("<AND>");
        return;
    } else if (strcmp("-", tk) == 0) {
        escreverArquivo("<MENOS>");
        return;
    } else if (strcmp("+", tk) == 0) {
        escreverArquivo("<MAIS>");
        return;
    } else if (strcmp("*", tk) == 0) {
        escreverArquivo("<MULT>");
        return;
    } else if (strcmp("!", tk) == 0) {
        escreverArquivo("<NOT>");
        return;
    } else if (strcmp("=", tk) == 0) {
        escreverArquivo("<IGUAL>");
        return;
    } else if (strcmp("!=", tk) == 0) {
        escreverArquivo("<DIFF>");
        return;
    } else if (strcmp("<", tk) == 0) {
        escreverArquivo("<MENOR>");
        return;
    } else if (strcmp(">", tk) == 0) {
        escreverArquivo("<MAIOR>");
        return;
    }
    // NUMEROS -> num
    char number[1024];
    //Limpa area de memoria para concatenacao de numeros
    memset(number, 0, 1024);
    int index = 0;
    int totalTimesCountNumber = 0;
    int totalTimesIterate = 0;
    int i;
    for (i = 0; tk[i] != '\0'; i++) {
        if (tk[i] == '0') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '1') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '2') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '3') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '4') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '5') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '6') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '7') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '8') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        } else if (tk[i] == '9') {
            number[index++] = tk[i];
            totalTimesCountNumber++;
        }
        totalTimesIterate++;
    }
    if (totalTimesCountNumber == totalTimesIterate) {
        char n[1024];
        strcpy(n, "<NUM,");
        strcat(n, number);
        strcat(n, ">");
        escreverArquivo(n);
        //identificou numero, portanto, deve parar execucao
        return;
    }
    // VARIAVEIS -> id
    bool primeiraLeitura = true;
    char variavel[1024];
    index = 0;
    memset(variavel, 0, 1024);
    for (i = 0; tk[i] != '\0'; i++) {
        // Valida o caracter como variavel, inicido com letra e seguido de letra, ou numero ou underline (_)
        if (tk[i] == 'a' || tk[i] == 'A' || tk[i] == 'b' || tk[i] == 'B' || tk[i] == 'c' || tk[i] == 'C'
            || tk[i] == 'd' || tk[i] == 'D' || tk[i] == 'e' || tk[i] == 'E' || tk[i] == 'f' || tk[i] == 'F'
            || tk[i] == 'g' || tk[i] == 'G' || tk[i] == 'h' || tk[i] == 'H' || tk[i] == 'i' || tk[i] == 'I'
            || tk[i] == 'j' || tk[i] == 'J' || tk[i] == 'k' || tk[i] == 'K' || tk[i] == 'l' || tk[i] == 'L'
            || tk[i] == 'm' || tk[i] == 'M' || tk[i] == 'n' || tk[i] == 'N' || tk[i] == 'o' || tk[i] == 'O'
            || tk[i] == 'p' || tk[i] == 'P' || tk[i] == 'q' || tk[i] == 'Q' || tk[i] == 'r' || tk[i] == 'R'
            || tk[i] == 's' || tk[i] == 'S' || tk[i] == 't' || tk[i] == 'T' || tk[i] == 'u' || tk[i] == 'U'
            || tk[i] == 'v' || tk[i] == 'V' || tk[i] == 'w' || tk[i] == 'W' || tk[i] == 'x' || tk[i] == 'X'
            || tk[i] == 'y' || tk[i] == 'Y' || tk[i] == 'z' || tk[i] == 'Z') {
            primeiraLeitura = false;
            variavel[index++] = tk[i];
        }
            // Valida o caracter como numero ou underline (_)
        else if (tk[i] == '0' || tk[i] == '1' || tk[i] == '2'
                 || tk[i] == '3' || tk[i] == '4' || tk[i] == '5'
                 || tk[i] == '6' || tk[i] == '7' || tk[i] == '8'
                 || tk[i] == '9' || tk[i] == '_') {
            // Se iniciar com numero ou underline e um erro
            if (primeiraLeitura == true) {
                printf("ERRO LEXICO: %s\n", tk);
                exit(-1);
            }
            primeiraLeitura = false;
            variavel[index++] = tk[i];
        }
            // Caso nao seja aceito por nenhuma das condicoes devo rejeitar e encerrar a execucao
        else {
            printf("ERRO LEXICO: %s\n", tk);
            exit(-1);
        }
    }
    char n[1024];
    int idIndex =addStructID(variavel);
    char idIndexString[2];
    sprintf(idIndexString, "%d", idIndex);
    strcpy(n, "<ID,");
    strcat(n, idIndexString);
    strcat(n, ">");
    escreverArquivo(n);
    return;
}


//---------------------------------------------------------------------------------------------------------------------
//Analisador Sintatico
//---------------------------------------------------------------------------------------------------------------------

/**
 * Executa o fluxo de analise sintatica
 */
void analiseSintatica(void) {
    lerArquivoToken();
    //termino da leitura e manipulacao dos tokens
    char *vetorString;
    vetorString = strtok(stringSintatic, " ");
    while (vetorString != NULL) {
        guardarReferenciaTokens(separarIdOuValorDeTokens(vetorString, 0), separarIdOuValorDeTokens(vetorString, 1));
        vetorString = strtok(NULL, " ");
    }
    if (isAnaliseSintaticaWithoutError()) {
        printf("ANALISE SINTATICA ***** OK **** \n");
    } else {
        printf("PROBLEMA AO REALIZAR ANALISE SINTATICA **** ERROR **** \n");
        while (popLog()) {
            NULL;
        }
        exit(-1);
    }
}

/**
 * Le o arquivo de nome e extensao passados como parametro
 */
void lerArquivoToken(void) {
    //leitura do arquivo de tokens
    FILE *fp;
    char readCharacter;
    fp = fopen("outTokens.txt", "r");
    int i = 0;
    while (true) {
        readCharacter = (char) fgetc(fp);
        if (feof(fp)) {
            break;
        }
        if (readCharacter != '\n') {
            stringSintatic[i] = readCharacter;
            stringSintatic[i + 1] = '\0';
        } else {
            strcat(stringSintatic, " ");
        }
        i++;
    }
    // Apos leitura, limpa o arquivo de tokens
    fp = fopen(tokenFile, "w+");
    fclose(fp);
}

/**
 * Funcao que popula as matrizes de tokens com o indice e valor correspondente de cada token
 *
 * @param idToken
 * @param valorToken
 */
void guardarReferenciaTokens(char *idToken, char *valorToken) {
    strcpy(matrizIdToken[indiceTokens], idToken);
    strcpy(matrizValorToken[indiceTokens], valorToken);
    indiceTokens++;
}

/**
 * Funcao que separa tokens de seus ids ou valores dependendo do tipo de retorno pedido.
 *
 * @param vetorTokens
 * @param tipoDeRetorno
 * @return vetor contendo ids de tokens caso o tipoDeRetorno seja 0 ou vetor contendo o valor dos tokens caso o tipo de retorno seja 1.
 */
char *separarIdOuValorDeTokens(char *vetorTokens, int tipoDeRetorno) {
    char *vetorTokenIds = malloc(30);
    char *vetorTokenValues = malloc(30);
    bool antesDaVirgula = true;
    int i, j;
    for (i = 0, j = 0; vetorTokens[i] != '\0'; i++) {
        if (vetorTokens[i] == '<') {
            continue;
        } else if (vetorTokens[i] == ',') {
            antesDaVirgula = false;
            continue;
        } else if (vetorTokens[i] == '>') {
            continue;
        } else {
            if (antesDaVirgula == true) {
                vetorTokenIds[j] = vetorTokens[i];
                vetorTokenIds[j + 1] = '\0';
            } else {
                vetorTokenValues[j] = vetorTokens[i];
                vetorTokenValues[j + 1] = '\0';
            }
            j++;
        }
    }
    if (tipoDeRetorno == 0) {
        return vetorTokenIds;
    } else {
        return vetorTokenValues;
    }
}

/**
 * Inicia a verificacao das sequencias de tokens (id e/ou valores) de acordo com a especificacao do mini java
 */
bool isAnaliseSintaticaWithoutError(void) {
    nonTerminalStart();
    if (isMainDeClasse()) {
        if (isClasse()) {
            return nonTerminalAccept();
        }
    } else {
        pushLog("[PROG]", "isClasse()");
        return nonTerminalRefuse();
    }
    if (isMainDeClasse()) {
        return nonTerminalAccept();
    } else {
        pushLog("[PROG]", "isMainDeClasse()");
        return nonTerminalRefuse();
    }
}

/**
 * Empilha o nao terminal que inicia a analise sintatica
 */
void nonTerminalStart(void) {
    empilha(pilha[indicePilha].atual, pilha[indicePilha].atual);
}

/**
 * Desempilha um nao terminal e refaz a pilha
 */
void nonTerminalError(void) {
    desempilha();
    empilha(pilha[indicePilha].atual, pilha[indicePilha].atual);
}

/**
 * Desempilha e recusa a sintaxe
 */
bool nonTerminalRefuse(void) {
    desempilha();
    return false;
}

/**
 * Aceita o nao terminal
 */
bool nonTerminalAccept(void) {
    int novaPosicaoAtual = getCurrentPosOnStack();
    desempilha();
    update(novaPosicaoAtual);
    return true;
}

/**
 * Printa no console os valores recebidos e esperados para os tokens de acordo com a especificacao do mini java
 */
void pushLog(char *funcao, char *esperado) {
    char mensagemFinal[2048];
    strcpy(mensagemFinal, funcao);
    strcat(mensagemFinal, " ESPERADO ");
    strcat(mensagemFinal, esperado);
    strcat(mensagemFinal, ". RECEBIDO ");
    strcat(mensagemFinal, matrizIdToken[getCurrentPosOnStack()]);
    strcpy(pilhaLog[indicePilhaLog].mensagem, mensagemFinal);
    pilhaLog[indicePilhaLog].atual = getCurrentPosOnStack();
    indicePilhaLog++;
}

/**
 * Apresenta uma mensagem da pilha de log, que guarda mensagens de erro da analise sintatica
 */
bool popLog(void) {
    if (indicePilhaLog == 1) {
        return false;
    }
    char currentTempString[30];
    sprintf(currentTempString, "%d", pilhaLog[indicePilhaLog].atual);

    char mensagemFinal[2048];
    strcpy(mensagemFinal, "POSICAO ");
    strcat(mensagemFinal, currentTempString);
    strcat(mensagemFinal, ": ");
    strcat(mensagemFinal, pilhaLog[indicePilhaLog].mensagem);
    indicePilhaLog--;
    printf("%s\n", mensagemFinal);
    return true;
}

/**
 * Funco para comparar o valor do TOKEN com o valor esperado retornando TRUE caso sejam iguais ou false caso contrario
 */
bool match(char *tk, char *word) {
    if (strcmp(tk, word) == 0) {
        return true;
    } else {
        return false;
    }
}

/**
 * Atualiza o valor da pilha com um novo valor
 */
void update(int newvalue) {
    pilha[indicePilha].atual = newvalue;
}

/**
 * Incrementa a pilha
 */
void increment(void) {
    pilha[indicePilha].atual = pilha[indicePilha].atual + 1;

}

/**
 * Empilha um valor novo na pilha
 */
void empilha(int vstart, int vcurrent) {
    indicePilha++;
    pilha[indicePilha].inicio = vstart;
    pilha[indicePilha].atual = vcurrent;
}

/**
 * Desempilha
 */
void desempilha(void) {
    if (indicePilha < 0) {
        printf("ERRO AO DESEMPILHAR \n");
        exit(-1);
    }
    indicePilha--;
}

/**
 * Obtem a posicao atual da pilha
 */
int getCurrentPosOnStack(void) {
    return pilha[indicePilha].atual;
}

/**
 * Obtem o proximo valor da matriz de tokens (id ou valor)
 */
bool pegarProximo(char *word) {
    if (match(matrizIdToken[getCurrentPosOnStack()], word)) {
        printf("LIDO: %s\n", matrizIdToken[getCurrentPosOnStack()]);
        increment();
        return true;
    } else {
        printf("ESPERADO: %s --------- LIDO %s\n", word, matrizIdToken[getCurrentPosOnStack()]);
        return false;
    }
}

int pegarAnteior() {
    return getCurrentPosOnStack()-1;
}

/**
 * Valida se a sequencia de tokens representa parametros de funcao ou procedimento
 */
bool isParametro(int idxLexemaNomeMetodo) {
    nonTerminalStart();
    if (pegarProximo("INT")) {
        int idxLexema = salvarTokenIdEmLexema(getCurrentPosOnStack());
        if (pegarProximo("ID")) {
            setStructIDType('i', idxLexema);
            setStrucIDMethodInputType('i', idxLexemaNomeMetodo);
            return nonTerminalAccept();
        } else {
            pushLog("[isParametro]", "ID");
            nonTerminalError();
            return false;
        }
    } else if (pegarProximo("BOOLEAN")) {
        int idxLexema = salvarTokenIdEmLexema(getCurrentPosOnStack());
        if (pegarProximo("ID")) {
            setStructIDType('b', idxLexema);
            setStrucIDMethodInputType('b', idxLexemaNomeMetodo);
            return nonTerminalAccept();
        } else {
            pushLog("[isParametro]", "ID");
            nonTerminalError();
        }
    }
    pushLog("[isParametro]", "INT ou BOOLEAN");
    nonTerminalError();
    return false;
}

/**
 * Valida se a sequencia de tokens representa declaracao de variaveis
 */
bool isVariavel(void) {
    nonTerminalStart();
    if (pegarProximo("INT")) {
        int idxLexema = salvarTokenIdEmLexema(getCurrentPosOnStack());
        if (pegarProximo("ID")) {
            if (pegarProximo("PONTOVIRGULA")) {
                setStructIDType('i',idxLexema);
                return nonTerminalAccept();
            } else {
                pushLog("[isVariavel]", "PONTOVIRGULA");
                nonTerminalError();
            }
        } else {
            pushLog("[isVariavel]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[isVariavel]", "INT");
        nonTerminalError();
    }
    if (pegarProximo("BOOLEAN")) {
        int idxLexema = salvarTokenIdEmLexema(getCurrentPosOnStack());
        if (pegarProximo("ID")) {
            if (pegarProximo("PONTOVIRGULA")) {
                setStructIDType('b',idxLexema);
                return nonTerminalAccept();
            } else {
                pushLog("[isVariavel]", "PONTOVIRGULA");
                nonTerminalError();
            }
        } else {
            pushLog("[isVariavel]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[isVariavel]", "BOOLEAN");
        nonTerminalError();
    }
    pushLog("[isVariavel]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa a declaracao de um metodo
 */
bool isDeclaracaoMetodo(void) {
    nonTerminalStart();
    if (pegarProximo("PUBLIC")) {
        if (pegarProximo("INT") || pegarProximo("BOOLEAN")) {
            char methodReturnType;
            if(isStringsEqual(getRecognizedTokenKeyByIndex(pegarAnteior()), "int")){
                methodReturnType='i';
            } else {
                methodReturnType='b';
            }
            int idxLexema= salvarTokenIdEmLexema(getCurrentPosOnStack());
            if (pegarProximo("ID")) {
                if (pegarProximo("ABRE_PARENTESES")) {
                    int firstTokenRead = pegarAnteior();
                    if (isParametro(idxLexema)) {
                        if (pegarProximo("FECHA_PARENTESES")) {
                            int lastTokenRead = pegarAnteior();
                            pushPilhaSemantica(firstTokenRead,lastTokenRead);
                            if (pegarProximo("ABRE_CHAVES")) {
                                METODO_VAR:
                                if (isVariavel()) {
                                    goto METODO_VAR;
                                } else {
                                    METODO_RETENTA_COMANDO:
                                    if (isFuncaoOuProcedimento()) {
                                        goto METODO_RETENTA_COMANDO;
                                    } else {
                                        if (pegarProximo("RETURN")) {
                                            if (isEquivalencia()) {
                                                if (pegarProximo("PONTOVIRGULA")) {
                                                    if (pegarProximo("FECHA_CHAVES")) {
                                                        setStructIDType(methodReturnType,idxLexema);
                                                        return nonTerminalAccept();
                                                    } else {
                                                        pushLog("[isDeclaracaoMetodo]", "FECHA_CHAVES");
                                                        nonTerminalError();
                                                    }
                                                } else {
                                                    pushLog("[isDeclaracaoMetodo]", "PONTOVIRGULA");
                                                    nonTerminalError();
                                                }
                                            } else {
                                                pushLog("[isDeclaracaoMetodo]", "<TENTATIVA EXP()>");
                                                nonTerminalError();
                                            }
                                        } else {
                                            pushLog("[isDeclaracaoMetodo]", "RETURN");
                                            nonTerminalError();
                                        }
                                    }
                                }
                            } else {
                                pushLog("[isDeclaracaoMetodo]", "FECHA_PARENTESES");
                                nonTerminalError();
                            }
                        } else {
                            pushLog("[isDeclaracaoMetodo]", "FECHA_PARENTESES");
                            nonTerminalError();
                        }
                    } else {
                        pushLog("[isDeclaracaoMetodo]", "isParametro()");
                        nonTerminalError();
                    }
                } else {
                    pushLog("[isDeclaracaoMetodo]", "ABRE_PARENTESES");
                    nonTerminalError();
                }
            } else {
                pushLog("[isDeclaracaoMetodo]", "ID");
                nonTerminalError();
            }
        } else {
            pushLog("[isDeclaracaoMetodo]", "INT ou BOOLEAN");
            nonTerminalError();
        }
    } else {
        pushLog("[isDeclaracaoMetodo]", "PUBLIC");
        nonTerminalError();
    }
    pushLog("[isDeclaracaoMetodo]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa a declaracao de uma classe
 */
bool isClasse(void) {
    nonTerminalStart();
    if (pegarProximo("CLASS")) {
        int idxLexema = salvarTokenIdEmLexema(getCurrentPosOnStack());
        if (pegarProximo("ID")) {
            if (pegarProximo("EXTENDS")) {
                if (pegarProximo("ID")) {
                    goto APOS_EXTENDS;
                } else {
                    pushLog("[isClasse]", "ID");
                    nonTerminalError();
                }
            } else {
                APOS_EXTENDS:
                if (pegarProximo("ABRE_CHAVES")) {
                    VAR_TESTE_AGAIN:
                    if (isVariavel()) {
                        goto VAR_TESTE_AGAIN;
                    } else {
                        if (isDeclaracaoMetodo()) {
                            if (pegarProximo("FECHA_CHAVES")) {
                                setStructIDType('c', idxLexema);
                                return nonTerminalAccept();
                            } else {
                                pushLog("[isClasse]", "FECHA_CHAVES");
                                nonTerminalError();
                            }
                        } else {
                            pushLog("[isClasse]", "<TENTATIVA isDeclaracaoMetodo()>");
                            nonTerminalError();
                        }
                    }
                } else {
                    pushLog("[isClasse]", "ABRE_CHAVES");
                    nonTerminalError();
                }
            }
        } else {
            pushLog("[isClasse]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[isClasse]", "CLASS");
        nonTerminalError();
    }

    pushLog("[isClasse]", "<PROBLEMA COM DERIVACAO>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa a chamada de uma funcao publica de uma classe
 */
bool isChamadaFuncao(void) {
    nonTerminalStart();
    if (pegarProximo("PONTO")) {
        if (pegarProximo("ID")) {
            int firstTokenRead = pegarAnteior();
            if (pegarProximo("ABRE_PARENTESES")) {
                if (isEquivalencia()) {
                    if (pegarProximo("FECHA_PARENTESES")) {
                        int lastTokenRead = pegarAnteior();
                        pushPilhaSemantica(firstTokenRead, lastTokenRead);
                        return nonTerminalAccept();
                    } else {
                        pushLog("[isChamadaFuncao]", "FECHA_PARENTESES");
                        nonTerminalError();
                    }
                } else {
                    pushLog("[isChamadaFuncao]", "<TENTATIVA EXP(9)>");
                    nonTerminalError();
                }
            } else {
                pushLog("[isChamadaFuncao]", "ABRE_PARENTESES");
                nonTerminalError();
            }
        } else {
            pushLog("[isChamadaFuncao]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[isChamadaFuncao]", "PONTO");
        nonTerminalError();
    }

    pushLog("[isChamadaFuncao]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa a chamada de uma funcao interna
 * Tambem trata chamadas recursivamente de ABRE_PARENTESES
 */
bool isInstanciaDeClasse(void) {
    nonTerminalStart();
    if (pegarProximo("ABRE_PARENTESES")) {
        if (isInstanciaDeClasse()) {
            if (pegarProximo("FECHA_PARENTESES")) {
                return nonTerminalAccept();
            } else {
                pushLog("[isInstanciaDeClasse]", "<FECHA_PARENTESES");
                nonTerminalError();
            }
        } else {
            pushLog("[isInstanciaDeClasse]", "<TENTATIVA isInstanciaDeClasse(99)>");
            nonTerminalError();
        }
    } else {
        pushLog("[isInstanciaDeClasse]", "ABRE_PARENTESES");
        nonTerminalError();
    }

    if (pegarProximo("ID")) {
        return nonTerminalAccept();
    } else {
        pushLog("[isInstanciaDeClasse]", "ID");
        nonTerminalError();
    }

    if (pegarProximo("THIS")) {
        if (isChamadaFuncao()) {
            return nonTerminalAccept();
        } else {
            pushLog("[isInstanciaDeClasse]", "isChamadaFuncao(2)");
            nonTerminalError();
        }
    } else {
        pushLog("[isInstanciaDeClasse]", "THIS");
        nonTerminalError();
    }
    if (pegarProximo("NEW")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("ABRE_PARENTESES")) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (isChamadaFuncao()) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[isInstanciaDeClasse]", "<TENTATIVA isChamadaFuncao()>");
                        nonTerminalError();
                    }
                } else {
                    pushLog("[isInstanciaDeClasse]", "FECHA_PARENTESES");
                    nonTerminalError();
                }
            } else {
                pushLog("[isInstanciaDeClasse]", "ABRE_PARENTESES");
                nonTerminalError();
            }
        } else {
            pushLog("[isInstanciaDeClasse]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[isInstanciaDeClasse]", "NEW");
        nonTerminalError();
    }

    if (pegarProximo("NEW")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("ABRE_PARENTESES")) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    return nonTerminalAccept();
                } else {
                    pushLog("[isInstanciaDeClasse]", "FECHA_PARENTESES");
                    nonTerminalError();
                }
            } else {
                pushLog("[isInstanciaDeClasse]", "ABRE_PARENTESES");
                nonTerminalError();
            }
        } else {
            pushLog("[isInstanciaDeClasse]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[isInstanciaDeClasse]", "NEW");
        nonTerminalError();
    }
    pushLog("[isInstanciaDeClasse]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa uma atribuicao de valor a uma variavel
 */
bool isAtribuicao(void) {
    nonTerminalStart();
    if (pegarProximo("TRUE")) {
        return nonTerminalAccept();
    } else {
        pushLog("[isAtribuicao]", "TRUE");
        nonTerminalError();
    }
    if (pegarProximo("FALSE")) {
        return nonTerminalAccept();
    } else {
        pushLog("[isAtribuicao]", "FALSE");
        nonTerminalError();
    }
    if (pegarProximo("NUM")) {
        return nonTerminalAccept();
    } else {
        pushLog("[isAtribuicao]", "NUM");
        nonTerminalError();
    }
    if (pegarProximo("NULL")) {
        return nonTerminalAccept();
    } else {
        pushLog("[isAtribuicao]", "NULL");
        nonTerminalError();
    }
    if (pegarProximo("NEW")) {
        if (pegarProximo("INT")) {
            if (pegarProximo("ABRE_COLCHETES")) {
                if (isEquivalencia()) {
                    if (pegarProximo("FECHA_COLCHETES")) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[isAtribuicao]", "FECHA_COLCHETES");
                        nonTerminalError();
                    }
                } else {
                    pushLog("[isAtribuicao]", "<TENTATIVA isEquivalencia(8)>");
                    nonTerminalError();
                }
            } else {
                pushLog("[isAtribuicao]", "ABRE_COLCHETES");
                nonTerminalError();
            }
        } else {
            pushLog("[isAtribuicao]", "INT");
            nonTerminalError();
        }
    } else {
        pushLog("[isAtribuicao]", "NEW");
        nonTerminalError();
    }
    pushLog("[isAtribuicao]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa uma multiplicacao
 */
bool isMultiplicacao(void) {
    nonTerminalStart();
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MULT")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                MEXP_GOTO:
                if (isAdicao()) {
                    goto MEXP_GOTO;
                }
                return nonTerminalAccept();
            } else {
                pushLog("[isMultiplicacao]", "[1] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[isMultiplicacao]", "MULT");
            nonTerminalError();
        }
    } else {
        pushLog("[isMultiplicacao]", "[2] ID ou NUM");
        nonTerminalError();
    }
    if (pegarProximo("ID")) {
        if (pegarProximo("MULT")) {
            if (isInstanciaDeClasse()) {
                return nonTerminalAccept();
            } else {
                pushLog("[isMultiplicacao]", "[1] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[isMultiplicacao]", "MULT");
            nonTerminalError();
        }
    } else {
        pushLog("[isMultiplicacao]", "[2] ID ou NUM");
        nonTerminalError();
    }
    pushLog("[isMultiplicacao]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa uma adicao de numeros
 */
bool isAdicao(void) {
    nonTerminalStart();
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MAIS")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                isAdicao_GOTO:
                if (isAdicao()) {
                    goto isAdicao_GOTO;
                }
                return nonTerminalAccept();
            } else {
                pushLog("[isAdicao-1]", "[1] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[isAdicao-1]", "MAIS");
            nonTerminalError();
        }
    } else {
        pushLog("[isAdicao-1]", "[2] ID ou NUM");
        nonTerminalError();
    }
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MENOS")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                isAdicao_GOTO2:
                if (isAdicao()) {
                    goto isAdicao_GOTO2;
                }
                return nonTerminalAccept();
            } else {
                pushLog("[isAdicao-2]", "[3] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[isAdicao-2]", "MENOS");
            nonTerminalError();
        }
    } else {
        pushLog("[isAdicao-2]", "[4] ID ou NUM");
        nonTerminalError();
    }
    if (pegarProximo("MENOS") || pegarProximo("MAIS")) {
        if (pegarProximo("ID") || pegarProximo("NUM")) {
            return nonTerminalAccept();
        } else {
            pushLog("[isAdicao-3]", "MINUS");
            nonTerminalError();
        }
    } else {
        pushLog("[isAdicao-3]", "[5] ID ou NUM");
        nonTerminalError();
    }
    pushLog("[isAdicao]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa uma subtracao de variaveis
 */
bool isSubtracao(void) {
    nonTerminalStart();
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MENOR")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[isSubtracao-1]", "[5] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[isSubtracao-1]", "MENOR");
            nonTerminalError();
        }
    } else {
        pushLog("[isSubtracao-1]", "[6] ID ou NUM");
        nonTerminalError();
    }
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MENOR")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[isSubtracao-2]", "[1] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[isSubtracao-2]", "MENOR");
            nonTerminalError();
        }
    } else {
        pushLog("[isSubtracao-2]", "[2] ID ou NUM");
        nonTerminalError();
    }

    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("DIFF")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[isSubtracao-3]", "[3] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[isSubtracao-3]", "MENOR");
            nonTerminalError();
        }
    } else {
        pushLog("[isSubtracao-3]", "[4] ID ou NUM");
        nonTerminalError();
    }

    pushLog("[isSubtracao]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa uma expressao de comparacao
 */
bool isEquivalencia(void) {
    nonTerminalStart();
    if (isSubtracao()) {
        return nonTerminalAccept();
    }
    if (isAdicao()) {
        return nonTerminalAccept();
    }
    if (isMultiplicacao()) {
        return nonTerminalAccept();
    }
    if (isAtribuicao()) {
        return nonTerminalAccept();
    }
    if (isInstanciaDeClasse()) {
        return nonTerminalAccept();
    }
    pushLog("[isEquivalencia]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa um comando
 */
bool isFuncaoOuProcedimento(void) {
    nonTerminalStart();
    if (pegarProximo("ABRE_CHAVES")) {
        if (isFuncaoOuProcedimento()) {
            COMANDO:
            if (isFuncaoOuProcedimento()) {
                goto COMANDO;
            }
            if (pegarProximo("FECHA_CHAVES")) {
                return nonTerminalAccept();
            } else {
                pushLog("[isFuncaoOuProcedimento-1]", "FECHA_CHAVES");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[isFuncaoOuProcedimento-1]", "<TENTATIVA isFuncaoOuProcedimento(1)>");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("IF")) {
        if (pegarProximo("ABRE_PARENTESES")) {
            if (isEquivalencia()) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (isFuncaoOuProcedimento()) {
                        if (pegarProximo("ELSE")) {
                            if (isFuncaoOuProcedimento()) {
                                return nonTerminalAccept();
                            } else {
                                pushLog("[isFuncaoOuProcedimento-2]", "<TENTATIVA CMD(2)>");
                                return nonTerminalRefuse();
                            }
                        } else {
                            pushLog("[isFuncaoOuProcedimento-2]", "ELSE");
                            return nonTerminalRefuse();
                        }
                    } else {
                        pushLog("[isFuncaoOuProcedimento-2]", "<TENTATIVA CMD(3)>");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[isFuncaoOuProcedimento-2]", "FECHA_PARENTESES");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[isFuncaoOuProcedimento-2]", "<TENTATIVA EXP(1)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[isFuncaoOuProcedimento-2]", "ABRE_PARENTESES");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("IF")) {
        if (pegarProximo("ABRE_PARENTESES")) {
            if (isEquivalencia()) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (isFuncaoOuProcedimento()) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[isFuncaoOuProcedimento-3]", "<TENTATIVA isFuncaoOuProcedimento(4)>");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[isFuncaoOuProcedimento-3]", "FECHA_PARENTESES");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[isFuncaoOuProcedimento-3]", "<TENTATIVA isEquivalencia(2)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[isFuncaoOuProcedimento-3]", "ABRE_PARENTESES");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("WHILE")) {
        if (pegarProximo("ABRE_PARENTESES")) {
            if (isEquivalencia()) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (isFuncaoOuProcedimento()) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[isFuncaoOuProcedimento-4]", "<TENTATIVA isFuncaoOuProcedimento(5)>");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[isFuncaoOuProcedimento-4]", "FECHA_PARENTESES");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[isFuncaoOuProcedimento-4]", "<TENTATIVA isEquivalencia(3)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[isFuncaoOuProcedimento-4]", "ABRE_PARENTESES");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("SYSOUTPRINTLN")) {
        if (pegarProximo("ABRE_PARENTESES")) {
            if (isEquivalencia()) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (pegarProximo("PONTOVIRGULA")) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[isFuncaoOuProcedimento-5]", "PONTOVIRGULA");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[isFuncaoOuProcedimento-5]", "FECHA_PARENTESES");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[isFuncaoOuProcedimento-5]", "<TENTATIVA isEquivalencia(4)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[isFuncaoOuProcedimento-5]", "ABRE_PARENTESES");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("ID")) {
        int firstTokenRead = pegarAnteior();
        if (pegarProximo("IGUAL")) {
            if (isEquivalencia()) {
                if (pegarProximo("PONTOVIRGULA")) {
                    int lastTokenRead = pegarAnteior();
                    pushPilhaSemantica(firstTokenRead, lastTokenRead);
                    return nonTerminalAccept();
                } else {
                    pushLog("[isFuncaoOuProcedimento-6]", "PONTOVIRGULA");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[isFuncaoOuProcedimento-6]", "<TENTATIVA isEquivalencia(5)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[isFuncaoOuProcedimento-6]", "IGUAL");
            return nonTerminalRefuse();
        }
    }
    pushLog("[isFuncaoOuProcedimento]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

/**
 * Valida se a sequencia de tokens representa a declaracao de uma main
 */
bool isMainDeClasse(void) {
    nonTerminalStart();
    if (pegarProximo("CLASS")) {
        int idxLexema = salvarTokenIdEmLexema(getCurrentPosOnStack());
        if (pegarProximo("ID")) {
            if (pegarProximo("ABRE_CHAVES")) {
                if (pegarProximo("PUBLIC")) {
                    if (pegarProximo("STATIC")) {
                        if (pegarProximo("VOID")) {
                            if (pegarProximo("MAIN")) {
                                if (pegarProximo("ABRE_PARENTESES")) {
                                    if (pegarProximo("ID")) {
                                        if (pegarProximo("ABRE_COLCHETE")) {
                                            if (pegarProximo("FECHA_COLCHETE")) {
                                                if (pegarProximo("ID")) {
                                                    if (pegarProximo("FECHA_PARENTESES")) {
                                                        if (pegarProximo("ABRE_CHAVES")) {
                                                            if (isFuncaoOuProcedimento()) {
                                                                if (pegarProximo("FECHA_CHAVES")) {
                                                                    if (pegarProximo("FECHA_CHAVES")) {
                                                                        setStructIDType('c', idxLexema);
                                                                        return nonTerminalAccept();
                                                                    } else {
                                                                        pushLog("[isMainDeClasse]", "FECHA_CHAVES");
                                                                        return nonTerminalRefuse();
                                                                    }
                                                                } else {
                                                                    pushLog("[isMainDeClasse]", "FECHA_CHAVES");
                                                                    return nonTerminalRefuse();
                                                                }
                                                            } else {
                                                                pushLog("[isMainDeClasse]", "CMD()");
                                                                return nonTerminalRefuse();
                                                            }
                                                        } else {
                                                            pushLog("[isMainDeClasse]", "ABRE_CHAVES");
                                                            return nonTerminalRefuse();
                                                        }
                                                    } else {
                                                        pushLog("[isMainDeClasse]", "FECHA_PARENTESES");
                                                        return nonTerminalRefuse();
                                                    }
                                                } else {
                                                    pushLog("[isMainDeClasse]", "ID");
                                                    return nonTerminalRefuse();
                                                }
                                            } else {
                                                pushLog("[isMainDeClasse]", "FECHA_COLCHETE");
                                                return nonTerminalRefuse();
                                            }
                                        } else {
                                            pushLog("[isMainDeClasse]", "ABRE_COLCHETE");
                                            return nonTerminalRefuse();
                                        }
                                    } else {
                                        pushLog("[isMainDeClasse]", "ID");
                                        return nonTerminalRefuse();
                                    }
                                } else {
                                    pushLog("[isMainDeClasse]", "ABRE_PARENTESES");
                                    return nonTerminalRefuse();
                                }
                            } else {
                                pushLog("[isMainDeClasse]", "isMainDeClasse");
                                return nonTerminalRefuse();
                            }
                        } else {
                            pushLog("[isMainDeClasse]", "VOID");
                            return nonTerminalRefuse();
                        }
                    } else {
                        pushLog("[isMainDeClasse]", "STATIC");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[isMainDeClasse]", "PUBLIC");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[isMainDeClasse]", "ABRE_CHAVES");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[isMainDeClasse]", "ID");
            return nonTerminalRefuse();
        }
    } else {
        pushLog("[isMainDeClasse]", "CLASS");
        return nonTerminalRefuse();
    }
}

//---------------------------------------------------------------------------------------------------------------------
//Analisador Semantico
//---------------------------------------------------------------------------------------------------------------------

const char* getRecognizedTokenValueByIndex(int idx){
    return matrizValorToken[idx];
}

const char* getRecognizedTokenKeyByIndex(int idx){
    return matrizIdToken[idx];
}

bool isStringsEqual(const char *string1, char *string2) {
    if (strcmp(string1, string2) == 0) {
        return true;
    } else {
        return false;
    }
}

int addStructID(char *id) {
    int i = 0;
    for (i = 0; i < structID.idx; i++) {
        if (strcmp(structID.ID[i], id) == 0) {
            return i;
        }
    }
    strcpy(structID.ID[structID.idx], id);
    i = structID.idx;
    structID.idx++;
    return i;
}

void setStructIDType(char vtype, int idx) {
    structID.type[idx] = vtype;
}

void setStrucIDMethodInputType(char vInputType, int idx) {
    structID.methodInputType[idx] = vInputType;
}

char getStructIDType(int idx) {
    return structID.type[idx];
}

char getStructIDMethodInputType(int idx) {
    return structID.methodInputType[idx];
}

char *getStructIDName(int idx) {
    return structID.ID[idx];
}

int salvarTokenIdEmLexema(int idxTokenPosition) {
    char idString[30];
    strcpy(idString, getRecognizedTokenValueByIndex(idxTokenPosition));
    int idStringConvertedtoInt = atoi(idString);
    return idStringConvertedtoInt;
}

void printStructID() {
    printf("Indice                   Lexema  Tipo  Metodo \n");
    printf("_______________________________________________________________\n");
    int i = 0;
    for (i = 0; i < structID.idx; i++) {
        printf("%2d %29s %3c %3c \n", i, getStructIDName(i), getStructIDType(i), getStructIDMethodInputType(i));
    }
    printf("\n");
}

void pushPilhaSemantica(int indiceTokenComeco, int indiceTokenFinal) {
    pilhaSemantica.idxTokenStart[pilhaSemantica.idx] = indiceTokenComeco;
    pilhaSemantica.idxTokenEnd[pilhaSemantica.idx] = indiceTokenFinal;
    pilhaSemantica.idx++;
}

bool regraAritmetica(int primeiroToken, int ultimoToken) {
    int i;
    char firstType = getStructIDType(salvarTokenIdEmLexema(primeiroToken));
    for (i = primeiroToken + 1; i <= ultimoToken; i++) {
        char TOKENKEY[30];
        strcpy(TOKENKEY, getRecognizedTokenKeyByIndex(i));
        printf("==> %s  \n",TOKENKEY);
        if (isStringsEqual(TOKENKEY, "id")) {
            printf("LEXEMA %d\n", salvarTokenIdEmLexema(i));
            char laterType = getStructIDType(salvarTokenIdEmLexema(i));
            if (firstType != laterType) {
                printf("%c | %c\n",firstType, laterType);
                printf("%s | %s\n",getStructIDName(salvarTokenIdEmLexema(primeiroToken)), getStructIDName(salvarTokenIdEmLexema(i)));
                printf("%d | %d\n", primeiroToken, i);
                return false;
            }
        } else {
            continue;
        }
    }
    return true;

}

bool regraParametro(int primeiroToken, int ultimoToken) {
    int i;
    char methodInputType = getStructIDMethodInputType(salvarTokenIdEmLexema(primeiroToken));
    for (i = primeiroToken + 1; i <= ultimoToken; i++) {
        char TOKENKEY[30];
        strcpy(TOKENKEY, getRecognizedTokenKeyByIndex(i));
        if (isStringsEqual(TOKENKEY, "id")) {
            char laterType = getStructIDType(salvarTokenIdEmLexema(i));
            if (methodInputType != laterType) {
                return false;
            }
        } else {
            continue;
        }
    }
    return true;
}

bool validarSemantica(int primeiroToken, int ultimoToken) {
    if (regraAritmetica(primeiroToken, ultimoToken)) {
        return true;
    }
    if (regraParametro(primeiroToken, ultimoToken)) {
        return true;
    }
    printf("Regras semanticas não validadas \n");
    return false;
}

void analiseSemantica(void) {
    char declaracaoRefeita[2056];
    memset(declaracaoRefeita, 0, 2056);
    int i;
    for (i = 0; i < pilhaSemantica.idx; i++) {
        if (!validarSemantica(pilhaSemantica.idxTokenStart[i], pilhaSemantica.idxTokenEnd[i])) {
            printf("Declaraçao refeita:\n%s\n", declaracaoRefeita);
            printf("----------- ERRO SEMANTICO \n");
            strcat(declaracaoRefeita, " ");
            strcat(declaracaoRefeita, getRecognizedTokenKeyByIndex(i));
            exit(-1);
        }
    }
    printStructID();
    printf("----------- Analise Semantica sem erros. \n");
}
