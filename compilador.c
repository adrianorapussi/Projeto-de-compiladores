//
// Created by Adriano Rapussi
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * -----------------------------------------------------------------------
 * Funçoes usadas
 * -----------------------------------------------------------------------
 */

void analisadorLexico();
int stringsIdentificadas(char *id);
void removerLinhaComentario();
void escreverArquivo(char *tk);
void lerArquivo(char *nomeDoArquivo);
void geradorTokens(char *tk);
void analisadorSintatico();
void guardarReferenciaTokens(char *key, char *value);
const char* separarIdOuValorDeTokens(char *vetorTokens, int tipoDeRetorno);
void fazerAnaliseSintatica();
bool isAnaliseSintaticaWithoutError();
void nonTerminalStart();
void nonTerminalError();
bool nonTerminalRefuse();
bool nonTerminalAccept();
void pushLog(char *funcao, char *esperado);
bool popLog();
bool match(char *tk, char *word);
void update(int newvalue);
void increment();
void empilha(int vstart, int vcurrent);
void desempilha();
int getCurrentPosOnStack();
bool pegarProximo(char *word);
bool PARAMS();
bool VAR();
bool METODO();
bool CLASSE();
bool PEXP2();
bool PEXP();
int SEXP();
bool MEXP();
bool AEXP();
bool REXP();
bool EXP();
bool CMD();
bool MAIN();

/*
 * -------------------------------------------------------------------------------------------------------------------
 * Constantes
 * -------------------------------------------------------------------------------------------------------------------
 */
#define BUFFER 2048
char stringLexical[BUFFER];
char stringSintatic[BUFFER];
char vetorStringsIdentificadas[128][30];
int indiceVetorStringsIdentificadas = 0;
static const char tokenFile[] = "outTokens.txt";
static const char programFile[] = "programFile.txt";
char matrizIdToken[2048][30];
char matrizValorToken[2048][30];
int  indiceTokens=0;

int indicePilha = 0;

struct Pilha {
    int inicio;
    int atual;
};
struct Pilha pilha[128];

int indicePilhaLog = 0;

struct PilhaLog {
    char mensagem[2048];
    int atual;
};
struct PilhaLog pilhaLog[128];

/**
 * -------------------------------------------------------------------------------------------------------------------
 * Funçao principal
 * -------------------------------------------------------------------------------------------------------------------
 */
int main() {
    analisadorLexico();
    analisadorSintatico();
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------
//Analisador Léxico
//---------------------------------------------------------------------------------------------------------------------

/**
 * Executa o fluxo de análise léxica
 */
void analisadorLexico(void) {
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
 * Funçao que remove as linhas de comentario do codigo
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
 * Função que grava a string recebida em um arquivo output de nome representado pela variavel TOKEN_FILE
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
 * Função que realiza a leitura do arquivo leitura.txt e armazena o conteudo do arquivo em um buffer
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
        } else if (ignorarLinha == true && linhaCaracter == '\n') {
            //quando encontrar \n com ignorarlinha ligado concatena espaço
            strcat(stringLexical, " ");
            ignorarLinha = false;
        } else if (ignorarLinha == true && linhaCaracter != '\n') {
            //Caracteres que devem ser ignorados
            continue;
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
    //Limpa area de memoria para concatenaçao de numeros
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
        // Caso nao seja aceito por nenhuma das condiçoes devo rejeitar e encerrar a execuçao
        else {
            printf("ERRO LEXICO: %s\n", tk);
            exit(-1);
        }
    }
    char n[1024];
    int idIndex = stringsIdentificadas(variavel);
    char idIndexString[2];
    sprintf(idIndexString, "%d", idIndex);
    strcpy(n, "<ID,");
    strcat(n, idIndexString);
    strcat(n, ">");
    escreverArquivo(n);
    return;
}

//---------------------------------------------------------------------------------------------------------------------
//Análisador Sintático
//_____________________________________________________________________________________________________________________

