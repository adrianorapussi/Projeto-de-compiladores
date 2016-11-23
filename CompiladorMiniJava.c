#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFFER 4196
#define TRUE 1
#define FALSE 0

char str[BUFFER];
char str3[BUFFER];

struct StructID {
    char ID[128][30]; //128 posicoes com 30 caracteres cada
    char type[128]; //(i)nt, (b)oolean, (c)lass
    char methodInputType[128]; //(i)nt (b)oolean
    int idx; //indice corrente;
};
struct StructID structID;

char tokenKey[2048][30];
char tokenValue[2048][30];
int  idxToken=0;
int  idxTempToken=0;

static const char TOKEN_FILE[] = "token.txt";
static const char SOURCE_FILE[] = "file.txt";

const char* getRecognizedTokenValueByIndex(int idx);
const char* getRecognizedTokenKeyByIndex(int idx);
void printRecognizedToken();
void addRecognizedToken(char* key, char *value);

char* spreadTokenKeyValue(char *tk, int kv);
void tokenLibrary(char * tk);

/*******************************************
  Funcoes de manipulacao da tabela de ID
  reconhecidos pelo analisador lexico
  Esta tabela é utilizada como base para 
  identificacao de tipagem de variavel e funcoes
 *******************************************/
int convertTokenPositionToLexemaID(int idxTokenPosition);
void printStructID();
void printStructIDbyIDX(int idx);
int addStructID(char * id );
void setStructIDType(char vtype, int idx );


char * getStructIDName(int idx);
void removeCommentLine();
void cleanTokenFile(char p);
void write_plp(const char *tk);
void read_plp( const char *filename);
void readToken();
void tokenLibrary(char * tk);

void AnalisadorSintatico();
void AnalisadorSemantico();

int main() {
	cleanTokenFile('c');
	read_plp(SOURCE_FILE);
	removeCommentLine();
	char *pch;
	pch = strtok(str," ");
	while (pch!= NULL){
		tokenLibrary(pch);
		pch = strtok(NULL, " ");
	}

	//Inicia leitura dos tokens
	readToken();
	pch = strtok(str3," ");
	while (pch!= NULL){        
		addRecognizedToken(spreadTokenKeyValue(pch,0),spreadTokenKeyValue(pch,1));
		pch = strtok(NULL, " ");
	}
	AnalisadorSintatico();
    printStructID();
    AnalisadorSemantico();
	return 0;
}

/*******************************************
  Recognized Tokens
 *******************************************/
void addRecognizedToken(char* key, char *value) {
	strcpy(tokenKey[idxToken], key);
	strcpy(tokenValue[idxToken], value);
	idxToken++;
}

/*******************************************
  Print Recognized Tokens
 *******************************************/
void printRecognizedToken() {
	int i=0;
	for (i=0; i < idxToken ; i ++) {
		printf("%s/%s\n", tokenKey[i],tokenValue[i] );
	}
}

const char* getRecognizedTokenValueByIndex(int idx){
	return tokenValue[idx];
}

const char* getRecognizedTokenKeyByIndex(int idx){
    return tokenKey[idx];
}

int convertTokenPositionToLexemaID(int idxTokenPosition) {
    char idString[30];
    strcpy(idString,getRecognizedTokenValueByIndex(idxTokenPosition));
    int idStringConvertedtoInt = atoi(idString);
    return idStringConvertedtoInt;
}



/*******************************************
  Controla Vetor de strings validas     
 *******************************************/
int addStructID( char * id ){
	int i=0;
	for (i=0; i< structID.idx ; i++) {
		if (strcmp(structID.ID[i],id)==0) {
			return i;
		}
	}
	strcpy(structID.ID[structID.idx], id);
	i = structID.idx;
	structID.idx++;
	return i;
}

void setStructIDType(char vtype, int idx ) {
    structID.type[idx] = vtype;
}
void setStrucIDMethodInputType (char vInputType, int idx) {
	structID.methodInputType[idx] = vInputType;
}
char  getStructIDType(int idx ) {
    return structID.type[idx];
}

char  getStructIDMethodInputType(int idx ) {
    return structID.methodInputType[idx];
}

char * getStructIDName(int idx) {
    return structID.ID[idx];
}

void printStructID() {
	printf("/**************************************\n\tLEXEMA TABLE\n*************************************/\n");
	printf("IDX                      LEXEMA  TYPE  METHOD INPUT TYPE\n");
	printf("--------------------------------------------------------\n");
    int i=0;
    for (i=0; i <structID.idx; i++) {       
        printf("%2d|%29s|%3c|%3c\n",i,getStructIDName(i), getStructIDType(i), getStructIDMethodInputType(i));
    }
    printf("\n");
}

/*******************************************
  Remove comentarios multline
 *******************************************/
void removeCommentLine() {
	char strwithoutcomment[BUFFER];
	int i;
	int strwithoutcommentIDX=0;
	int ignorar=0;
	for (i=1; i < strlen(str); i++ ){        
		if ( str[i] == '*' && str[i-1]=='/') {
			//nao copia
			ignorar=1;
			//anda uma casa para tras no ponteiro para substituir o / que foi colocado
			strwithoutcommentIDX--;
			continue;
		}
		if (str[i] == '/' && str[i-1]=='*' && ignorar==1) {
			ignorar=0;
			continue;
		}

		if (ignorar!=1) {
			//copia
			//printf("%c", str[i]);
			strwithoutcomment[strwithoutcommentIDX] = str[i];
			strwithoutcomment[strwithoutcommentIDX+1] = '\0';
			strwithoutcommentIDX++;
		} 
	}
	strcpy(str,strwithoutcomment);
}

/*******************************************
  Cria o arquivo de TOKENs
 *******************************************/
