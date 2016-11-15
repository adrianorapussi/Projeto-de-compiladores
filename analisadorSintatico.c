//
// Created by Adriano Rapussi.
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

bool isParametro(void);

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

/*
 * -------------------------------------------------------------------------------------------------------------------
 * Constantes
 * -------------------------------------------------------------------------------------------------------------------
 */
#define BUFFER 2048
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

//---------------------------------------------------------------------------------------------------------------------
//Analisador Sintatico
//_____________________________________________________________________________________________________________________

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
    int newCurrentPosition = getCurrentPosOnStack();
    desempilha();
    update(newCurrentPosition);
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

/**
 * Valida se a sequencia de tokens representa parametros de funcao ou procedimento
 */
bool isParametro(void) {
    nonTerminalStart();
    if (pegarProximo("INT") || pegarProximo("BOOLEAN")) {
        if (pegarProximo("ID")) {
            return nonTerminalAccept();
        } else {
            pushLog("[isParametro]", "ID");
            nonTerminalError();
            return false;
        }
    } else {
        pushLog("[isParametro]", "INT ou BOOLEAN");
        nonTerminalError();
        return false;
    }
}

/**
 * Valida se a sequencia de tokens representa declaracao de variaveis
 */
bool isVariavel(void) {
    nonTerminalStart();
    if (pegarProximo("INT")) {
        if (pegarProximo("ID")) {
            if (pegarProximo("PONTOVIRGULA")) {
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
        if (pegarProximo("ID")) {
            if (pegarProximo("PONTOVIRGULA")) {
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
            if (pegarProximo("ID")) {
                if (pegarProximo("ABRE_PARENTESES")) {
                    if (isParametro()) {
                        if (pegarProximo("FECHA_PARENTESES")) {
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
            if (pegarProximo("ABRE_PARENTESES")) {
                if (isEquivalencia()) {
                    if (pegarProximo("FECHA_PARENTESES")) {
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
            CMD_CMD:
            if (isFuncaoOuProcedimento()) {
                goto CMD_CMD;
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
        if (pegarProximo("IGUAL")) {
            if (isEquivalencia()) {
                if (pegarProximo("PONTOVIRGULA")) {
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