/**
 * Executa o fluxo de análise sintática
 */
void analisadorSintatico(void) {
    //leitura do arquivo de tokens
    FILE *fp;
    char readCharacter;
    fp = fopen(tokenFile, "r");
    while(true) {
        readCharacter = (char) fgetc(fp);
        if(feof(fp)){
            break;
        }
        if(readCharacter != '\n' ) {
            stringSintatic[strlen(stringSintatic)] = readCharacter;
            stringSintatic[strlen(stringSintatic)+1] = '\0';
        } else {
            strcat(stringSintatic, " ");
        }
    }
    fclose(fp);
    //termino da leitura e manipulação dos tokens
    char *vetorString;
    vetorString = strtok(stringSintatic," ");
    while (vetorString != NULL){
        guardarReferenciaTokens(separarIdOuValorDeTokens(vetorString, 0), separarIdOuValorDeTokens(vetorString, 1));
        vetorString = strtok(NULL, " ");
    }
    fazerAnaliseSintatica();
}

/**
 * Função que popula as matrizes de tokens com o indice e valor correspondente de cada token
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
 * Função que separa tokens de seus ids ou valores dependendo do tipo de retorno pedido.
 *
 * @param vetorTokens
 * @param tipoDeRetorno
 * @return vetor contendo ids de tokens caso o tipoDeRetorno seja 0 ou vetor contendo o valor dos tokens caso o tipo de
 * retorno seja 1.
 */
const char* separarIdOuValorDeTokens(char *vetorTokens, int tipoDeRetorno) {
    char *vetorTokenIds = malloc(30);
    char *vetorTokenValues = malloc(30);
    bool antesDaVirgula = true;
    int i=0;
    for (i=0; vetorTokens[i] != '\0' ; i++) {
        if (vetorTokens[i] == '<') {
            continue;
        } else if (vetorTokens[i] == ',') {
            antesDaVirgula = false;
            continue;
        } else if (vetorTokens[i] == '>') {
            continue;
        } else {
            if (antesDaVirgula == true) {
                vetorTokenIds[strlen(vetorTokenIds)] = vetorTokens[i];
                vetorTokenIds[strlen(vetorTokenIds)+1] = '\0';
            } else {
                vetorTokenValues[strlen(vetorTokenValues)] = vetorTokens[i];
                vetorTokenValues[strlen(vetorTokenValues)+1] = '\0';
            }
        }
    }
    if (tipoDeRetorno == 0) {
        return vetorTokenIds;
    } else if (tipoDeRetorno == 1) {
        return vetorTokenValues;
    }
}

void fazerAnaliseSintatica() {
    if (isAnaliseSintaticaWithoutError()) {
        printf("Análise sintática concluída sem erros \n");
    } else {
        printf("Problema ao realizar análise sintática \n");
        while (popLog()) {
            NULL;
        }
    }
}

bool isAnaliseSintaticaWithoutError() {
    nonTerminalStart();

    if (MAIN()) {
        if (CLASSE()) {
            return nonTerminalAccept();
        }
    } else {
        pushLog("[PROG]", "CLASSE()");
        return nonTerminalRefuse();
    }

    if (MAIN()) {
        return nonTerminalAccept();
    } else {
        pushLog("[PROG]", "MAIN()");
        return nonTerminalRefuse();
    }
}

void nonTerminalStart() {
    empilha(pilha[indicePilha].atual, pilha[indicePilha].atual);
}

void nonTerminalError() {
    desempilha();
    empilha(pilha[indicePilha].atual, pilha[indicePilha].atual);
}

bool nonTerminalRefuse() {
    desempilha();
    return false;
}

bool nonTerminalAccept() {
    int newCurrentPosition = getCurrentPosOnStack();
    desempilha();
    update(newCurrentPosition);
    return true;
}

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

