# Projeto de Linguagens de Programação

A proposta desta atividade é a implementação das etapas front-end do projeto de uma linguagem de programação, ou seja, as primeiras etapas da construção do compilador da linguagem específica. 

A linguagem utilizada será o MiniJava (Java Simplificado).

## Etapas:
1. Gramática livre de contexto e autômatos finitos
2. Analisador léxico
3. Analisador sintático
4. Analisador semântico

**OBS1:** todas as etapas devem ser entregues na data determinada.

**OBS2:** cada entrega deve ser composta por 2 arquivos: código e documentação (ou simplesmente código muito bem comentado).

**OBS3:** a não entrega de uma etapa zera o projeto.

### ETAPA 1
Gramática Livre de Contexto e Autômatos finitos

A partir das definições da linguagem específica e da sugestão de uma sintaxe (final do arquivo), construir a gramática livre de contexto correspondente.

Construir os autômatos para cada classe de elementos do vocabulário terminal da linguagem: letras, dígitos, números, identificadores, operadores, palavras reservadas, pontuação, comentários.

Testar cada classe gerada.

Juntar todos os autômatos em um único para que represente todos os elementos.

**OBS4:** encaminhar o autômato completo e não separadamente para cada padrão definido.

**OBS5:** utilizar a ferramenta JFLAP (www.jflap.org) para a construção dos autômatos.

### ETAPA 2 – Analisador léxico
Desenvolver um programa que realize a análise léxica.

A partir de um arquivo contendo um código (programa) em MiniJava, o analisador léxico deverá produzir um arquivo com os tokens identificados e seus valores correspondentes.

Cada linha do arquivo de saída deve conter um token reconhecido.

Fazer uso do que foi implementado na Etapa#1.

O analisador léxico deve ser capaz de lidar com os erros léxicos encontrados no programa. A mensagem de erro, “ERRO LÉXICO” juntamente com a sequência lexicamente errada, devem ser apresentadas e todo o processo finalizado.

### ETAPA 3 – Analisador sintático
Construir um programa que realize a análise sintática (análise léxica + análise sintática).

A partir do arquivo com os tokens gerados pelo analisador léxico (Etapa 2), realizar a análise ascendente ou descendente com base na gramática da linguagem.

O analisador sintático deve ser capaz de lidar com erros sintáticos encontrados no programa. A mensagem de erro, “ERRO SINTÁTICO” juntamente com o token incorreto, deve ser apresentados e todo o processo finalizado.

É necessário entregar os analisadores léxico e sintático funcionando em conjunto.

### ETAPA 4 – Analisador semântico
Deve-se construir e/ou atualizar a tabela de símbolos, fazer a checagem de tipos (a partir da árvore sintática gerada) e verificar as chamadas de métodos. 

É necessário entregar todos os analisadores desenvolvidos funcionando em conjunto.

#### CRITÉRIO DE AVALIAÇÃO
+ Etapa 1 – 25%
+ Etapa 2 – 25%
+ Etapa 3 – 25%
+ Etapa 4 – 25%

**OBS6:** o desenvolvimento de cada ETAPA deve ser explicado e, se preciso for, exemplificado.

Documentação interna: comentários claros e objetivos incluídos no código fonte do programa

## DATAS IMPORTANTES

+ Etapa 1 – 08/SET/2016
+ Etapa 2 – 29/SET/2016
+ Etapa 3 – 27/OUT/2016
+ Etapa 4 – 17/NOV/2016

## MINIJAVA

MiniJava é um subconjunto da linguagem Java. Como é um subconjunto, todo programa MiniJava é um programa Java válido que pode ser executado pela JVM.

MiniJava restringe a linguagem Java para ter apenas inteiros, booleanos, vetores de inteiros e classes, removendo interfaces, números de ponto flutuante, classes abstratas, strings, vetores de outros tipos etc. 

Também não há sobrecarga de métodos, ou métodos estáticos, exceto pelo método main da classe principal do programa. 

O comando System.out.println de MiniJava só pode imprimir números.

Um exemplo simples de MiniJava:

```java
class Factorial{
    public static void main(String[] a){
        System.out.println(new Fac().ComputeFac(10));
    }
}
class Fac {
    public int ComputeFac(int num){
        int num_aux;
        if (num < 1)
            num_aux = 1;
        else
            num_aux = num * (this.ComputeFac(num-1));
        return num_aux ;
    }
}
```

Um programa MiniJava está restrito a um único arquivo fonte, não existe o conceito de pacote. 

Existem outros programas exemplo na página de MiniJava.

### Especificação Léxica

Note que MiniJava trata System.out.println como uma palavra reservada, não como uma chamada do método println. Isso facilita o restante do compilador.
Também não há um operador de divisão.

### Espaços em branco:
```
[ \n  \t  \r  \f ]
```
### Comentários: dois tipos de comentário

```
// -- comentário de linha
/* */ -- comentário de 1 ou mais linhas
```

#### Identificadores: uma letra [a-zA-Z]+, seguido de zero ou mais letras [0-9a-zA-Z]*, dígitos ou _

#### Numerais: apenas números inteiros __[0-9]__

#### Operadores: 

```
=
<
==
!=
+
-
*
&&
!
```

#### Pontuação: 

```
(
)
[
]
{
}
;
.
,
```

#### Palavras reservadas: 

```
boolean
class
extends
public
static
void
main
String
return
int
if
else
while
System.out.println
length
true
false
this
new
null
```

### Sintaxe

A sintaxe é dada usando EBNF. Meta-símbolos EBNF usados como tokens estão entre aspas simples.
```
PROG   -> isMainDeClasse {isClasse}
isMainDeClasse   -> class id '{' public static void main ( String [ ] id ) '{' CMD '}' '}'
isClasse -> class id [extends id] '{' {VAR} {isDeclaracaoMetodo} '}'
VAR    -> TIPO id ;
isDeclaracaoMetodo -> public TIPO id '(' [isParametro] ')' '{' {VAR} {CMD} return EXP ; '}'
isParametro -> TIPO id {, TIPO id}
TIPO   -> int '[' ']'
| boolean
| int
| id
CMD    -> '{' {CMD} '}'
| if '(' EXP ')' CMD
| if '(' EXP ')' CMD else CMD
| while '(' EXP ')' CMD
| System.out.println '(' EXP ')' ;
| id = EXP ;
| id '[' EXP ']' = EXP ;
EXP    -> EXP && isSubtracao
| isSubtracao
isSubtracao   -> isSubtracao < isAdicao
| isSubtracao == isAdicao
| isSubtracao != isAdicao
| isAdicao
isAdicao   -> isAdicao + isMultiplicacao
| isAdicao - isMultiplicacao
| isMultiplicacao
isMultiplicacao   -> isMultiplicacao * isAtribuicao
| isMultiplicacao / isAtribuicao
| isAtribuicao
isAtribuicao   -> ! isAtribuicao
| - isAtribuicao
| true
| false
| num
| null
| new int '[' EXP ']'
| isInstanciaDeClasse . length
| isInstanciaDeClasse '[' EXP ']'
| isInstanciaDeClasse
isInstanciaDeClasse   -> id
| this
| new id '(' ')'
| '(' EXP ')'
| isInstanciaDeClasse . id
| isInstanciaDeClasse . id '(' [EXPS] ')'
EXPS   -> EXP {, EXP}
```
### Referências:

1. http://www.cambridge.org/resources/052182060X/MCIIJ2e/grammar.htm
2. http://www.cs.tufts.edu/~sguyer/classes/comp181-2006/minijava.html
3. http://cs.fit.edu/~