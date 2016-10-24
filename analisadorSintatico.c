//
// Created by Bruno Weninger
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "analisadorSintatico.h"

#define BUFFER 2048
#define MAX 32
char str[BUFFER];

void lerArquivo();

void classificarToken(Token token);
Token *tokens;
int numTokens;

void main(){

}

void lerArquivo(){
    numTokens = 0;
    FILE *fp;
    char linhaCaracter;
    fp = fopen("outTokens.txt", "r");
    tokens = (Token *) malloc(numTokens * sizeof(Token));
    Token tokenAtual;
    while (true) {
        linhaCaracter = (char) fgetc(fp);        
        if (feof(fp)) {
            break;
        } else if (linhaCaracter == '<') { //Novo Token encontrado          
            tokens = (Token *)realloc(tokens, ++numTokens * sizeof(Token));
        } else if (linhaCaracter == '>') { //Fim do Token atual
            strcat(tokenAtual.str, '\0');
            classificarToken(tokenAtual);
        } else { //obtem apenas simbolos e não os caracteres < e > pois não nos interessam
            strcat(tokenAtual.str, linhaCaracter);
        }
        
    }
    fclose(fp);
}

void classificarToken(Token token) {
    if(token.str[0] == 'I' && token.str[1] == 'D') { // Se as duas primeiras letras forem ID, variável
        token.tokenType = IDtk;
    } else if (token.str[0] == 'N' && token.str[1] == 'U' && token.str[2] == 'M') { //Se as três primeiras forem NUM, numeral
        token.tokenType = NUMERALtk;
    } else if (strcmp(token.str, "IF") == 0) {
        token.tokenType = IFtk;
    } else if (strcmp(token.str, "ELSE") == 0) {
        token.tokenType = ELSEtk;
    } else if (strcmp(token.str ,"WHILE") == 0) {
        token.tokenType = WHILEtk;
    } else if (strcmp(token.str ,"NEW") == 0) {
        token.tokenType = NEWtk;
    } else if (strcmp(token.str ,"THIS") == 0) {
        token.tokenType = THIStk;
    } else if (strcmp(token.str ,"SYSOUTPRINTLN") == 0) {
        token.tokenType = SYSOUTPRINTLNtk;
    } else if (strcmp(token.str ,"BOOLEAN") == 0) {
        token.tokenType = BOOLEANtk;    
    } else if (strcmp(token.str ,"CLASS") == 0) {
        token.tokenType = CLASStk;
    } else if (strcmp(token.str ,"PUBLIC") == 0) {
        token.tokenType = PUBLICtk;
    } else if (strcmp(token.str ,"STATIC") == 0) {
        token.tokenType = STATICtk;
    } else if (strcmp(token.str ,"VOID") == 0) {
        token.tokenType = VOIDtk;
    } else if (strcmp(token.str ,"MAIN") == 0) {
        token.tokenType = MAINtk;
    } else if (strcmp(token.str ,"EXTENDS") == 0) {
        token.tokenType = EXTENDStk;
    } else if (strcmp(token.str ,"RETURN") == 0) {
        token.tokenType = RETURNtk;
    } else if (strcmp(token.str ,"INT") == 0) {
        token.tokenType = INTtk;
    } else if (strcmp(token.str ,"ABRE_PARENTESES") == 0) {
        token.tokenType = APtk;
    } else if (strcmp(token.str ,"FECHA_PARENTESES") == 0) {
        token.tokenType = FPtk;
    } else if (strcmp(token.str ,"ABRE_COLCHETE") == 0) {
        token.tokenType = ACOtk;
    } else if (strcmp(token.str ,"FECHA_COLCHETE") == 0) {
        token.tokenType = FCOtk;
    } else if (strcmp(token.str ,"ABRE_CHAVES") == 0) {
        token.tokenType = ACHtk;
    } else if (strcmp(token.str ,"FECHA_CHAVES") == 0) {
        token.tokenType = FCHtk;
    } else if (strcmp(token.str ,"PONTO") == 0) {
        token.tokenType = PONTOtk;
    } else if (strcmp(token.str ,"VIRGULA") == 0) {
        token.tokenType = VIRGULAtk;
    } else if (strcmp(token.str ,"PONTOVIRGULA") == 0) {
        token.tokenType = PONTOVIRGULAtk;
    } else if (strcmp(token.str ,"EQUIVALENTE") == 0) {
        token.tokenType = EQUIVALENTEtk;
    } else if (strcmp(token.str ,"IGUAL") == 0) {
        token.tokenType = IGUALtk;
    } else if (strcmp(token.str ,"MAIS") == 0) {
        token.tokenType = MAIStk;
    } else if (strcmp(token.str ,"MENOS") == 0) {
        token.tokenType = MENOStk;
    } else if (strcmp(token.str ,"MAIOR") == 0) {
        token.tokenType = MAIORtk;
    } else if (strcmp(token.str ,"MENOR") == 0) {
        token.tokenType = MENORtk;
    } else if (strcmp(token.str ,"AND") == 0) {
        token.tokenType = ANDtk;
    } else if (strcmp(token.str ,"MULT") == 0) {
        token.tokenType = MULTtk;
    } else if (strcmp(token.str ,"DIFF") == 0) {
        token.tokenType = DIFFtk;
    } else if (strcmp(token.str ,"NOT") == 0) {
        token.tokenType = NOTtk;
    }
}