bool popLog() {
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

bool match(char *tk, char *word) {
    if (strcmp(tk, word) == 0) {
        return true;
    } else {
        return false;
    }
}

void update(int newvalue) {
    pilha[indicePilha].atual = newvalue;
}

void increment() {
    pilha[indicePilha].atual = pilha[indicePilha].atual + 1;

}

void empilha(int vstart, int vcurrent) {
    indicePilha++;
    printf("empilha \n\tstart: %d\n\tcurrent: %d\n\tidxStack: %d\n", vstart, vcurrent, indicePilha);
    pilha[indicePilha].inicio = vstart;
    pilha[indicePilha].atual = vcurrent;
}

void desempilha() {
    if (indicePilha < 0) {
        printf("ERRO AO DESEMPILHAR\n");
        exit(-1);
    }
    indicePilha--;
}

int getCurrentPosOnStack() {
    return pilha[indicePilha].atual;
}

bool pegarProximo(char *word) {
    if (match(matrizIdToken[getCurrentPosOnStack()], word)) {
        printf("LIDO: %s\n", matrizIdToken[getCurrentPosOnStack()]);
        increment();
        return true;
    } else {
        printf("ESPERADO: %s | LIDO %s\n", word, matrizIdToken[getCurrentPosOnStack()]);
        return false;
    }
}

bool PARAMS() {
    nonTerminalStart();
    if (pegarProximo("INT") || pegarProximo("BOOLEAN")) {
        if (pegarProximo("ID")) {
            return nonTerminalAccept();
        } else {
            pushLog("[PARAMS]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[PARAMS]", "INT ou BOOLEAN");
        nonTerminalError();
    }
}

bool VAR() {
    nonTerminalStart();
    if (pegarProximo("INT")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("PONTOVIRGULA")) {
                return nonTerminalAccept();
            } else {
                pushLog("[VAR]", "PONTOVIRGULA");
                nonTerminalError();
            }
        } else {
            pushLog("[VAR]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[VAR]", "INT");
        nonTerminalError();
    }
    if (pegarProximo("BOOLEAN")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("PONTOVIRGULA")) {
                return nonTerminalAccept();
            } else {
                pushLog("[VAR]", "PONTOVIRGULA");
                nonTerminalError();
            }
        } else {
            pushLog("[VAR]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[VAR]", "BOOLEAN");
        nonTerminalError();
    }

    pushLog("[VAR]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool METODO() {
    nonTerminalStart();
    if (pegarProximo("PUBLIC")) {
        if (pegarProximo("INT") || pegarProximo("BOOLEAN")) {
            if (pegarProximo("ID")) {
                if (pegarProximo("ABRE_PARENTESES")) {
                    if (PARAMS()) {
                        if (pegarProximo("FECHA_PARENTESES")) {
                            if (pegarProximo("ABRE_CHAVES")) {
                                METODO_VAR:
                                if (VAR()) {
                                    goto METODO_VAR;
                                } else {
                                    METODO_RETENTA_COMANDO:
                                    if (CMD()) {
                                        goto METODO_RETENTA_COMANDO;
                                    } else {
                                        if (pegarProximo("RETURN")) {
                                            if (EXP()) {
                                                if (pegarProximo("PONTOVIRGULA")) {
                                                    if (pegarProximo("FECHA_CHAVES")) {
                                                        return nonTerminalAccept();
                                                    } else {
                                                        pushLog("[METODO]", "FECHA_CHAVES");
                                                        nonTerminalError();
                                                    }
                                                } else {
                                                    pushLog("[METODO]", "PONTOVIRGULA");
                                                    nonTerminalError();
                                                }
                                            } else {
                                                pushLog("[METODO]", "<TENTATIVA EXP()>");
                                                nonTerminalError();
                                            }
                                        } else {
                                            pushLog("[METODO]", "RETURN");
                                            nonTerminalError();
                                        }
                                    }
                                }
                            } else {
                                pushLog("[METODO]", "FECHA_PARENTESES");
                                nonTerminalError();
                            }
                        } else {
                            pushLog("[METODO]", "FECHA_PARENTESES");
                            nonTerminalError();
                        }
                    } else {
                        pushLog("[METODO]", "PARAMS()");
                        nonTerminalError();
                    }
                } else {
                    pushLog("[METODO]", "ABRE_PARENTESES");
                    nonTerminalError();
                }
            } else {
                pushLog("[METODO]", "ID");
                nonTerminalError();
            }
        } else {
            pushLog("[METODO]", "INT ou BOOLEAN");
            nonTerminalError();
        }
    } else {
        pushLog("[METODO]", "PUBLIC");
        nonTerminalError();
    }
    pushLog("[METODO]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool CLASSE() {
    nonTerminalStart();
    if (pegarProximo("CLASS")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("EXTENDS")) {
                if (pegarProximo("ID")) {
                    goto APOS_EXTENDS;
                } else {
                    pushLog("[CLASSE]", "ID");
                    nonTerminalError();
                }
            } else {
                APOS_EXTENDS:
                if (pegarProximo("ABRE_CHAVES")) {
                    VAR_TESTE_AGAIN:
                    if (VAR()) {
                        goto VAR_TESTE_AGAIN;
                    } else {
                        if (METODO()) {
                            if (pegarProximo("FECHA_CHAVES")) {
                                return nonTerminalAccept();
                            } else {
                                pushLog("[CLASSE]", "FECHA_CHAVES");
                                nonTerminalError();
                            }
                        } else {
                            pushLog("[CLASSE]", "<TENTATIVA METODO()>");
                            nonTerminalError();
                        }
                    }
                } else {
                    pushLog("[CLASSE]", "ABRE_CHAVES");
                    nonTerminalError();
                }
            }
        } else {
            pushLog("[CLASSE]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[CLASSE]", "CLASS");
        nonTerminalError();
    }

    pushLog("[CLASSE]", "<PROBLEMA COM DERIVACAO>");
    return nonTerminalRefuse();
}

bool PEXP2() {
    nonTerminalStart();
    //PEXP . ID '(' [EXPS] ')'
    if (pegarProximo("PONTO")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("ABRE_PARENTESES")) {
                if (EXP()) {
                    if (pegarProximo("FECHA_PARENTESES")) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[PEXP2]", "FECHA_PARENTESES");
                        nonTerminalError();
                    }
                } else {
                    pushLog("[PEXP2]", "<TENTATIVA EXP(9)>");
                    nonTerminalError();
                }
            } else {
                pushLog("[PEXP2]", "ABRE_PARENTESES");
                nonTerminalError();
            }
        } else {
            pushLog("[PEXP2]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[PEXP2]", "PONTO");
        nonTerminalError();
    }

    pushLog("[PEXP2]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool PEXP() {
    nonTerminalStart();
    if (pegarProximo("ABRE_PARENTESES")) {
        if (PEXP()) {
            if (pegarProximo("FECHA_PARENTESES")) {
                return nonTerminalAccept();
            } else {
                pushLog("[PEXP]", "<FECHA_PARENTESES");
                nonTerminalError();
            }
        } else {
            pushLog("[PEXP]", "<TENTATIVA PEXP(99)>");
            nonTerminalError();
        }
    } else {
        pushLog("[PEXP]", "ABRE_PARENTESES");
        nonTerminalError();
    }

    if (pegarProximo("ID")) {
        return nonTerminalAccept();
    } else {
        pushLog("[PEXP]", "ID");
        nonTerminalError();
    }

    if (pegarProximo("THIS")) {
        if (PEXP2()) {
            return nonTerminalAccept();
        } else {
            pushLog("[PEXP]", "PEXP2(2)");
            nonTerminalError();
        }
    } else {
        pushLog("[PEXP]", "THIS");
        nonTerminalError();
    }
    if (pegarProximo("NEW")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("ABRE_PARENTESES")) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (PEXP2()) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[PEXP]", "<TENTATIVA PEXP2()>");
                        nonTerminalError();
                    }
                } else {
                    pushLog("[PEXP]", "FECHA_PARENTESES");
                    nonTerminalError();
                }
            } else {
                pushLog("[PEXP]", "ABRE_PARENTESES");
                nonTerminalError();
            }
        } else {
            pushLog("[PEXP]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[PEXP]", "NEW");
        nonTerminalError();
    }

    //new id ( )
    if (pegarProximo("NEW")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("ABRE_PARENTESES")) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    return nonTerminalAccept();
                } else {
                    pushLog("[PEXP]", "FECHA_PARENTESES");
                    nonTerminalError();
                }
            } else {
                pushLog("[PEXP]", "ABRE_PARENTESES");
                nonTerminalError();
            }
        } else {
            pushLog("[PEXP]", "ID");
            nonTerminalError();
        }
    } else {
        pushLog("[PEXP]", "NEW");
        nonTerminalError();
    }
    pushLog("[PEXP]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

int SEXP() {
    nonTerminalStart();
    if (pegarProximo("TRUE")) {
        return nonTerminalAccept();
    } else {
        pushLog("[SEXP]", "TRUE");
        nonTerminalError();
    }
    if (pegarProximo("FALSE")) {
        return nonTerminalAccept();
    } else {
        pushLog("[SEXP]", "FALSE");
        nonTerminalError();
    }
    if (pegarProximo("NUM")) {
        return nonTerminalAccept();
    } else {
        pushLog("[SEXP]", "NUM");
        nonTerminalError();
    }
    if (pegarProximo("NULL")) {
        return nonTerminalAccept();
    } else {
        pushLog("[SEXP]", "NULL");
        nonTerminalError();
    }
    if (pegarProximo("NEW")) {
        if (pegarProximo("INT")) {
            if (pegarProximo("ABRE_COLCHETES")) {
                if (EXP()) {
                    if (pegarProximo("FECHA_COLCHETES")) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[SEXP]", "FECHA_COLCHETES");
                        nonTerminalError();
                    }
                } else {
                    pushLog("[SEXP]", "<TENTATIVA EXP(8)>");
                    nonTerminalError();
                }
            } else {
                pushLog("[SEXP]", "ABRE_COLCHETES");
                nonTerminalError();
            }
        } else {
            pushLog("[SEXP]", "INT");
            nonTerminalError();
        }
    } else {
        pushLog("[SEXP]", "NEW");
        nonTerminalError();
    }
    pushLog("[SEXP]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool MEXP() {
    nonTerminalStart();
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MULT")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[MEXP]", "[1] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[MEXP]", "MULT");
            nonTerminalError();
        }
    } else {
        pushLog("[MEXP]", "[2] ID ou NUM");
        nonTerminalError();
    }
    if (pegarProximo("ID")) {
        if (pegarProximo("MULT")) {
            if (PEXP()) {
                return nonTerminalAccept();
            } else {
                pushLog("[MEXP]", "[1] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[MEXP]", "MULT");
            nonTerminalError();
        }
    } else {
        pushLog("[MEXP]", "[2] ID ou NUM");
        nonTerminalError();
    }
    pushLog("[MEXP]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool AEXP() {
    nonTerminalStart();
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MAIS")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[AEXP-1]", "[1] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[AEXP-1]", "MAIS");
            nonTerminalError();
        }
    } else {
        pushLog("[AEXP-1]", "[2] ID ou NUM");
        nonTerminalError();
    }
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MENOS")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[AEXP-2]", "[3] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[AEXP-2]", "MENOS");
            nonTerminalError();
        }
    } else {
        pushLog("[AEXP-2]", "[4] ID ou NUM");
        nonTerminalError();
    }
    pushLog("[AEXP]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool REXP() {
    nonTerminalStart();
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MENOR")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[REXP-1]", "[5] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[REXP-1]", "MENOR");
            nonTerminalError();
        }
    } else {
        pushLog("[REXP-1]", "[6] ID ou NUM");
        nonTerminalError();
    }
    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("MENOR")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[REXP-2]", "[1] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[REXP-2]", "MENOR");
            nonTerminalError();
        }
    } else {
        pushLog("[REXP-2]", "[2] ID ou NUM");
        nonTerminalError();
    }

    if (pegarProximo("ID") || pegarProximo("NUM")) {
        if (pegarProximo("DIFF")) {
            if (pegarProximo("ID") || pegarProximo("NUM")) {
                return nonTerminalAccept();
            } else {
                pushLog("[REXP-3]", "[3] ID ou NUM");
                nonTerminalError();
            }
        } else {
            pushLog("[REXP-3]", "MENOR");
            nonTerminalError();
        }
    } else {
        pushLog("[REXP-3]", "[4] ID ou NUM");
        nonTerminalError();
    }

    pushLog("[REXP]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool EXP() {
    nonTerminalStart();
    if (REXP()) {
        return nonTerminalAccept();
    }
    if (AEXP()) {
        return nonTerminalAccept();
    }
    if (MEXP()) {
        return nonTerminalAccept();
    }
    if (SEXP()) {
        return nonTerminalAccept();
    }
    if (PEXP()) {
        return nonTerminalAccept();
    }
    pushLog("[EXP]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool CMD() {
    nonTerminalStart();
    if (pegarProximo("ABRE_CHAVES")) {
        if (CMD()) {
            if (pegarProximo("FECHA_CHAVES")) {
                return nonTerminalAccept();
            } else {
                pushLog("[CMD-1]", "FECHA_CHAVES");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[CMD-1]", "<TENTATIVA CMD(1)>");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("IF")) {
        if (pegarProximo("ABRE_PARENTESES")) {
            if (EXP()) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (CMD()) {
                        if (pegarProximo("ELSE")) {
                            if (CMD()) {
                                return nonTerminalAccept();
                            } else {
                                pushLog("[CMD-2]", "<TENTATIVA CMD(2)>");
                                return nonTerminalRefuse();
                            }
                        } else {
                            pushLog("[CMD-2]", "ELSE");
                            return nonTerminalRefuse();
                        }
                    } else {
                        pushLog("[CMD-2]", "<TENTATIVA CMD(3)>");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[CMD-2]", "FECHA_PARENTESES");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[CMD-2]", "<TENTATIVA EXP(1)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[CMD-2]", "ABRE_PARENTESES");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("IF")) {
        if (pegarProximo("ABRE_PARENTESES")) {
            if (EXP()) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (CMD()) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[CMD-3]", "<TENTATIVA CMD(4)>");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[CMD-3]", "FECHA_PARENTESES");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[CMD-3]", "<TENTATIVA EXP(2)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[CMD-3]", "ABRE_PARENTESES");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("WHILE")) {
        if (pegarProximo("ABRE_PARENTESES")) {
            if (EXP()) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (CMD()) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[CMD-4]", "<TENTATIVA CMD(5)>");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[CMD-4]", "FECHA_PARENTESES");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[CMD-4]", "<TENTATIVA EXP(3)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[CMD-4]", "ABRE_PARENTESES");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("SYSOUTPRINTLN")) {
        if (pegarProximo("ABRE_PARENTESES")) {
            if (EXP()) {
                if (pegarProximo("FECHA_PARENTESES")) {
                    if (pegarProximo("PONTOVIRGULA")) {
                        return nonTerminalAccept();
                    } else {
                        pushLog("[CMD-5]", "PONTOVIRGULA");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[CMD-5]", "FECHA_PARENTESES");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[CMD-5]", "<TENTATIVA EXP(4)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[CMD-5]", "ABRE_PARENTESES");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("ID")) {
        if (pegarProximo("ATRIBUTO")) {
            if (EXP()) {
                if (pegarProximo("PONTOVIRGULA")) {
                    return nonTerminalAccept();
                } else {
                    pushLog("[CMD-6]", "PONTOVIRGULA");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[CMD-6]", "<TENTATIVA EXP(5)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[CMD-6]", "ATRIBUTO");
            return nonTerminalRefuse();
        }
    }
    if (pegarProximo("ID")) {
        if (pegarProximo("ABRE_COLCHETE")) {
            if (EXP()) {
                if (pegarProximo("FECHA_COLCHETE")) {
                    if (pegarProximo("ATRIBUTO")) {
                        if (EXP()) {
                            if (pegarProximo("PONTOVIRGULA")) {
                                return nonTerminalAccept();
                            } else {
                                pushLog("[CMD-7]", "PONTOVIRGULA");
                                return nonTerminalRefuse();
                            }
                        } else {
                            pushLog("[CMD-7]", "<TENTATIVA EXP(6)>");
                            return nonTerminalRefuse();
                        }
                    } else {
                        pushLog("[CMD-6]", "ATRIBUTO");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[CMD-7]", "FECHA_COLCHETE");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[CMD-7]", "<TENTATIVA EXP(7)>");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[CMD-7]", "ABRE_COLCHETE");
            return nonTerminalRefuse();
        }
    }
    pushLog("[CMD]", "<PROBLEMA COM DERIVACOES>");
    return nonTerminalRefuse();
}

bool MAIN() {
    nonTerminalStart();
    if (pegarProximo("CLASS")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("ABRE_CHAVE")) {
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
                                                            if (CMD()) {
                                                                if (pegarProximo("FECHA_CHAVES")) {
                                                                    if (pegarProximo("FECHA_CHAVES")) {
                                                                        return nonTerminalAccept();
                                                                    } else {
                                                                        pushLog("[MAIN]", "FECHA_CHAVES");
                                                                        return nonTerminalRefuse();
                                                                    }
                                                                } else {
                                                                    pushLog("[MAIN]", "FECHA_CHAVES");
                                                                    return nonTerminalRefuse();
                                                                }
                                                            }
                                                        } else {
                                                            pushLog("[MAIN]", "ABRE_CHAVES");
                                                            return nonTerminalRefuse();
                                                        }
                                                    } else {
                                                        pushLog("[MAIN]", "FECHA_PARENTESES");
                                                        return nonTerminalRefuse();
                                                    }
                                                } else {
                                                    pushLog("[MAIN]", "ID");
                                                    return nonTerminalRefuse();
                                                }
                                            } else {
                                                pushLog("[MAIN]", "FECHA_COLCHETE");
                                                return nonTerminalRefuse();
                                            }
                                        } else {
                                            pushLog("[MAIN]", "ABRE_COLCHETE");
                                            return nonTerminalRefuse();
                                        }
                                    } else {
                                        pushLog("[MAIN]", "ID");
                                        return nonTerminalRefuse();
                                    }
                                } else {
                                    pushLog("[MAIN]", "ABRE_PARENTESES");
                                    return nonTerminalRefuse();
                                }
                            } else {
                                pushLog("[MAIN]", "MAIN");
                                return nonTerminalRefuse();
                            }
                        } else {
                            pushLog("[MAIN]", "VOID");
                            return nonTerminalRefuse();
                        }
                    } else {
                        pushLog("[MAIN]", "STATIC");
                        return nonTerminalRefuse();
                    }
                } else {
                    pushLog("[MAIN]", "PUBLIC");
                    return nonTerminalRefuse();
                }
            } else {
                pushLog("[MAIN]", "ABRE_CHAVES");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[MAIN]", "ID");
            return nonTerminalRefuse();
        }
    } else {
        pushLog("[MAIN]", "CLASS");
        return nonTerminalRefuse();
    }
}