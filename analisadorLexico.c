/*
TODO:
- fazer a matriz para guardar os id's 
- fazer uma função para gravar o token no arquivo
- fazer a compilação parar quando encontrar um erro
- ignorar comentarios

----------------   PROJETO DE LINGUAGEM DE PROGRAMAÇÃO ---------------------------------------------------*/


#define MAX 30

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void imprimeMatriz(char** g, int m, int n);

void imprimeMatriz(char** g, int m, int n){
	int lin, col;
	for(lin=0; lin<m; lin++){
		for(col=0; col<n; col++){
			printf("%c \t", g[lin][col]);
		}
		printf("\n");
	}

}

/* Função verifica é chamada no main e recebe como parâmetro uma palavra. Esta função simula o funcionamento do automato, ou seja, faz a analise de 
palavra recebida e verifica se esta palavra pode ser aceita, e se for, qual será sua classificação */

char ** MATRIXinit (int r, int c);
char ** MATRIXinit (int r, int c)
{
	char **t = malloc(r*sizeof(char*)); //t é ponteiro para ponteiro para inteiro (vetor de vetores = matriz) e o r é o tamanho desse vetor
	char i, j;
		for (i=0; i<r; i++) //i = linha (mais uma alocação no vetor) // linha por linha vai criando as colunas
			t[i] = malloc(c*sizeof(char)); //
		for(i=0; i<r; i++){ //tudo já está criado
			for(j=0; j<c; j++){
				t[i][j] = '1';
			}
		}
	return t;	
}

int gravaIdMatriz(char** matriz, char* str);

int gravaIdMatriz(char** matriz, char* str){
    
    int tamanho = strlen(str);

    int ultima = 0;

    int y;
    for(y = 0; y < MAX; y++){

        if(matriz[y][0]=='1'){
            ultima = y;

            printf("Ultima: %d \n", ultima);
            break;
        }
      

    }
    
    int i,j;
    int a = 0;
    for (i = 0; i <= ultima; i++){
        for (j = 0; j< tamanho; j++){
            if((matriz[i][j] == str[j])){
              if(j == tamanho-1){
                printf("ENTREI DUPLO FOR linha %d \n", i);
                imprimeMatriz(matriz,10,10);
                return i;
              }
            }
        }    
    }
    
    printf("i: %d \n", i);
    //i -= 1;
    int k;
    for (k = 0; k< tamanho; k++){
           matriz[ultima][k] = str[k];
    }
    
    imprimeMatriz(matriz,10,10);
    
    return ultima;
}

int verifica(char* str);

