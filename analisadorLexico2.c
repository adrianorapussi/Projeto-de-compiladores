//
// Created by Adriano Rapussi
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * -----------------------------------------------------------------------
 * Funçoes usadas
 * -----------------------------------------------------------------------
 */
int stringsIdentificadas(char *id);
void removerLinhaComentario();
void limparArquivoToken(char p);
void escreverArquivo(char *tk);
void lerArquivo();
void libTokens(char *tk);

/*
 * -----------------------------------------------------------------------
 * Constantes
 * -----------------------------------------------------------------------
 */
#define BUFFER 2048
char str [BUFFER];
char vetorStringsIdentificadas[128][30];
int indiceVetorStringsIdentificadas = 0;
static const char TOKEN_FILE[] = "outTokens.txt";

/**
 * -----------------------------------------------------------------------
 * Funçao principal
 * -----------------------------------------------------------------------
 */
int main() {
    limparArquivoToken('c');
    lerArquivo();
    removerLinhaComentario();
    char *pch;
    pch = strtok(str, " ");
    while (pch != NULL) {
        libTokens(pch);
        pch = strtok(NULL, " ");
    }
    return 0;
}

/**
 * -----------------------------------------------------------------------
 * Controla Vetor de strings validas
 * -----------------------------------------------------------------------
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
 * -----------------------------------------------------------------------
 * Remove comentarios multline
 * -----------------------------------------------------------------------
 */
void removerLinhaComentario() {
    char stringWithoutComments[BUFFER];
    int i;
    int indiceStringWithoutComments = 0;
    int ignorar = 0;
    for (i = 1; i < strlen(str); i++) {
        if (str[i] == '*' && str[i - 1] == '/') {
            ignorar = 1;
            indiceStringWithoutComments--;
            continue;
        }
        if (str[i] == '/' && str[i - 1] == '*' && ignorar == 1) {
            ignorar = 0;
            continue;
        }
        if (ignorar != 1) {
            stringWithoutComments[indiceStringWithoutComments] = str[i];
            stringWithoutComments[indiceStringWithoutComments + 1] = '\0';
            indiceStringWithoutComments++;
        }
    }
    strcpy(str, stringWithoutComments);
}

/**
 * -----------------------------------------------------------------------
 * Cria o arquivo de TOKENs
 * -----------------------------------------------------------------------
 */
void limparArquivoToken(char p) {
    FILE *fp;
    if (p == 'd') {
        remove(TOKEN_FILE);
    } else {
        fp = fopen(TOKEN_FILE, "w+");
        fclose(fp);
    }
}

/**
 * -----------------------------------------------------------------------
 * Função realiza append no arquivo de TOKEN
 * -----------------------------------------------------------------------
 */
void escreverArquivo(char *tk) {
    FILE *fp;
    fp = fopen(TOKEN_FILE, "a");
    fprintf(fp, "%s\n", tk);
    fclose(fp);
}

/**
 * -----------------------------------------------------------------------
 * Função que realiza a leitura do arquivo leitura.txt e
 * armazena o conteudo do arquivo em um buffer
 * -----------------------------------------------------------------------
 */
void lerArquivo() {
    FILE *fp;
    char c;
    fp = fopen("leitura.txt", "r");
    int ignorarLinha = 0;
    while (1) {
        c = (char) fgetc(fp);
        if (feof(fp)) {
            break;
        }
        if (c != '\n' && ignorarLinha == 0) {
            if (c == '/' && str[strlen(str) - 1] == '/') {
                // Caso ler um barra e o caracter anterior for barra,
                // entao remove o caracter anterior e liga ignorar
                // resto da linha ate \n
                str[strlen(str) - 1] = '\0';
                ignorarLinha = 1;
                continue;
            }
            str[strlen(str)] = c;
            str[strlen(str) + 1] = '\0';
        } else if (ignorarLinha == 1 && c == '\n') {
            //quando encontrar \n com ignorarlinha ligado concatena espaço
            strcat(str, " ");
            ignorarLinha = 0;
        } else if (ignorarLinha == 1 & c != '\n') {
            //Caracteres que devem ser ignorados
            continue;
        } else {
            strcat(str, " ");
        }
    }
    fclose(fp);
}

/**
 * -----------------------------------------------------------------------
 * Biblioteca de TOKENs
 * -----------------------------------------------------------------------
 */