void cleanTokenFile(char p) {
	FILE * fp;

	if ( p == 'd') {
		remove(TOKEN_FILE);
	}
	else {
		fp = fopen(TOKEN_FILE, "w+");
		fclose(fp);         
	}
	// todo exemplo

}

/*******************************************
  Função realiza append
  no arquivo de TOKEN 
 *******************************************/
void write_plp(const char *tk) {
	FILE * fp;

	// todo exemplo
	fp = fopen(TOKEN_FILE, "a");
	fprintf(fp, "%s\n",tk);
	fclose(fp);
}

/*******************************************
  Função realiza realiza a leitura
  e joga todo o arquivo em um buffer
 *******************************************/
void read_plp(const char *filename) {
	FILE *fp;
	char c;

	fp = fopen(filename, "r");
	int ignorarLinha=0;
	while(1) {
		c = fgetc(fp);
		if(feof(fp)){ 
			break;
		}

		if(c != '\n' && ignorarLinha==0) {

			if ( c == '/' && str[strlen(str)-1]=='/') {
				//Caso ler um barra e o caracter anterior for barra, entao remove o caracter anterior
				//e liga ignorar resto da linha ate \n
				str[strlen(str)-1] = '\0';    
				ignorarLinha=1;
				continue;

			}
			str[strlen(str)] = c;
			str[strlen(str)+1] = '\0';

		} else if (ignorarLinha==1 && c=='\n') {
			//quando encontrar \n com ignorarlinha ligado concatena espaço
			strcat(str, " ");
			ignorarLinha=0;

		} else if (ignorarLinha==1 & c!='\n') {
			//Caracteres que devem ser ignorados
			continue;
		}
		else {
			strcat(str, " ");
		}

	}
	fclose(fp);
}

/*******************************************
  Separa os tokens em key,value
 *******************************************/
char* spreadTokenKeyValue (char *tk, int kv) {
	char *tokenKey = malloc(32);
	char *tokenValue = malloc(32);    
	//Diretiva para WINDOWS
	//memset adicionado para garantir que nao seja referenciado espacos de memorias pre alocados
	memset(tokenKey, 0, sizeof(tokenKey));
	memset(tokenValue, 0, sizeof(tokenValue));
	int antesVirgula = 1;
	int i = 0;

	for (i = 0; tk[i] != '\0'; i++){
		if (tk[i] == '<') {
			continue;
		} else if (tk[i] == ',') {
			antesVirgula = 0;
			continue;
		} else if (tk[i] == '>') {
			continue;
		} else {
			char aux[2] = {tk[i], '\0'};
			if (antesVirgula == 1) {
				strcat(tokenKey, aux);
			} else {
				strcat(tokenValue, aux);
			}
		}
	}
	if (kv == 0) {
		return tokenKey;
	} else if (kv == 1) {
		return tokenValue;
	}
    return "";
}

/*******************************************
  Função realiza realiza a leitura
  e joga todo o arquivo em um buffer
 *******************************************/
void readToken() {
	FILE *fp;
	char c;

	fp = fopen(TOKEN_FILE, "r");

	int ignorarlinha=0;

	while(1) {
		c = fgetc(fp);
		if(feof(fp)){ 
			break;
		}

		if(c != '\n' ) {            
			str3[strlen(str3)] = c;
			str3[strlen(str3)+1] = '\0';
		}

		else  {
			strcat(str3, " ");
		}
	}

	fclose(fp);
}

/*******************************************
  Pilha de controle para posicao do vetor
  de tokens reconhecidos
 *******************************************/
int idxStack=0;

struct Stack {
	int start;
	int current;
};

struct Stack stack[128];


/*******************************************
  Pilha para log de mensagens
 *******************************************/
int idxStackLog=0;
int idxStackLogPop=0;

struct StackLog {
	char mensagem[2048];
	int current;
};
struct StackLog stackLog[128];

void pushLog(char* funcao, char* esperado) {
	char  mensagemFinal[2048];

	strcpy(mensagemFinal,funcao);
	strcat(mensagemFinal," Esperado ");
	strcat(mensagemFinal,esperado);
	strcat(mensagemFinal,". Recebido ");
	strcat(mensagemFinal,tokenKey[getCurrentPosOnStack()]);    
	strcpy(stackLog[idxStackLog].mensagem,mensagemFinal);
	stackLog[idxStackLog].current = getCurrentPosOnStack();

	idxStackLog++;
}

int popLog(){        
	if (idxStackLog==1){
		return FALSE;
	}
	char currentTempString[30];
	sprintf(currentTempString, "%d", stackLog[idxStackLog].current);

	char  mensagemFinal[2048];
	strcpy(mensagemFinal,"Posicao ");
	strcat(mensagemFinal,currentTempString);
	strcat(mensagemFinal,": ");
	strcat(mensagemFinal,stackLog[idxStackLog].mensagem);
	idxStackLog--;
	printf("%s\n",mensagemFinal );
	return TRUE;
}

/*******************************************
  Pilha de controle para posicao do vetor
  de tokens reconhecidos
 *******************************************/
int match(char* tk, char* word){
	if (strcmp(tk,word)==0) {
		return TRUE;
	} else {
		return FALSE;
	}
}


void update(int newvalue) {
	//printf("update - current %d\n",newvalue);
	stack[idxStack].current=newvalue;
}

void increment() {
	//printf("increment - atualizando current para %d\n",stack[idxStack].current+1 );
	stack[idxStack].current=stack[idxStack].current+1;

}

void push(int vstart, int vcurrent ) {    
	idxStack++;
	printf("push \n\tstart: %d\n\tcurrent: %d\n\tidxStack: %d\n",vstart,vcurrent,idxStack );
	stack[idxStack].start=vstart;
	stack[idxStack].current=vcurrent;


}

void pop() {
	//printf("pop %d \n",idxStack );
	if (idxStack<0) {
		printf("[ERRO]deu ruim na pilha\n");
		exit(-1);
	}
	idxStack--;
}

