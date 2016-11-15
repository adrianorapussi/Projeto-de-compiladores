//
// Created by Adriano Rapussi
//

#include "analisadorLexico.c"
#include "analisadorSintatico.c"

/**
 * -------------------------------------------------------------------------------------------------------------------
 * Funcao principal
 * -------------------------------------------------------------------------------------------------------------------
 */

int main() {
    analiseLexica();
    analiseSintatica();
    return 0;
}