void libTokens(char *tk) {
    // PALAVRAS RESERVADAS
    if (strcmp("if", tk) == 0) {
        escreverArquivo("<if>");
        return;
    } else if (strcmp("while", tk) == 0) {
        escreverArquivo("<while>");
        return;
    } else if (strcmp("this", tk) == 0) {
        escreverArquivo("<this>");
        return;
    } else if (strcmp("new", tk) == 0) {
        escreverArquivo("<new>");
        return;
    } else if (strcmp("else", tk) == 0) {
        escreverArquivo("<else>");
        return;
    } else if (strcmp("System.out.println", tk) == 0) {
        escreverArquivo("<System.out.println>");
        return;
    } else if (strcmp("boolean", tk) == 0) {
        escreverArquivo("<boolean>");
        return;
    } else if (strcmp("class", tk) == 0) {
        escreverArquivo("<class>");
        return;
    } else if (strcmp("extends", tk) == 0) {
        escreverArquivo("<extends>");
        return;
    } else if (strcmp("public", tk) == 0) {
        escreverArquivo("<public>");
        return;
    } else if (strcmp("static", tk) == 0) {
        escreverArquivo("<static>");
        return;
    } else if (strcmp("void", tk) == 0) {
        escreverArquivo("<void>");
        return;
    } else if (strcmp("main", tk) == 0) {
        escreverArquivo("<main>");
        return;
    } else if (strcmp("return", tk) == 0) {
        escreverArquivo("<return>");
        return;
    } else if (strcmp("int", tk) == 0) {
        escreverArquivo("<int>");
        return;
    }
    // PONTUACAO
    else if (strcmp("(", tk) == 0) {
        escreverArquivo("<AP>");
        return;
    } else if (strcmp(")", tk) == 0) {
        escreverArquivo("<FP>");
        return;
    } else if (strcmp("[", tk) == 0) {
        escreverArquivo("<AC>");
        return;
    } else if (strcmp("]", tk) == 0) {
        escreverArquivo("<FC>");
        return;
    } else if (strcmp("{", tk) == 0) {
        escreverArquivo("<ACH>");
        return;
    } else if (strcmp("}", tk) == 0) {
        escreverArquivo("<FCH>");
        return;
    } else if (strcmp(".", tk) == 0) {
        escreverArquivo("<PONTO>");
        return;
    } else if (strcmp(",", tk) == 0) {
        escreverArquivo("<VIRG>");
        return;
    } else if (strcmp(";", tk) == 0) {
        escreverArquivo("<PV>");
        return;
    }
    // OPERADORES
    else if (strcmp("==", tk) == 0) {
        escreverArquivo("<EQ>");
        return;
    } else if (strcmp("&&", tk) == 0) {
        escreverArquivo("<AND>");
        return;
    } else if (strcmp("-", tk) == 0) {
        escreverArquivo("<MINUS>");
        return;
    } else if (strcmp("+", tk) == 0) {
        escreverArquivo("<PLUS>");
        return;
    } else if (strcmp("*", tk) == 0) {
        escreverArquivo("<MULT>");
        return;
    } else if (strcmp("!", tk) == 0) {
        escreverArquivo("<NOT>");
        return;
    } else if (strcmp("=", tk) == 0) {
        escreverArquivo("<ATR>");
        return;
    } else if (strcmp("!=", tk) == 0) {
        escreverArquivo("<DIF>");
        return;
    } else if (strcmp("<", tk) == 0) {
        escreverArquivo("<LT>");
        return;
    }
    // NUMEROS
    int i;
    char number[1024];
    //Limpa area de memoria. Necessario para concatenacao de Numero realizada mais abaixo
    memset(number, 0, 1024);
    int index = 0;
    int totalTimesCountNumber = 0;
    int totalTimesIterate = 0;
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
        strcpy(n, "<num,");
        strcat(n, number);
        strcat(n, ">");
        escreverArquivo(n);
        //identificou numero, portanto, deve parar execucao
        return;
    }
    // VARIAVEIS
    int primeiraLeitura = 1;
    char variavel[1024];
    index = 0;
    memset(variavel, 0, 1024);
    for (i = 0; tk[i] != '\0'; i++) {
        if (tk[i] == 'a' || tk[i] == 'b' || tk[i] == 'c' || tk[i] == 'd'
            || tk[i] == 'e' || tk[i] == 'f' || tk[i] == 'g' || tk[i] == 'h'
            || tk[i] == 'i' || tk[i] == 'j' || tk[i] == 'k' || tk[i] == 'l'
            || tk[i] == 'm' || tk[i] == 'n' || tk[i] == 'o' || tk[i] == 'p'
            || tk[i] == 'q' || tk[i] == 'r' || tk[i] == 'r' || tk[i] == 's'
            || tk[i] == 't' || tk[i] == 'u' || tk[i] == 'v' || tk[i] == 'x'
            || tk[i] == 'z' || tk[i] == 'y' || tk[i] == 'w') {
            primeiraLeitura = 0;
            variavel[index++] = tk[i];
        } else if (tk[i] == '0' || tk[i] == '1' || tk[i] == '2'
                   || tk[i] == '3' || tk[i] == '4' || tk[i] == '5'
                   || tk[i] == '6' || tk[i] == '7' || tk[i] == '8'
                   || tk[i] == '9' || tk[i] == '_') {
            if (primeiraLeitura == 1) {
                //Leu um caracter diferente de ALPHA como primeiro caracter entao nao é variavel
                printf("ERRO LEXICO: %s\n", tk);
                exit(-1);
            }
            primeiraLeitura = 0;
            variavel[index++] = tk[i];
        } else {
            printf("ERRO LEXICO: %s\n", tk);
            exit(-1);
        }
    }
    char n[1024];
    int idIndex = stringsIdentificadas(variavel);
    char idIndexString[2];
    sprintf(idIndexString, "%d", idIndex);
    strcpy(n, "<id,");
    strcat(n, idIndexString);
    strcat(n, ">");
    escreverArquivo(n);
    return;
}