int getCurrentPosOnStack(){
	return stack[idxStack].current;
}


int lookahead(char* word) {

	if (match(tokenKey[getCurrentPosOnStack()],word)) {
		printf("LIDO: %s\n",tokenKey[getCurrentPosOnStack()] );
		increment();
		return TRUE;
	} else {
		printf("ESPERADO: %s | LIDO %s\n",word,tokenKey[getCurrentPosOnStack()] );
		return FALSE;
	}
}

int lookback() {
    return getCurrentPosOnStack()-1;
}


void nonTerminalStart() {
	push(stack[idxStack].current,stack[idxStack].current);
}

void nonTerminalError() {
	pop();
	push(stack[idxStack].current,stack[idxStack].current);
}

int nonTerminalRefuse() {
	pop();
	return FALSE;
}

int nonTerminalAccept() {
	int newCurrentPosition=getCurrentPosOnStack();
	pop();
	update(newCurrentPosition);

	return TRUE;
}

int equalString(const char* string1, char* string2) {
	if (strcmp(string1, string2)==0) {
		return TRUE;
	} else {
		return FALSE;
	}	
}

struct StackSemantica {
    int idxTokenStart[256];
    int idxTokenEnd[256];
    int idx;
};
struct StackSemantica stackSemantica;

void pushStackSemantica(int tokenStart, int tokenEnd ) {
    stackSemantica.idxTokenStart[stackSemantica.idx] = tokenStart;
    stackSemantica.idxTokenEnd[stackSemantica.idx] = tokenEnd;
    stackSemantica.idx++;    
}

int ruleArithimetic(int firstTokenRead, int lastTokenRead) {
	int i;
	char firstType = getStructIDType(convertTokenPositionToLexemaID(firstTokenRead));
	
	for (i=firstTokenRead+1; i<=lastTokenRead; i++) {
		
		char TOKENKEY[30];
		strcpy(TOKENKEY,getRecognizedTokenKeyByIndex(i));
		//printf("==> %s  \n",TOKENKEY);

		if (equalString(TOKENKEY, "id") ) {
			//printf("LEXEMA %d\n", convertTokenPositionToLexemaID(i));
			char laterType=getStructIDType(convertTokenPositionToLexemaID(i));

			if (firstType!=laterType ) {
				//printf("%c | %c\n",firstType, laterType);
				//printf("%s | %s\n",getStructIDName(convertTokenPositionToLexemaID(firstTokenRead)), getStructIDName(convertTokenPositionToLexemaID(i)));
				//printf("%d | %d\n",firstTokenRead, i);
				return FALSE;
			}
		} else {
			continue;
		}
	}
	return TRUE;

}

int ruleMethodInput(int firstTokenRead, int lastTokenRead) {
	int i;
	char methodInputType = getStructIDMethodInputType(convertTokenPositionToLexemaID(firstTokenRead));
	for (i=firstTokenRead+1; i<=lastTokenRead; i++) {
		
		char TOKENKEY[30];
		strcpy(TOKENKEY,getRecognizedTokenKeyByIndex(i));

		if (equalString(TOKENKEY, "id") ) {
			char laterType=getStructIDType(convertTokenPositionToLexemaID(i));

			if (methodInputType!=laterType ) {
				return FALSE;
			}
		} else {
			continue;
		}
	}
	return TRUE;

}

int semanticValidator(int firstTokenRead, int lastTokenRead) {
	//Arithimetic Rule
	if (ruleArithimetic(firstTokenRead,lastTokenRead)) {
		return TRUE;
	}

	if (ruleMethodInput(firstTokenRead,lastTokenRead)) {
		return TRUE;
	}
	printf("Semantic rules not validated\n");
	return FALSE;
}

void AnalisadorSemantico() {
	int i;
	int reconstructionindex;
	char recontructedStatement[2056];
	memset(recontructedStatement,0, 2056);

	for (i=0; i < stackSemantica.idx ; i ++ ) {
		if(!semanticValidator(stackSemantica.idxTokenStart[i],stackSemantica.idxTokenEnd[i])) {
			printf("Reconstructed Statement:\n%s\n",recontructedStatement );
			printf("/**************************************\n\tSEMANTIC ERROR\n*************************************/\n");
			strcat(recontructedStatement," ");
			strcat(recontructedStatement,getRecognizedTokenKeyByIndex(i));
			exit(-1);
		}
	}
	printf("/**************************************\n\tSEMANTIC OK\n*************************************/\n");
}

int PARAMS(int idxLexemaNomeMetodo) {

	nonTerminalStart();

	if (lookahead("int")  ) {
        int idxLexema = convertTokenPositionToLexemaID(getCurrentPosOnStack());
		if (lookahead("id") ) {
            setStructIDType('i',idxLexema);
            setStrucIDMethodInputType('i', idxLexemaNomeMetodo);
			return nonTerminalAccept();
		} else {
			pushLog("[PARAMS-1]", "id");
			nonTerminalError();
		}
	} 
	
    if (lookahead("boolean") ) {
        int idxLexema = convertTokenPositionToLexemaID(getCurrentPosOnStack());
        if (lookahead("id") ) {
            setStructIDType('b',idxLexema);
            setStrucIDMethodInputType('b', idxLexemaNomeMetodo);
            return nonTerminalAccept();
        } else {
            pushLog("[PARAMS-2]", "id");
            nonTerminalError();
        }
    }
    
    pushLog("[PARAMS]", "<nenhuma das derivacoes foi identificada>");
    return nonTerminalRefuse(); 

}

