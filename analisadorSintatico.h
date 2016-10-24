#define BUFFER 2048
#define MAX 32

typedef enum {
	// Variáveis
	IDtk, 

	// Palavras-Chave
	IFtk, WHILEtk, ELSEtk, NEWtk, THIStk, SYSOUTPRINTLNtk, BOOLEANtk, CLASStk,
	EXTENDStk, PUBLICtk, STATICtk, VOIDtk, MAINtk, RETURNtk, INTtk,

	//Pontuação
	APtk, FPtk, ACOtk, FCOtk, ACHtk, FCHtk, PONTOtk, VIRGULAtk, PONTOVIRGULAtk,

	// Numeros
	NUMERALtk, 

	// Operadores
	EQUIVALENTEtk, ANDtk, MENOStk, MAIStk, MULTtk, IGUALtk, DIFFtk, NOTtk, MENORtk, MAIORtk

} TokenType;


struct tokenTag {
	char str[MAX];
	TokenType tokenType;

	struct tokenTag *next;
};
typedef struct tokenTag Token;


typedef enum {
	programNode, classNode, blockNode, methodNode, mainNode,
	declNode, assignNode, exprNode, numNode, operationNode,
	idNode, objectNode, callNode, 
	ifNode, elseNode, whileNode, paramNode, conditionNode,
	returnNode
} NodeType; //Tipos de nó para nossa AST

/*------- TREE -------*/
struct nodeTag {
	NodeType nodeType;
	Token *tokenPtr; // Ponteiro para os tokens que compõe este nó 
	struct nodeTag *child1; // Filhos do nó (até 4)
	struct nodeTag *child2;
	struct nodeTag *child3;
	struct nodeTag *child4; 
};
typedef struct nodeTag Node;