int verifica(char* str){
    
    //Abrindo arquivo para gravação
    FILE * fr;

    fr = fopen ("saida.txt", "a");
    if(fr == NULL){
      perror("Error opening file");
      return(-1);
    }

   //variáveis de controle
   int tamanho = strlen(str);
   int i = 0; 


//-----------------------------------------  INICIO DA VALIDAÇÃO DOS CARACTERES  ------------------------------------------//

   //Q0 - ESTADO INICIAL ONDE CADA LETRA DA PALAVRA É ANALISADA E SE ACEITA, É DIRECIONADA PARA O ESTADO DA SUA RESPECTIVA CLASSE PARA PROSSEGUIR COM A VALIDAÇÃO
   
   // Verifica se a Primeira letra da Palavra recebida pertence a uma palavra reservadas 
   if(str[i]=='S'){ i++; goto q1;} 
   if(str[i]=='s'){ i++; goto q24;}
   if(str[i]=='w'){ i++; goto q30;}
   if(str[i]=='e'){ i++; goto q35;} 
   if(str[i]=='b'){ i++; goto q39;}
   if(str[i]=='c'){ i++; goto q46;}
   if(str[i]=='v'){ i++; goto q58;}
   if(str[i]=='p'){ i++; goto q62;}
   if(str[i]=='m'){ i++; goto q68;}
   if(str[i]=='r'){ i++; goto q72;}
   if(str[i]=='i'){ i++; goto q78;}
   if(str[i]=='l'){ i++; goto q82;}
   if(str[i]=='t'){ i++; goto q88;}
   if(str[i]=='f'){ i++; goto q95;}
   if(str[i]=='n'){ i++; goto q100;}

   // Verifica se o primeiro caracter da palavra recebida é um Numero 
   if(str[i]=='0'||str[i]=='1'||str[i]=='2'||str[i]=='3'||str[i]=='4'||str[i]=='5'||str[i]=='6'||str[i]=='7'||str[i]=='8'||str[i]=='9'){ 
    	goto q106;
   }

   // Verifica se o primeiro caracter da palavra recebida é um pontuação 
   if(str[i]=='{'||str[i]=='}'||str[i]==';'||str[i]=='('||str[i]==')'||str[i]=='.'||str[i]==','||str[i]=='['||str[i]==']'){ 
       goto q107;
   }
   // Verifica se o primeiro caracter da palavra recebida é um espaço em branco 
   if(str[i]=='\n'||str[i]==' '||str[i]=='\t'||str[i]=='\r'||str[i]=='\f'||str[i]=='\0'){ 
      goto q108;
   }
    // Verifica se o primeiro caracter da palavra recebida é um operador 
   if(str[i]=='*'||str[i]=='-'||str[i]=='+'||str[i]=='<'||str[i]=='='||str[i]=='!'||str[i]=='&'){ 
		goto q111;
   }
   
   // Verifica se o primeiro caracter da palavra recebida é um caracter válido para uma variavel 
   if(str[i]=='a'||str[i]=='d'||str[i]=='g'||str[i]=='h'||str[i]=='j'||str[i]=='k'||str[i]=='o'||str[i]=='q'||str[i]=='u'||str[i]=='x'||str[i]=='y'||str[i]=='z'||
      str[i]=='A'||str[i]=='B'||str[i]=='C'||str[i]=='D'||str[i]=='E'||str[i]=='F'||str[i]=='G'||str[i]=='H'||str[i]=='I'||str[i]=='J'||str[i]=='K'||str[i]=='L'||
	  str[i]=='M'||str[i]=='N'||str[i]=='O'||str[i]=='P'||str[i]=='Q'||str[i]=='R'||str[i]=='T'||str[i]=='U'||str[i]=='V'||str[i]=='W'||str[i]=='X'||str[i]=='Y'||
	  str[i]=='Z'){ 
    	goto q113;
   }
   
   // Caso não reconheça nenhum caracter listado acima, gera erro.
   else {
     printf("Erro 111\n");
       return-1;
   }
   
    
   
//--------------------------- INICIO DOS ESTADOS  ------------------------------------------//
  
  // Cada estado faz a validação de toda a palavra e decide se esta palavra foi aceita 
  
  //PALAVRAS RESERVADAS
  // System.out.println
    
   q1: 
   if(str[i]=='y'){ i++; goto q2;}
   if(str[i]=='t'){ i++; goto q19;}else{goto q113;} 

   q2: 
   if(str[i]=='s'){ i++; goto q3;}else{goto q113;}

   q3: 
   if(str[i]=='t'){ i++; goto q4;}else{goto q113;}

   q4: 
   if(str[i]=='e'){ i++; goto q5;}else{goto q113;}

   q5: 
   if(str[i]=='m'){ i++; goto q6;}else{goto q113;}

   q6: 
   if(str[i]=='.'){ i++; goto q7;}else{goto q113;}

   q7: 
   if(str[i]=='o'){ i++; goto q8;}else{goto q113;}

   q8: 
   if(str[i]=='u'){ i++; goto q9;}else{goto q113;}

   q9: 
   if(str[i]=='t'){ i++; goto q10;}else{goto q113;}

   q10: 
   if(str[i]=='.'){ i++; goto q11;}else{goto q113;}

   q11: 
   if(str[i]=='p'){ i++; goto q12;}else{goto q113;}

   q12: 
   if(str[i]=='r'){ i++; goto q13;}else{goto q113;}

   q13: 
   if(str[i]=='i'){ i++; goto q14;}else{goto q113;}

   q14: 
   if(str[i]=='n'){ i++; goto q15;}else{goto q113;}

   q15: 
   if(str[i]=='t'){ i++; goto q16;}else{goto q113;}

   q16: 
   if(str[i]=='l'){ i++; goto q17;}else{goto q113;}

   q17: 
   if(str[i]=='n'){ 
      if(i+1 == tamanho){	  
	  	i++;
      	printf("<System.out.println> %s \n", str);
        //grava no arquivo
        fprintf (fr, "%s \n" , str);
        fclose(fr);
     	return -1;
  		}else{
		  goto q113;
		}
	}else{goto q113;}

   
   // String
   
   q19: 
   if(str[i]=='r'){ i++; goto q20;}else{goto q113;}

   q20: 
   if(str[i]=='i'){ i++; goto q21;}else{goto q113;}

   q21: 
   if(str[i]=='n'){ i++; goto q22;}else{goto q113;}

   q22: 
   if(str[i]=='g'){ 
    if(i+1 == tamanho){
      i++;
      printf("<String> \n"); 
      //gravar na matriz e no arquivo
      fprintf (fr, "%s \n" , str);
      fclose(fr);
      return -1;
     }else{
        goto q113;
	 }
   }else {goto q113;}

  // static
  
   q24: 
   if(str[i]=='t'){ i++; goto q25;}else{goto q113;}

   q25: 
   if(str[i]=='a'){ i++; goto q26;}else{goto q113;}

   q26: 
   if(str[i]=='t'){ i++; goto q27;}else{goto q113;}

   q27: 
   if(str[i]=='i'){ i++; goto q28;}else{goto q113;}

   q28: 
   if(str[i]=='c'){ 
        if(i+1 == tamanho){
          i++; 
          printf("<static> \n");
          return -1;
          //gravar na matriz e no arquivo
        }else{
            goto q113;
	    }
   }else {goto q113;}

   // while
   
   q30: 
   if(str[i]=='h'){ i++; goto q31;}else{goto q113;}

   q31: 
   if(str[i]=='i'){ i++; goto q32;}else{goto q113;}

   q32: 
   if(str[i]=='l'){ i++; goto q33;}else{goto q113;}

   q33: 
   if(str[i]=='e'){
      if(i+1 == tamanho){ 
          i++; 
          printf("<while> \n");
          return -1;
          //gravar na matriz e no arquivo
        }else{
            goto q113;
	    }
   }
   else {goto q113;}

  //else
  
   q35: 
   if(str[i]=='l'){ i++; goto q36;}
   if(str[i]=='x'){ i++; goto q52;}else{goto q113;}

   q36: 
   if(str[i]=='s'){ i++; goto q37;}else{goto q113;}

   q37: 
   if(str[i]=='e'){
              if(i+1 == tamanho){  
      i++; 
      printf("<else> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

  //extends
  
   q52: 
   if(str[i]=='t'){ i++; goto q53;}else{goto q113;}

   q53: 
   if(str[i]=='e'){ i++; goto q54;}else{goto q113;}

   q54: 
   if(str[i]=='n'){ i++; goto q55;}else{goto q113;}

   q55: 
   if(str[i]=='d'){ i++; goto q56;}else{goto q113;}

   q56: 
   if(str[i]=='s'){
        if(i+1 == tamanho){  
      i++; 
      printf("<extends> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //Boolean
   q39: 
   if(str[i]=='o'){ i++; goto q40;}else{goto q113;}

   q40: 
   if(str[i]=='o'){ i++; goto q41;}else{goto q113;}

   q41: 
   if(str[i]=='l'){ i++; goto q42;}else{goto q113;}

   q42: 
   if(str[i]=='e'){ i++; goto q43;}else{goto q113;}

   q43: 
   if(str[i]=='a'){ i++; goto q44;}else{goto q113;}

   q44: 
   if(str[i]=='n'){
        if(i+1 == tamanho){  
      i++; 
      printf("<boolean> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

  //class
   q46: 
   if(str[i]=='l'){ i++; goto q47;}else{goto q113;}

   q47: 
   if(str[i]=='a'){ i++; goto q48;}else{goto q113;}

   q48: 
   if(str[i]=='s'){ i++; goto q49;}else{goto q113;}

   q49: 
   if(str[i]=='s'){
        if(i+1 == tamanho){ 
      i++; 
      printf("<class> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //void
   q58: 
   if(str[i]=='o'){ i++; goto q59;}else{goto q113;}

   q59: 
   if(str[i]=='i'){ i++; goto q60;}else{goto q113;}

   q60: 
   if(str[i]=='d'){
        if(i+1 == tamanho){  
      i++; 
      printf("<void> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //public
   q62: 
   if(str[i]=='u'){ i++; goto q63;}else{goto q113;}

   q63: 
   if(str[i]=='b'){ i++; goto q64;}else{goto q113;}

   q64: 
   if(str[i]=='l'){ i++; goto q65;}else{goto q113;}

   q65: 
   if(str[i]=='i'){ i++; goto q66;}else{goto q113;}

   q66: 
   if(str[i]=='c'){
        if(i+1 == tamanho){ 
      i++; 
      printf("<public> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

    //main
   q68: 
   if(str[i]=='a'){ i++; goto q69;}else{goto q113;}

   q69: 
   if(str[i]=='i'){ i++; goto q70;}else{goto q113;}

   q70: 
   if(str[i]=='n'){
        if(i+1 == tamanho){ 
      i++; 
      printf("<main> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //return
   q72: 
   if(str[i]=='e'){ i++; goto q73;}else{goto q113;}

   q73: 
   if(str[i]=='t'){ i++; goto q74;}else{goto q113;}

   q74: 
   if(str[i]=='u'){ i++; goto q75;}else{goto q113;}

   q75: 
   if(str[i]=='r'){ i++; goto q76;}else{goto q113;}

   q76: 
   if(str[i]=='n'){
        if(i+1 == tamanho){ 
      i++; 
      printf("<return> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //int
   q78: 
   if(str[i]=='n'){ i++; goto q79;}
   if(str[i]=='f'){ 
        if(i+1 == tamanho){
      i++; 
      printf("<if> \n");
      return -1;
   }else {goto q113;}

   q79: 
   if(str[i]=='t'){ 
      i++; 
      printf("<int> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //length
   q82: 
   if(str[i]=='e'){ i++; goto q83;}else{goto q113;}

   q83: 
   if(str[i]=='n'){ i++; goto q84;}else{goto q113;}

   q84: 
   if(str[i]=='g'){ i++; goto q85;}else{goto q113;}

   q85: 
   if(str[i]=='t'){ i++; goto q86;}else{goto q113;}

   q86: 
   if(str[i]=='h'){
        if(i+1 == tamanho){ 
      i++; 
      printf("<length> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //true
   q88: 
   if(str[i]=='r'){ i++; goto q89;}
   if(str[i]=='h'){ i++; goto q92;}else{goto q113;}

   q89: 
   if(str[i]=='u'){ i++; goto q90;}else{goto q113;}

   q90: 
   if(str[i]=='e'){
        if(i+1 == tamanho){ 
      i++; 
      printf("<true> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}
   
   //this
   q92: 
   if(str[i]=='i'){ i++; goto q93;}else{goto q113;}

   q93: 
   if(str[i]=='s'){
        if(i+1 == tamanho){ 
      i++; 
      printf("<this> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}
   
   //false
   q95: 
   if(str[i]=='a'){ i++; goto q96;}else{goto q113;}

   q96: 
   if(str[i]=='l'){ i++; goto q97;}else{goto q113;}

   q97: 
   if(str[i]=='s'){ i++; goto q98;}else{goto q113;}

   q98: 
   if(str[i]=='e'){
        if(i+1 == tamanho){ 
      i++; 
      printf("<false> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //new
   q100: 
   if(str[i]=='e'){ i++; goto q101;}
   if(str[i]=='u'){ i++; goto q103;}else{goto q113;}

   q101: 
   if(str[i]=='w'){
        if(i+1 == tamanho){  
      i++; 
      printf("<new> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}

   //null
   q103: 
   if(str[i]=='l'){ i++; goto q104;}else{goto q113;}

   q104: 
   if(str[i]=='l'){ 
        if(i+1 == tamanho){ 
      i++; 
      printf("<null> \n");
      return -1;
      //gravar na matriz e no arquivo
      }else{
            goto q113;
	    }
   }
   else {goto q113;}
   
   	
   	//Numeros
   	q106:
   if(i=tamanho){
      //gravar numero
      printf("<NUM, VALOR> \n");
      return -1;
   }else if(str[i]=='0'||str[i]=='1'||str[i]=='2'||str[i]=='3'||str[i]=='4'||str[i]=='5'||str[i]=='6'||str[i]=='7'||str[i]=='8'||str[i]=='9'){ 
      i++; 
      goto q106;
   }else{
           printf("Erro");
           return -1;
	    }
    
	
    //pontuação
   q107:
   if(str[i]=='{'){
   		printf("<PONT, AC> \n");
    	 i++;
		 return -1;
   }else if(str[i]=='}'){
   		printf("<PONT, FC> \n");
    	 i++;
		 return -1;
   }else if (str[i]==';'){
   		printf("<pont, PV> \n");
   		i++;
		return -1;
   }else if (str[i]=='('){
   		printf("<PONT, PA> \n");
  		 i++;
		return -1;
   }else if (str[i]==')'){
   		printf("<PONT, PF> \n");
    	 i++;
		 return -1;
   }else if(str[i]=='.'){
   		printf("<PONT, PT> \n");
    	i++;
	 	return -1;
   }else if(str[i]==','){
   		printf("<PONT, VG> \n");
   		i++;
		return -1;
	}else if (str[i]=='['){
		printf("<PONT, CHA> \n");
    	 i++;
		 return -1;
	}else if (str[i]==']'){ 
		printf("<PONT, CHF> \n");
    	 i++;
		return -1;
   }else{
      	printf("<ERRO PONTUAÇÃO> \n");
      	i++;
      	return -1;
	  }
	  
	//ignora quebra de linha 
	q108:
   	i++;
   	return -1;
    	
    	
   //operadores
   q111:
   	if(str[i]=='*'){ 
      i++; 
      printf("<OP, MULT> \n");
      return -1;
      //gravar na matriz e no arquivo
   } else if(str[i]=='-'){ 
      i++; 
      printf("<OP, MIN> \n");
      return -1;
      //gravar na matriz e no arquivo
   } else if(str[i]=='+'){ 
      i++; 
      printf("<OP, SUM> \n");
      return -1;
      //gravar na matriz e no arquivo
   } else if(str[i]=='<'){ 
      i++; 
      printf("<OP, LT> \n");
      return -1;
      //gravar na matriz e no arquivo
   }  else if(str[i]=='!'){ 
    	if(str[i+1]=='='){
        //grava !=
        	printf("<OP, DIF> \n");
        	i++;
        	return -1;
     	}else if((i+1) == tamanho){
          		//grava !
           		printf("<OP, EXC> \n");   
           		return -1;
       		}else{
            		printf("ERRO! \n");
            		return -1;
          	}
    } else if(str[i]=='='){ 
      if(str[i+1]=='='){
        //grava ==
         printf("<OP, 2EQ> \n");
         i++;
         return -1;
      }else if((i+1) == tamanho){
            //grava !
            printf("<OP, EQ> \n");   
            return -1;
         }else{
            printf("TAMANHO: %d \n",tamanho);
            printf("i: %d \n",i);
            return -1;
         }
      } else if(str[i]=='&'){ 
      if(str[i+1]=='&'){
        //grava &&
         printf("<OP, AND> \n");
         i++;
         return -1;
      }else{
        printf("ERRO! \n");
         return -1;
      }
   } else {
   	printf("AC - ERRO OP \n");
	   return -1;  
   }	
   	
   	//Variaveis 
   q113:
   	 if(i=tamanho){
      //gravar id na matriz 
 		printf("<ID, XX> \n");
       return -1; 
	   }else if (str[i]=='a'||str[i]=='b'||str[i]=='c'||str[i]=='d'||str[i]=='e'||str[i]=='f'||str[i]=='g'||str[i]=='i'||str[i]=='h'||str[i]=='j'||
	   str[i]=='k'||str[i]=='l'||str[i]=='m'||str[i]=='n'||str[i]=='o'||str[i]=='p'||str[i]=='q'||str[i]=='r'||str[i]=='s'||str[i]=='t'||str[i]=='u'||
	   str[i]=='v'||str[i]=='x'||str[i]=='w'||str[i]=='y'||str[i]=='z' || 
	   str[i]=='A'||str[i]=='B'||str[i]=='C'||str[i]=='D'||str[i]=='E'||str[i]=='F'||str[i]=='G'||str[i]=='H'||str[i]=='I'||str[i]=='J'||str[i]=='K'||
	   str[i]=='L'||str[i]=='M'||str[i]=='N'||str[i]=='O'||str[i]=='P'||str[i]=='Q'||str[i]=='R'||str[i]=='S'||str[i]=='T'||str[i]=='U'||str[i]=='V'||
	   str[i]=='W'||str[i]=='X'||str[i]=='Y'||str[i]=='Z'||
	   str[i]=='0'||str[i]=='1'||str[i]=='2'||str[i]=='3'||str[i]=='4'||str[i]=='5'||str[i]=='6'||str[i]=='7'||str[i]=='8'||str[i]=='9'||
	   str[i]=='_'){ 
	   i++;
       goto q113;
      
  }    else {printf("AC - ERRO ID \n");}

}


//-------------------------------------------- MAIN -----------------------------------------------------------------//
int main()
{
    
    int cont = 0;
    char **matriz = MATRIXinit(MAX,MAX); 
    
    int i = gravaIdMatriz(matriz, "fer");
    int j = gravaIdMatriz(matriz, "fyr");
    int f = gravaIdMatriz(matriz, "fer");
    int g = gravaIdMatriz(matriz, "sam");
    int h = gravaIdMatriz(matriz, "dan");
    int s = gravaIdMatriz(matriz, "sam");
    int t = gravaIdMatriz(matriz, "fer");
    int u = gravaIdMatriz(matriz, "fov");
    int v = gravaIdMatriz(matriz, "sam");
    int w = gravaIdMatriz(matriz, "fov");
    int x = gravaIdMatriz(matriz, "sam");
    
    printf("i: %d  j: %d f: %d", i, j, f);
    
    /*
   FILE *fp;
   char str[2000];

   //opening file for reading
   fp = fopen("teste.txt" , "r");
   if(fp == NULL) 
   {
      perror("Error opening file");
      return(-1);
   }
   
   const char s[2] = " ";
   char *token;
   
   

   while (!feof(fp)){
         
         if( fgets (str, 2000, fp)!=NULL ){

            token = strtok(str, s);
            
			//verifica se chegou ao final da linha e caso tenha chegado, marca com \0
            int tam = strlen(str);
            if(str[tam-1]== '\n'){
            	str[tam-1]= '\0';
			}

            while(token != NULL){
               	verifica(token);
                  
                token = strtok(NULL, s);
			}
			   

       }

   }

   
      
   fclose(fp);
   
   */
   return(0);
   
}