int VAR() {
	nonTerminalStart();

	if (lookahead("int") ) {
		//sintax
        int idxLexema = convertTokenPositionToLexemaID(getCurrentPosOnStack());        
		if (lookahead("id") ) {
			if (lookahead("PV") ) {
                setStructIDType('i',idxLexema);
				return nonTerminalAccept();
			} else {
				pushLog("[VAR]", "PV");
				nonTerminalError();
			}
		} else {
			pushLog("[VAR]", "id");
			nonTerminalError();
		}
	} else {
		pushLog("[VAR]", "int");
		nonTerminalError();
	}
	if (lookahead("boolean") ) {
        int idxLexema = convertTokenPositionToLexemaID(getCurrentPosOnStack()); 
		if (lookahead("id") ) {
			if (lookahead("PV") ) {
                setStructIDType('b',idxLexema);
				return nonTerminalAccept();
			} else {
				pushLog("[VAR]", "PV");
				nonTerminalError();
			}
		} else {
			pushLog("[VAR]", "id");
			nonTerminalError();
		}
	} else {
		pushLog("[VAR]", "boolean");
		nonTerminalError();
	}

	pushLog("[VAR]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}


int METODO () {
	nonTerminalStart();
	if (lookahead("public") ) {
		if (lookahead("int") || lookahead("boolean")) {
			char methodReturnType;
			if(equalString(getRecognizedTokenKeyByIndex(lookback()), "int" )){
				methodReturnType='i';
			} else {
				methodReturnType='b';
			}
            int idxLexema=convertTokenPositionToLexemaID(getCurrentPosOnStack());
			if (lookahead("id") ) {
				if (lookahead("AP") ) {
					//semantica
					int firstTokenRead = lookback();					
					if (PARAMS(idxLexema) ) {				
						if (lookahead("FP") ) {
							//semantica
							int lastTokenRead = lookback();					
							//pushStackSemantica(firstTokenRead,lastTokenRead);
							if (lookahead("ACH") ) {
                                METODO_VAR:
								if (VAR() ) {
									goto METODO_VAR;
								} else {
                                    METODO_RETENTA_COMANDO: 
									if (CMD()) {
										goto METODO_RETENTA_COMANDO;
									} else {
										if (lookahead("return") ) {
											if (EXP() ) {
												if (lookahead("PV") ) {
													if (lookahead("FCH") ) {
                                                        setStructIDType(methodReturnType,idxLexema);
														return nonTerminalAccept();
													} else {
														pushLog("[METODO]", "FCH");
														nonTerminalError();
													}
												} else {
													pushLog("[METODO]", "PV");
													nonTerminalError();
												}
											} else {
												pushLog("[METODO]", "<TENTATIVA EXP()>");
												nonTerminalError();
											}
										} else {
											pushLog("[METODO]", "return");
											nonTerminalError();
										}
									}
								}
							} else {
								pushLog("[METODO]", "FP");
								nonTerminalError();
							}
						} else {
							pushLog("[METODO]", "FP");
							nonTerminalError();
						}
					} else {
						pushLog("[METODO]", "PARAMS()");
						nonTerminalError();
					}
				} else {
					pushLog("[METODO]", "AP");
					nonTerminalError();
				}
			} else {
				pushLog("[METODO]", "id");
				nonTerminalError();
			}
		} else {
			pushLog("[METODO]", "int ou boolean");
			nonTerminalError();
		}
	} else {
		pushLog("[METODO]", "public");
		nonTerminalError();
	}


	pushLog("[METODO]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}
int CLASSE() {
	nonTerminalStart();

	if (lookahead("class") ) {
        int idxLexema=convertTokenPositionToLexemaID(getCurrentPosOnStack());
		if (lookahead("id") ) {
			if (lookahead("extends") ) {
				if (lookahead("id") ) {
					goto APOS_EXTENDS;
				} else {
					pushLog("[CLASSE]", "id");
					nonTerminalError();
				}
			} else {
APOS_EXTENDS:
				if (lookahead("ACH") ) {
VAR_TESTE_AGAIN:
					if (VAR() ) {   
						goto VAR_TESTE_AGAIN;                                             
					} else {
						//CASO NAO HOUVER DECLARAO DE VAR OK
						if ( METODO() ) {
							if (lookahead("FCH") ) {
                                setStructIDType('c',idxLexema);
								return nonTerminalAccept();
							} else {
								pushLog("[CLASSE]", "FCH");
								nonTerminalError();
							}
						} else {
							pushLog("[CLASSE]", "<TENTATIVA METODO()>");
							nonTerminalError();
						}
					}
				} else {
					pushLog("[CLASSE]", "ACH");
					nonTerminalError();
				}
			}
		} else {
			pushLog("[CLASSE]", "id");
			nonTerminalError();
		}
	} else {
		pushLog("[CLASSE]", "class");
		nonTerminalError();
	}

	pushLog("[CLASSE]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}

int PEXP2() {
	nonTerminalStart();

	//PEXP . ID '(' [EXPS] ')'    
	if (lookahead("PONTO") ) {
		if (lookahead("id") ) {
			//semantica para validar parametro de input de metodo
			int firstTokenRead = lookback();	
			if (lookahead("AP") ) {
				if ( EXP() ) {
					if (lookahead("FP") ) {
						int lastTokenRead = lookback();	
						pushStackSemantica(firstTokenRead, lastTokenRead);
						return nonTerminalAccept();
					} else {
						pushLog("[PEXP2]", "FP");
						nonTerminalError();
					}
				} else {
					pushLog("[PEXP2]", "<TENTATIVA EXP(9)>");
					nonTerminalError();
				}
			} else {
				pushLog("[PEXP2]", "AP");
				nonTerminalError();
			}
		} else {
			pushLog("[PEXP2]", "id");
			nonTerminalError();
		}
	} else {
		pushLog("[PEXP2]", "PONTO");
		nonTerminalError();
	}

	pushLog("[PEXP2]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}

int  PEXP() {
	nonTerminalStart();

	if (lookahead("AP") ) {
		if (PEXP() ) {
			if (lookahead("FP") ) {
				return nonTerminalAccept();
			} else {
				pushLog("[PEXP]", "<FP");
				nonTerminalError();
			}
		} else {
			pushLog("[PEXP]", "<TENTATIVA PEXP(99)>");
			nonTerminalError();
		}
	} else {
		pushLog("[PEXP]", "AP");
		nonTerminalError();
	}

	if (lookahead("id") ) {
		return nonTerminalAccept();
	} else {
		pushLog("[PEXP]", "id");
		nonTerminalError();
	}

	//this
	if (lookahead("this") ) {		
		if (PEXP2() ) {
			return nonTerminalAccept(); 
		} else {
			pushLog("[PEXP]", "PEXP2(2)");
			nonTerminalError();            
		}
	} else {
		pushLog("[PEXP]", "this");
		nonTerminalError();
	}
	//new id ( ) . id ( exp )
	if (lookahead("new") ) {
		if (lookahead("id") ) {
			if (lookahead("AP") ) {
				if (lookahead("FP") ) {
					if ( PEXP2() ) {
						return nonTerminalAccept();
					} else {
						pushLog("[PEXP]", "<TENTATIVA PEXP2()>");
						nonTerminalError();
					}
				} else {
					pushLog("[PEXP]", "FP");
					nonTerminalError();
				}
			} else {
				pushLog("[PEXP]", "AP");
				nonTerminalError();
			}
		} else {
			pushLog("[PEXP]", "id");
			nonTerminalError();
		}
	} else {
		pushLog("[PEXP]", "new");
		nonTerminalError();
	}

	//new id ( )
	if (lookahead("new") ) {
		if (lookahead("id") ) {
			if (lookahead("AP") ) {
				if (lookahead("FP") ) {
					return nonTerminalAccept();
				} else {
					pushLog("[PEXP]", "FP");
					nonTerminalError();
				}
			} else {
				pushLog("[PEXP]", "AP");
				nonTerminalError();
			}
		} else {
			pushLog("[PEXP]", "id");
			nonTerminalError();
		}
	} else {
		pushLog("[PEXP]", "new");
		nonTerminalError();
	}


	pushLog("[PEXP]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}

int SEXP() {
	nonTerminalStart();

	if (lookahead("true") ) {
		return nonTerminalAccept();
	} else {
		pushLog("[SEXP]", "true");
		nonTerminalError();
	}

	//false
	if (lookahead("false") ) {
		return nonTerminalAccept();
	} else {
		pushLog("[SEXP]", "false");
		nonTerminalError();
	}
	//num
	if (lookahead("num") ) {
		return nonTerminalAccept();
	} else {
		pushLog("[SEXP]", "num");
		nonTerminalError();
	}
	//null
	if (lookahead("null") ) {
		return nonTerminalAccept();
	} else {
		pushLog("[SEXP]", "null");
		nonTerminalError();
	}
	//new int [ EXP ]
	if (lookahead("new") ) {
		if (lookahead("int") ) {
			if (lookahead("AC") ) {
				if (EXP()) {
					if (lookahead("FC")) {
						return nonTerminalAccept();
					} else {
						pushLog("[SEXP]", "FC");
						nonTerminalError();
					}                
				} else {
					pushLog("[SEXP]", "<TENTATIVA EXP(8)>");
					nonTerminalError();
				}                    
			} else {
				pushLog("[SEXP]", "AC");
				nonTerminalError();
			}                
		} else {
			pushLog("[SEXP]", "int");
			nonTerminalError();
		}            
	} else {
		pushLog("[SEXP]", "new");
		nonTerminalError();
	}

	pushLog("[SEXP]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}

int MEXP() {
	nonTerminalStart();
	//ID OU NUM * ID OU NUM
	if (lookahead("id") || lookahead("num") ) {
		if (lookahead("MULT")) {
			if (lookahead("id")  || lookahead("num")) {
                MEXP_GOTO:
                if (AEXP()) {
                    goto MEXP_GOTO;
                }
				return nonTerminalAccept();
			} else {
				pushLog("[MEXP]", "[1] id ou num");
				nonTerminalError();
			}   
		} else {
			pushLog("[MEXP]", "MULT");
			nonTerminalError();         
		}
	} else {
		pushLog("[MEXP]", "[2] id ou num");
		nonTerminalError();
	}

	if (lookahead("id") ) {
		if (lookahead("MULT")) {
			if ( PEXP() ) {
				return nonTerminalAccept();
			} else {
				pushLog("[MEXP]", "[1] id ou num");
				nonTerminalError();
			}   
		} else {
			pushLog("[MEXP]", "MULT");
			nonTerminalError();         
		}
	} else {
		pushLog("[MEXP]", "[2] id ou num");
		nonTerminalError();
	}

	pushLog("[MEXP]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}

int AEXP() {
	nonTerminalStart();
	//ID OU NUM + ID OU NUM
	if (lookahead("id") || lookahead("num") ) {
		if (lookahead("PLUS")) {
			if (lookahead("id")  || lookahead("num")) {
                AEXP_GOTO:
                if (AEXP()) {
                    goto AEXP_GOTO;
                }
				return nonTerminalAccept();
			} else {
				pushLog("[AEXP-1]", "[1] id ou num");
				nonTerminalError();
			}   
		} else {
			pushLog("[AEXP-1]", "PLUS");
			nonTerminalError();         
		}
	} else {
		pushLog("[AEXP-1]", "[2] id ou num");
		nonTerminalError();
	}

	//ID OU NUM - ID OU NUM
	if (lookahead("id") || lookahead("num") ) {
		if (lookahead("MINUS")) {
			if (lookahead("id")  || lookahead("num")) {
                AEXP_GOTO2:
                if (AEXP()) {
                    goto AEXP_GOTO2;
                }
				return nonTerminalAccept();
			} else {
				pushLog("[AEXP-2]", "[3] id ou num");
				nonTerminalError();
			}   
		} else {
			pushLog("[AEXP-2]", "MINUS");
			nonTerminalError();         
		}
	} else {
		pushLog("[AEXP-2]", "[4] id ou num");
		nonTerminalError();
	}
    // + id
    // - id
    // + num
    // - num
    if (lookahead("MINUS") || lookahead("PLUS") ) {
        if (lookahead("id") || lookahead("num")) {
            return nonTerminalAccept();
        } else {
            pushLog("[AEXP-3]", "MINUS");
            nonTerminalError();         
        }
    } else {
        pushLog("[AEXP-3]", "[5] id ou num");
        nonTerminalError();
    }

	pushLog("[AEXP]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}

int REXP() {
	nonTerminalStart();
	//REXP < AEXP
	if (lookahead("id") || lookahead("num") ) {
		if (lookahead("LT")) {
			if (lookahead("id")  || lookahead("num")) {
				return nonTerminalAccept();
			} else {
				pushLog("[REXP-1]", "[5]id ou num");
				nonTerminalError();
			}   
		} else {
			pushLog("[REXP-1]", "LT");
			nonTerminalError();         
		}
	} else {
		pushLog("[REXP-1]", "[6]id ou num");
		nonTerminalError();
	}

	//REXP == AEXP
	if (lookahead("id") || lookahead("num") ) {
		if (lookahead("LT")) {
			if (lookahead("id") || lookahead("num")) {
				return nonTerminalAccept();
			} else {
				pushLog("[REXP-2]", "[1] id ou num");
				nonTerminalError();      
			}   
		} else {
			pushLog("[REXP-2]", "LT");
			nonTerminalError();
		}
	} else {
		pushLog("[REXP-2]", "[2]id ou num");
		nonTerminalError();
	}

	//REXP != AEXP
	if (lookahead("id") || lookahead("num") ) {
		if (lookahead("DIF")) {
			if (lookahead("id") || lookahead("num")) {
				return nonTerminalAccept();
			} else {
				pushLog("[REXP-3]", "[3] id ou num");
				nonTerminalError();      
			}   
		} else {
			pushLog("[REXP-3]", "LT");
			nonTerminalError();
		}
	} else {
		pushLog("[REXP-3]", "[4]id ou num");
		nonTerminalError();
	}

	pushLog("[REXP]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}
int EXP() {
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

	pushLog("[EXP]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();
}

int CMD() {
	nonTerminalStart();

	//'{' {CMD} '}'
    if (lookahead("ACH")) {
        if (CMD()) {
            CMD_CMD:
            if (CMD()) {
                goto CMD_CMD;
            }
            if (lookahead("FCH")) {
                return nonTerminalAccept();
            } else {
                pushLog("[CMD-1]", "FCH");
                return nonTerminalRefuse();
            }
        } else {
            pushLog("[CMD-1]", "<TENTATIVA CMD(1)>");
            return nonTerminalRefuse();
        }
    } 

	//if '(' EXP ')' CMD else CMD
	if (lookahead("if")) {
		if (lookahead("AP")) {
			if (EXP()) {
				if (lookahead("FP")) {
					if (CMD()) {
						if (lookahead("else")) {
							if (CMD()) {
								return nonTerminalAccept();
							} else {
								pushLog("[CMD-2]", "<TENTATIVA CMD(2)>");
								return nonTerminalRefuse();
							}
						} else {
							pushLog("[CMD-2]", "else");
							return nonTerminalRefuse();
						}
					} else {
						pushLog("[CMD-2]", "<TENTATIVA CMD(3)>");
						return nonTerminalRefuse();
					}
				} else {
					pushLog("[CMD-2]", "FP");
					return nonTerminalRefuse();
				}
			} else {
				pushLog("[CMD-2]", "<TENTATIVA EXP(1)>");
				return nonTerminalRefuse();
			}
		} else {
			pushLog("[CMD-2]", "AP");
			return nonTerminalRefuse();
		}
	} 
	//if '(' EXP ')' CMD
	if (lookahead("if")) {
		if (lookahead("AP")) {
			if (EXP()) {
				if (lookahead("FCP")) {
					if (CMD()) {
						return nonTerminalAccept();
					} else {
						pushLog("[CMD-3]", "<TENTATIVA CMD(4)>");
						return nonTerminalRefuse();
					}
				} else {
					pushLog("[CMD-3]", "FCP");
					return nonTerminalRefuse();
				}
			} else {
				pushLog("[CMD-3]", "<TENTATIVA EXP(2)>");
				return nonTerminalRefuse();
			}
		} else {
			pushLog("[CMD-3]", "AP");
			return nonTerminalRefuse();
		}
	} 
	//while '(' EXP ')' CMD
	if (lookahead("while")) {
		if (lookahead("AP")) {
			if (EXP()) {
				if (lookahead("FCP")) {
					if (CMD()) {
						return nonTerminalAccept();
					} else {
						pushLog("[CMD-4]", "<TENTATIVA CMD(5)>");
						return nonTerminalRefuse();
					}
				} else {
					pushLog("[CMD-4]", "FCP");
					return nonTerminalRefuse();
				}
			} else {
				pushLog("[CMD-4]", "<TENTATIVA EXP(3)>");
				return nonTerminalRefuse();
			}
		} else {
			pushLog("[CMD-4]", "AP");
			return nonTerminalRefuse();
		}
	}
	//System.out.println '(' EXP ')' ;
	if (lookahead("System.out.println")) {
		if (lookahead("AP")) {
			if (EXP()) {
				if (lookahead("FP")) {
					if (lookahead("PV")) {
						return nonTerminalAccept();
					} else {
						pushLog("[CMD-5]", "PV");
						return nonTerminalRefuse();                    
					}
				} else {
					pushLog("[CMD-5]", "FP");
					return nonTerminalRefuse();                    
				}
			} else {
				pushLog("[CMD-5]", "<TENTATIVA EXP(4)>");
				return nonTerminalRefuse();
			}
		} else {
			pushLog("[CMD-5]", "AP");
			return nonTerminalRefuse();
		}
	} 
	//ID = EXP ;
	if (lookahead("id")) {
		int firstTokenRead = lookback();
		if (lookahead("ATR")) {
			if (EXP()) {
				if (lookahead("PV")) {
					int lastTokenRead = lookback();					
					pushStackSemantica(firstTokenRead,lastTokenRead);
					return nonTerminalAccept();
				} else {
					pushLog("[CMD-6]", "PV");
					return nonTerminalRefuse();
				}
			} else {
				pushLog("[CMD-6]", "<TENTATIVA EXP(5)>");
				return nonTerminalRefuse();
			}
		} else {
			pushLog("[CMD-6]", "ATR");
			return nonTerminalRefuse();
		}
	} 
    //aparentemente, nao eh utilizado. comentado
	//ID '[' EXP ']' = EXP ;
	/*if (lookahead("id")) {
		if (lookahead("AC")) {
			if (EXP()) {
				if (lookahead("FC")) {
					if (lookahead("ATR")) {
						if (EXP()) {
							if (lookahead("PV")) {
								return nonTerminalAccept();
							} else {
								pushLog("[CMD-7]", "PV");
								return nonTerminalRefuse();                        
							}
						} else {
							pushLog("[CMD-7]", "<TENTATIVA EXP(6)>");
							return nonTerminalRefuse();                        
						}
					} else {
						pushLog("[CMD-6]", "ATR");
						return nonTerminalRefuse();                        
					}
				} else {
					pushLog("[CMD-7]", "FC");
					return nonTerminalRefuse();
				}
			} else {
				pushLog("[CMD-7]", "<TENTATIVA EXP(7)>");
				return nonTerminalRefuse();
			}
		} else {
			pushLog("[CMD-7]", "AC");
			return nonTerminalRefuse();
		}
	}*/

	pushLog("[CMD]", "<nenhuma das derivacoes foi identificada>");
	return nonTerminalRefuse();

}

int MAIN() {
	nonTerminalStart();

	if (lookahead("class")){
        int idxLexema = convertTokenPositionToLexemaID(getCurrentPosOnStack());
		if (lookahead("id")) {
			if (lookahead("ACH")) {
				if (lookahead("public")) {
					if (lookahead("static")) {
						if (lookahead("void")) {
							if (lookahead("main")) {
								if (lookahead("AP")) {
									if (lookahead("id")) {
										if (lookahead("AC")) {
											if (lookahead("FC")) {
												if (lookahead("id")) {
													if (lookahead("FP")) {
														if (lookahead("ACH")) {
															if (CMD()) {
																if (lookahead("FCH")) {
																	if (lookahead("FCH")) {
                                                                        setStructIDType('c',idxLexema);
																		return nonTerminalAccept();
																	} else {
																		//FCH
																		pushLog("[MAIN]", "FCH");
																		return nonTerminalRefuse();
																	}
																} else {
																	//FCH
																	pushLog("[MAIN]", "FCH");
																	return nonTerminalRefuse();                                                                    
																}
															} else {
																//FP
																pushLog("[MAIN]", "CMD()");
																return nonTerminalRefuse();
															}
														} else {
															//ACH
															pushLog("[MAIN]", "ACH");
															return nonTerminalRefuse();                                                            
														}
													} else {
														//FP
														pushLog("[MAIN]", "FP");
														return nonTerminalRefuse();                                                        
													}
												} else {
													//id
													pushLog("[MAIN]", "id");
													return nonTerminalRefuse();                                                    
												}
											} else {
												//FC
												pushLog("[MAIN]", "FC");
												return nonTerminalRefuse();                                                
											}
										} else {
											//AC
											pushLog("[MAIN]", "AC");
											return nonTerminalRefuse();                                            
										}
									} else {
										//id
										pushLog("[MAIN]", "id");
										return nonTerminalRefuse();
									}
								} else {
									//AP
									pushLog("[MAIN]", "AP");
									return nonTerminalRefuse();                                     
								}                                
							} else {
								//main
								pushLog("[MAIN]", "main");
								return nonTerminalRefuse();                                
							}
						} else {
							//void
							pushLog("[MAIN]", "void");
							return nonTerminalRefuse();                            
						}                        
					} else {
						//static
						pushLog("[MAIN]", "static");
						return nonTerminalRefuse();                         
					}
				} else {
					//public
					pushLog("[MAIN]", "public");
					return nonTerminalRefuse();                    
				}
			} else {
				//ACH
				pushLog("[MAIN]", "ACH");
				return nonTerminalRefuse();                

			}
		} else {
			pushLog("[MAIN]", "id");
			return nonTerminalRefuse();
		}
	} else {
		pushLog("[MAIN]", "class");
		return nonTerminalRefuse();
	}



}

int PROG() {
	nonTerminalStart();

	if (MAIN()){
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

void AnalisadorSintatico(){
	if (PROG()) {
        printf("/**************************************\n\tSINTAX OK\n*************************************/\n");
	} else {
		printf("/**************************************\n\tSINTAX ERROR\n\tPILHA DE ERRO\n*************************************/\n");
		while (popLog()) {
			NULL;
		}
		exit(-1);
	}
}

/*******************************************
  Biblioteca de TOKENs
 *******************************************/
void tokenLibrary(char * tk) {
	//
	// PALAVRAS RESERVADAS
	//
	if (strcmp("if",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<if>");
		return;
	} else if (strcmp("while",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<while>");
		return;
	} else if (strcmp("this",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<this>");
		return;
	} else if (strcmp("new",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<new>");
		return;
	} else if (strcmp("else",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<else>");
		return;
	} else if (strcmp("System.out.println",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<System.out.println>");
		return;
	} else if (strcmp("boolean",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<boolean>");
		return;
	} else if (strcmp("class",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<class>");
		return;
	} else if (strcmp("extends",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<extends>");
		return;
	} else if (strcmp("public",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<public>");
		return;
	} else if (strcmp("static",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<static>");
		return;
	} else if (strcmp("void",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<void>");
		return;
	} else if (strcmp("main",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<main>");
		return;
	} else if (strcmp("return",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<return>");
		return;
	} else if (strcmp("int",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<int>");
		return;
	}
	//
	// PONTUACAO
	//
	else if (strcmp("(",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<AP>");		
		return;
	} else if (strcmp(")",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<FP>");		
		return;
	} else if (strcmp("[",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<AC>");		
		return;
	} else if (strcmp("]",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<FC>");		
		return;
	} else if (strcmp("{",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<ACH>");		
		return;
	} else if (strcmp("}",tk)==0) {		
		write_plp("<FCH>");		
		return;
	} else if (strcmp(".",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<PONTO>");		
		return;
	} else if (strcmp(",",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<VIRG>");       
		return;
	} else if (strcmp(";",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<PV>");       
		return;
	}
	//
	// OPERADORES
	//
	else if (strcmp("==",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<EQ>");		
		return;
	} else if (strcmp("&&",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<AND>");
		return;
	} else if (strcmp("-",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<MINUS>");		
		return;
	} else if (strcmp("+",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<PLUS>");		
		return;
	} else if (strcmp("*",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<MULT>");		
		return;
	} else if (strcmp("!",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<NOT>");		
		return;
	} else if (strcmp("=",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<ATR>");		
		return;
	} else if (strcmp("!=",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<DIF>");		
		return;
	} else if (strcmp("<",tk)==0) {
		//printf("RECONHECIDO: %s \n", tk );
		write_plp("<LT>");		
		return;
	}
	//
	// NUMEROS
	//  
	int i;
	char number[1024];
	//Limpa area de memoria. Necessario para concatenacao de Numero realizada mais abaixo
	memset(number,0, 1024);
	int index=0;
	int totalTimesCountNumber=0;
	int totalTimesIterate=0;



	for (i=0; tk[i]!='\0'; i++){        
		if (tk[i]=='0') {   
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='1') {
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='2') {
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='3') {
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='4') {
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='5') {
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='6') {
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='7') {
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='8') {
			number[index++]=tk[i];
			totalTimesCountNumber++;

		} else if (tk[i]=='9') {
			number[index++]=tk[i];
			totalTimesCountNumber++;
		} 
		totalTimesIterate++;
	}

	if (totalTimesCountNumber==totalTimesIterate) {
		char n[1024];
		strcpy(n, "<num,");
		strcat(n, number);
		strcat(n, ">"); 
		write_plp(n); 
		//identificou numero, portanto, deve parar execucao
		return;
	}
	//
	// VARIAVEIS
	//
	int primeiraLeitura=1;
	char variavel[1024];
	index=0;
	memset(variavel,0,1024);
	for (i=0; tk[i]!='\0'; i++){
		if (tk[i]=='a' || tk[i]=='b' ||tk[i]=='c' ||tk[i]=='d' ||tk[i]=='e' ||tk[i]=='f' ||tk[i]=='g' ||
				tk[i]=='h' ||tk[i]=='i' ||tk[i]=='j' ||tk[i]=='k' ||tk[i]=='l' ||tk[i]=='m' ||tk[i]=='n' ||
				tk[i]=='o' ||tk[i]=='p' ||tk[i]=='q' ||tk[i]=='r' ||tk[i]=='r' ||tk[i]=='s' ||tk[i]=='t' ||
				tk[i]=='u' ||tk[i]=='v' ||tk[i]=='x' ||tk[i]=='z' ||tk[i]=='y' ||tk[i]=='w' ||
				tk[i]=='A' || tk[i]=='B' ||tk[i]=='C' ||tk[i]=='D' ||tk[i]=='E' ||tk[i]=='F' ||tk[i]=='G' ||
				tk[i]=='H' ||tk[i]=='I' ||tk[i]=='J' ||tk[i]=='K' ||tk[i]=='L' ||tk[i]=='M' ||tk[i]=='N' ||
				tk[i]=='O' ||tk[i]=='P' ||tk[i]=='Q' ||tk[i]=='R' ||tk[i]=='R' ||tk[i]=='S' ||tk[i]=='T' ||
				tk[i]=='U' ||tk[i]=='V' ||tk[i]=='X' ||tk[i]=='Z' ||tk[i]=='Y' ||tk[i]=='W' 
		   ) 
		{
			//
			primeiraLeitura=0;
			//
			variavel[index++]=tk[i];

		} else if (tk[i]=='0'||tk[i]=='1'||tk[i]=='2'||tk[i]=='3'||tk[i]=='4'||tk[i]=='5'||tk[i]=='6'||
				tk[i]=='7'||tk[i]=='8'||tk[i]=='9'|| tk[i]=='_')
		{
			if (primeiraLeitura==1){
				//Leu um caracter diferente de ALPHA como primeiro caracter entao nao é variavel
				printf("ERRO LEXICO: %s\n", tk);
				exit(-1);
			}
			primeiraLeitura=0;
			variavel[index++]=tk[i];

		} else {
			printf("ERRO LEXICO: %s\n", tk);
			exit(-1);
		}

	}
	char n[1024];
	int idIndex =addStructID(variavel);
	char idIndexString[2];
	sprintf(idIndexString, "%d", idIndex);
	strcpy(n, "<id,");
	strcat(n, idIndexString);
	strcat(n, ">"); 
	write_plp(n);   
	return; 

}
