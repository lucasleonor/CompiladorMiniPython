#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenValidators.h"
#include "lexico.h"
#include "semantico.h"

#define FILE_LEX_IN           "in.txt"
#define FILE_LEX_OUT          "tokens.txt"
#define FILE_SYNTACTIC_OUT    "sintatico.txt"
#define ERRO                  printf("\nERRO SINTATICO, %d - %s", lineNumber, line);fprintf(out, "ERRO SINTATICO, %d - %s\n", lineNumber, line);exit(2);
//#define DEBUG_ALL             1
//#define DEBUG                 1

typedef enum {INTEIRO = 0, REAL, BOOLEANO, LITERAL, TUPLA, LISTA, FUNCAO, VAZIO, NI} Tipo;
typedef struct Lista{
    Tipo *elementos;
    int numElementos;
}Lista;

FILE *in, *out;
Tipo *identificadores;
Lista *listIds;
Lista temp;
char *type;
char *value;
char *line;
int lineNumber = 0;
char *tipos[9] = {"Int", "Real", "Boolean", "String", "Tupla", "Lista", "Func", "Null", "Nao Inicializado"};

 #ifdef DEBUG_ALL
int tab = 0;
char* getTabs() {
    return "";
    int i = 0;
    char *str = calloc(200, sizeof(char));
    for(i; i < tab; i++) {
        strcat(str, "\t");
    }
    str[i]='\0';
    return str;
}
 #endif

int isTest();
Tipo test();
int isFactor();
Tipo factor();
int isStatement();
void statement();
int isFuncStatement();
void funcStatement();
int isExpr();
Tipo expr();

Lista addElementToLista(Lista lista, Tipo elemento){
    int size = lista.numElementos;
    Tipo *listaAux = malloc(size+1*sizeof(Tipo));
    int i;
    for (i = 0; i < size; ++i){
        listaAux[i] = lista.elementos[i];
    }
    listaAux[size] = elemento;
    lista.elementos = listaAux;
    lista.numElementos = size+1;
    return lista;
}

void printLista(Lista lista){
    int i;
    printf("Lista{\n\tnumElementos:%d\n\telementos:{\n", lista.numElementos);
    for (i = 0; i < lista.numElementos; ++i){
        printf("\t\tTipo:%s\n", tipos[lista.elementos[i]]);
    }
    printf("\t}\n}\n");
}

int strToInt(char* str){
    int integer = 0, i;
    if(!strcmp(str, "True")){
        integer = 1;
    }else if(!strcmp(str, "False")){
        integer = 0;
    }else{
        for (i = 0; i < strlen(str); ++i){
            if(isdigit(str[i])){
                integer = (integer * 10) + (str[i]-48);
            }else{
                printf("NOT A NUMBER\n");
                break;
            }
        }
    }
 #ifdef DEBUG_ALL
    printf("strToInt str:%s\tint:%d\n", str, integer);
 #endif
    return integer;
}

char *getTokenType(char *token) {
    if(token[0]=='\0'){
        return token;
    }
    int i = 1;
    while (token[i] != '\0' && token[i] != ',')i++;
    i--;
    char *temp = malloc(20);
    memcpy(temp, &token[1], (size_t) i);
    temp[i] = '\0';
 #ifdef DEBUG_ALL
    printf("%sType: %d, %s\n",getTabs(), i, temp);
 #endif
    return temp;
}

char *getTokenValue(char *token) {
    if(token[0]=='\0'){
        return token;
    }
    int ini = 1, end;
    while (token[ini] != '\0' && token[ini] != ',')ini++;
    end = ++ini;
    while (token[end] != '\0' && token[end] != '>')end++;
    end -= ini;
    char *temp = malloc(20);
    memcpy(temp, &token[ini], (size_t) end);
    temp[end] = '\0';
 #ifdef DEBUG_ALL
    printf("%sValue: %d, %d, %s\n", getTabs(), ini, end, temp);
 #endif
    return temp;
}

void nextLine() {
    char *token = malloc(50);
    fgets(token, 50, in);
    if (strcmp(token, "ERRO LEXICO\n") == 0) {
        fprintf(out, "%s", token);
        printf("\n%s\n", token);
        exit(1);
    }
    if (token[0] == '\0' || token[0] == '\n') {
        type[0] = '\0';
        value[0] = '\0';
        line = token;
    } else {
        line = token;
        type = getTokenType(token);
        value = getTokenValue(token);
        lineNumber++;
 #ifdef DEBUG_ALL
        printf("%sGot line: type[%s], value[%s]. Line: %s", getTabs(), type, value, line);
 #endif
    }
}

int endOfFile(){
    if(feof(in)){
        return 1;
    }
    //printf("[%s]\n", line);
    if(line[0]=='\0' || line[0] == '\n'){
        return 1;
    }
    return 0;
}

void goBack() {
    int len = (int) (strlen(line) + 1);
 #ifdef DEBUG_ALL
    printf("%sGo Back: %d \n", getTabs(), len);
 #endif
    fseek(in, len * -1, SEEK_CUR);
    lineNumber--;
}

int isBoolean(){
    return isTrue() || isFalse();
}

Tipo boolean(){
    if(isTrue()){
 #ifdef DEBUG
        printf(" True ");
 #endif
    }else if(isFalse()){
 #ifdef DEBUG
        printf(" False ");
 #endif
    }else{
        ERRO
    }
    return BOOLEANO;
}

int isExprList(){
    return isExpr();
}

void exprlist(){
 #ifdef DEBUG_ALL
    printf("%sSTART EXPRLIST\n", getTabs());
    tab++;
 #endif
    if (isExpr()) {
        expr();
        nextLine();
        while (isVirgula()) {
 #ifdef DEBUG
            printf(",");
 #endif
            nextLine();
            if (isExpr()) {
                expr();
                nextLine();
            } else {
                goBack();
                return;
            }
        }
        goBack();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND EXPRLIST\n", getTabs());
 #endif
}

int isTestlist() {
    return isTest();
}

Tipo testlist() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART TESTLIST\n", getTabs());
    tab++;
 #endif
    if (isTest()) {
        tipo = test();
        nextLine();
        if(isVirgula()){
            temp.numElementos = 0;
            tipo = TUPLA;
        }
        while (isVirgula()) {
 #ifdef DEBUG
            printf(",");
 #endif
            nextLine();
            if (isTest()) {
                Tipo tipo2 = test();
                printf("add Elemento <%s> To ", tipos[tipo2]);
                printLista(temp);
                temp = addElementToLista(temp, tipo2);
                nextLine();
            } else {
                goBack();
                break;
            }
        }
        goBack();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND TESTLIST, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isAtom() {
    return isAbreParenteses() || isAbreColchete() || isIdentificador() || isLiteral() || isNumerico() || isBoolean() || isFuncStatement();
}

Tipo atom() {
    Tipo tipo = VAZIO;
 #ifdef DEBUG_ALL
    printf("%sSTART ATOM\n", getTabs());
    tab++;
 #endif
    if (isAbreParenteses()) {
 #ifdef DEBUG
        printf("(");
 #endif
        nextLine();
        if (isTestlist()) {
            testlist();
            nextLine();
        }
        if (isFechaParenteses()) {
 #ifdef DEBUG
            printf(" ) ");
 #endif
        } else {
            ERRO
        }
        tipo = TUPLA;
    } else if (isAbreColchete()) {
 #ifdef DEBUG
        printf(" [ ");
 #endif
        nextLine();
        if (isTestlist()) {
            testlist();
            nextLine();
        }
        if (isFechaColchete()) {
 #ifdef DEBUG
            printf(" ] ");
 #endif
        } else {
            ERRO
        }
        tipo = LISTA;
    } else if (isIdentificador() || isLiteral() || isNumerico()) {
        if(isIdentificador()) {
 #ifdef DEBUG
            printf(" name%s ", value);
 #endif
            int id = strToInt(value)-1;
            tipo = identificadores[id];
            nextLine();
            if(isAbreColchete()){
 #ifdef DEBUG
                printf(" [ ");
 #endif
                if(tipo != LISTA && tipo != TUPLA){
                    printf("ERRO SEMANTICO, Identificador nao é uma lista|tupla\n");
                    exit(4);
                }
                nextLine();
                if(isTest()){
                    char *valueAux = value;
                    printf("%s\n", line);
                    Tipo tipo2 = test();
                    if(tipo2 != INTEIRO && tipo2 != BOOLEANO){
                        printf("ERRO SEMANTICO, tipo <%s> nao aceito para id[<%s>]\n", tipos[tipo2], tipos[INTEIRO]);
                        exit(4);
                    }
                    int pos = strToInt(valueAux);
                    if(pos>=listIds[id].numElementos || pos < 0){
                        printf("ERRO SEMANTICO, out of bounds %d (>= %d || < 0)\n", pos, listIds[id].numElementos);
                        exit(4);
                    }
                    tipo = listIds[id].elementos[pos];
                    nextLine();
                    if(isFechaColchete()){
 #ifdef DEBUG
                        printf(" ] ");
 #endif
                    }else{
                        ERRO
                    }
                }else{
                    ERRO
                }
            }else{
                goBack();
            }
        } else{
 #ifdef DEBUG
            printf(" %s ", value);
 #endif
            if(isLiteral()){
                tipo = LITERAL;
            }else if(isNumerico()){
                if(isInteiro()){
                    tipo = INTEIRO;
                }else if(isReal()){
                    tipo = REAL;
                }
            }
        }
    } else if(isBoolean()){
        tipo = boolean();
    } else if(isFuncStatement()){
        funcStatement();
        tipo = FUNCAO;
    }else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND ATOM, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isPower() {
    return isAtom();
}

Tipo power() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART POWER\n", getTabs());
    tab++;
 #endif
    if (isAtom()) {
        tipo = atom();
        nextLine();
        if (isPotencia()) {
            Tipo tipo2;
 #ifdef DEBUG
            printf("^");
 #endif
            nextLine();
            if (isFactor()) {
                tipo2 = factor();
                if(tipo == INTEIRO && tipo2 == INTEIRO){
                    tipo = INTEIRO;
                }else if(tipo == REAL && tipo2 == REAL){
                    tipo = REAL;
                }else{
                    if(tipo == NI || tipo2 == NI){
                        printf("ERRO SEMANTICO, Identificador nao inicializado\n");
                        exit(4);
                    }else{
                        printf("ERRO SEMANTICO, tipos nao suportados para '^': <%s> <%s>\n", tipos[tipo], tipos[tipo2]);
                        exit(3);
                    }
                }
            } else {
                ERRO
            }
        } else {
            goBack();
        }
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND POWER, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isFactor() {
    return isMais() || isMenos() || isComplemento() || isPower();
}


Tipo factor() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART FACTOR\n", getTabs());
    tab++;
 #endif
    if (isMais() || isMenos() || isComplemento()) {
 #ifdef DEBUG
        char aux[3];
        if(isMais()){
            aux[0] = '+';
            aux[1] = '\0';
        }else if(isMenos()){
            aux[0] = '-';
            aux[1] = '\0';
        }else{
            aux[0] = '*';
            aux[1] = '*';
            aux[2] = '\0';
        }
        printf("%s", aux);
 #endif
        int complemento = isComplemento();
        nextLine();
        if (isFactor()) {
            tipo = factor();
            if(complemento && tipo != INTEIRO){
                if(tipo == NI){
                    printf("ERRO SEMANTICO, Identificador nao inicializado\n");
                    exit(4);
                }else{
                    printf("ERRO SEMANTICO, tipo(s) nao suportados para '~': <%s>\n", tipos[tipo]);
                    exit(3);
                }
            }
        } else {
            ERRO
        }
    } else if (isPower()) {
        tipo = power();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND FACTOR, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isTerm() {
    return isFactor();
}

Tipo term() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART TERM\n", getTabs());
    tab++;
 #endif
    if (isFactor()) {
        tipo = factor();
        nextLine();
        while (isVezes() || isDivisao() || isResto()) {
 #ifdef DEBUG
            char aux[2];
            if(isVezes()){
                aux[0] = '*';
                aux[1] = '\0';
            }else if(isDivisao()){
                aux[0] = '/';
                aux[1] = '\0';
            }else{
                aux[0] = '%';
                aux[1] = '\0';
            }
            printf("%s", aux);
 #endif
            int op = -1;
            if(isVezes()){
                op = 0;
            }else if(isDivisao()){
                op =1;
            }else if(isResto()){
                op = 2;
            }
            Tipo tipo2;
            nextLine();
            if (isFactor()) {
                tipo2 = factor();
                if((tipo == INTEIRO || tipo == REAL)&&(tipo2==INTEIRO || tipo2 == REAL)){
                    if(tipo == REAL || tipo2 == REAL || op == 1){
                        tipo = REAL;
                    }else{
                        tipo = INTEIRO;
                    }
                }else{
                    if(tipo == NI || tipo2 == NI){
                        printf("ERRO SEMANTICO, Identificador nao inicializado\n");
                        exit(4);
                    }else{
                        printf("ERRO SEMANTICO, tipo(s) nao suportados para '*|/|%%': <%s> <%s>\n", tipos[tipo], tipos[tipo2]);
                        exit(3);
                    }
                }
                nextLine();
            } else {
                ERRO
            }
        }
        goBack();
    } else {
        ERRO
    }
    multiplicacaoDivisao();
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND TERM, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isExpr() {
    return isTerm();
}

Tipo expr() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART EXPR\n", getTabs());
    tab++;
 #endif
    if (isTerm()) {
        tipo = term();
        nextLine();
        while (isMais() || isMenos()) {
 #ifdef DEBUG
            char aux[2];
            if(isMais()){
                aux[0] = '+';
                aux[1] = '\0';
            }else {
                aux[0] = '-';
                aux[1] = '\0';
            }
            printf("%s", aux);
 #endif
            Tipo tipo2;
            nextLine();
            if (isTerm()) {
                tipo2 = term();
                if((tipo == INTEIRO || tipo == REAL)&&(tipo2==INTEIRO || tipo2 == REAL)){
                    if(tipo == REAL || tipo2 == REAL){
                        tipo = REAL;
                    }else{
                        tipo = INTEIRO;
                    }
                }else{
                    if(tipo == NI || tipo2 == NI){
                        printf("ERRO SEMANTICO, Identificador nao inicializado\n");
                        exit(4);
                    }else{
                        printf("ERRO SEMANTICO, tipo(s) nao suportados para '+|-': <%s> <%s>\n", tipos[tipo], tipos[tipo2]);
                        exit(3);
                    }
                }
                nextLine();
            } else {
                ERRO
            }
        }
        goBack();
    } else {
        ERRO
    }
    somaSub();
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND EXPR, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isCompOp() {
    if (isNot()) {
        nextLine();
        if (isIn()) {
 #ifdef DEBUG
            printf(" not in ");
 #endif
            return 1;
        }
        goBack();
        return 0;
    }
    if (isIs()) {
        nextLine();
        if (isNot()) {
 #ifdef DEBUG
            printf(" is not ");
 #endif
            return 1;
        }
        goBack();
 #ifdef DEBUG
        printf(" is ");
 #endif
        return 1;
    }
 #ifdef DEBUG
    char aux[3];
    if(isMenor()){
        aux[0] = '<';
        aux[1] = '\0';
    }else if(isMaior()){
        aux[0] = '>';
        aux[1] = '\0';
    }else if(isComparaIgual()){
        aux[0] = '=';
        aux[1] = '=';
        aux[2] = '\0';
    }else if(isMaiorIgual()){
        aux[0] = '>';
        aux[1] = '=';
        aux[2] = '\0';
    }else if(isMenorIgual()){
        aux[0] = '<';
        aux[1] = '=';
        aux[2] = '\0';
    }else if(isDiferente()){
        aux[0] = '!';
        aux[1] = '=';
        aux[2] = '\0';
    }else if(isIn()){
        aux[0] = 'i';
        aux[1] = 'n';
        aux[2] = '\0';
    }else{
        aux[0] = '\0';
    }
    printf("%s", aux);
 #endif
    return isMenor() || isMaior() || isComparaIgual() || isMaiorIgual() || isMenorIgual() || isDiferente() || isIn();
}

int isComparison() {
    return isExpr();
}

Tipo comparison() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART COMPARISON\n", getTabs());
    tab++;
 #endif
    if (isExpr()) {
        tipo = expr();
        nextLine();
        while (isCompOp()) {
            nextLine();
            Tipo tipo2;
            if (isExpr()) {
                tipo2 = expr();
                if((tipo == tipo2)|| ((tipo == INTEIRO && tipo2 == REAL) || (tipo == REAL && tipo2 == INTEIRO))){
                    tipo = BOOLEANO;
                }else{
                    if(tipo == NI || tipo2 == NI){
                        printf("ERRO SEMANTICO, Identificador nao inicializado\n");
                        exit(4);
                    }else{
                        printf("ERRO SEMANTICO, tipo(s) nao suportados para 'comparacao': <%s> <%s>\n", tipos[tipo], tipos[tipo2]);
                        exit(3);
                    }
                }
                nextLine();
            } else {
                ERRO
            }
        }
        goBack();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND COMPARISON, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isNotTest() {
    return isNot() || isComparison();
}

Tipo notTest() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART NOT_TEST\n", getTabs());
    tab++;
 #endif
    if (isNot()) {
 #ifdef DEBUG
        printf(" not ");
 #endif
        nextLine();
        if (isNotTest()) {
            notTest();
            tipo = BOOLEANO;
        } else {
            ERRO
        }
    } else if (isComparison()) {
        tipo = comparison();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND NOT_TEST, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isAndTest() {
    return isNotTest();
}

Tipo andTest() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART AND_TEST\n", getTabs());
    tab++;
 #endif
    if (isNotTest()) {
        tipo = notTest();
        nextLine();
        while (isAnd()) {
 #ifdef DEBUG
            printf(" and ");
 #endif
            nextLine();
            if (isNotTest()) {
                Tipo tipo2 = notTest();
                if(tipo != BOOLEANO || tipo2 != BOOLEANO){
                    if(tipo == NI || tipo2 == NI){
                        printf("ERRO SEMANTICO, Identificador nao inicializado\n");
                        exit(4);
                    }else{
                        printf("ERRO SEMANTICO, tipo(s) nao suportados para 'and': <%s> <%s>\n", tipos[tipo], tipos[tipo2]);
                        exit(3);
                    }
                }
                nextLine();
            } else {
                ERRO
            }
        }
        goBack();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND AND_TEST, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isOrTest() {
    return isAndTest();
}

Tipo orTest() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART OR_TEST\n", getTabs());
    tab++;
 #endif
    if (isAndTest()) {
        tipo = andTest();
        nextLine();
        while (isOr()) {
 #ifdef DEBUG
            printf(" or ");
 #endif
            nextLine();
            if (isAndTest()) {
                Tipo tipo2 = andTest();
                if(tipo != BOOLEANO || tipo2 != BOOLEANO){
                    if(tipo == NI || tipo2 == NI){
                        printf("ERRO SEMANTICO, Identificador nao inicializado\n");
                        exit(4);
                    }else{
                        printf("ERRO SEMANTICO, tipo(s) nao suportados para 'or': <%s> <%s>\n", tipos[tipo], tipos[tipo2]);
                        exit(3);
                    }
                }
                nextLine();
            } else {
                ERRO
            }
        }
        goBack();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND OR_TEST, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isTest() {
    return isOrTest();
}

Tipo test() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART TEST\n", getTabs());
    tab++;
 #endif
    if (isOrTest()) {
        tipo = orTest();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND TEST, tipo:%s\n", getTabs(), tipos[tipo]);
 #endif
    return tipo;
}

int isParameters() {
    return isAbreParenteses();
}

void parameters() {
 #ifdef DEBUG_ALL
    printf("%sSTART PARAMETERS\n", getTabs());
    tab++;
 #endif
    if(isAbreParenteses()) {
 #ifdef DEBUG
        printf(" ( ");
 #endif
        nextLine();
        if (isIdentificador()) {
 #ifdef DEBUG
            printf(" name%s ", value);
 #endif
            nextLine();
            while(isVirgula()) {
 #ifdef DEBUG
                printf(" , ");
 #endif
                nextLine();
                if(isIdentificador()) {
 #ifdef DEBUG
                    printf(" name%s ", value);
 #endif
                    nextLine();
                } else {
                    break;
                }
            }
        }
        if (isFechaParenteses()) {
 #ifdef DEBUG
            printf(" ) ");
 #endif
        } else {
            ERRO
        }
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND PARAMETERS\n", getTabs());
 #endif
}

int isFuncDef() {
    return isDef();
}

void funcDef() {
 #ifdef DEBUG_ALL
    printf("%sSTART FUNC_DEF\n", getTabs());
    tab++;
 #endif
    if(isDef()) {
 #ifdef DEBUG
        printf(" def ");
 #endif
        nextLine();
        if (isIdentificador()) {
 #ifdef DEBUG
            printf(" name%s ", value);
 #endif
            nextLine();
            if(isParameters()) {
                parameters();
                nextLine();
                if(isDoisPontos()) {
 #ifdef DEBUG
                    printf(" :\n");
 #endif
                    nextLine();
                    if(isStatement()) {
                        statement();
 #ifdef DEBUG
                    printf(" \n");
 #endif
                        nextLine();
                        while(isStatement()) {
                            statement();
 #ifdef DEBUG
                    printf(" \n");
 #endif
                            nextLine();
                        }
                        goBack();
                    } else {
                        ERRO
                    }
                } else {
                    ERRO
                }
            } else {
                ERRO
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND FUNC_DEF\n", getTabs());
 #endif
}

int isWithItem() {
    return isTest();
}

void withItem() {
    if(isTest()) {
        test();
        nextLine();
        if(isAs()) {
 #ifdef DEBUG
            printf(" as ");
 #endif
            nextLine();
            if(isExpr()) {
                expr();
            } else {
                ERRO
            }
        } else {
            goBack();
        }
    } else {
        ERRO
    }
}

int isWithStatement() {
    return isWith();
}

void withStatement() {
    if(isWith()) {
 #ifdef DEBUG
        printf(" with ");
 #endif
        nextLine();
        if(isWithItem()) {
            withItem();
            nextLine();
            while(isVirgula()) {
 #ifdef DEBUG
                printf(" , ");
 #endif
                nextLine();
                if(isWithItem()) {
                    withItem();
                    nextLine();
                } else {
                    ERRO
                }
            }
            if(isDoisPontos()) {
 #ifdef DEBUG
                printf(" :\n");
 #endif
                nextLine();
                if(isStatement()) {
                    statement();
 #ifdef DEBUG
                    printf("\n");
 #endif
                    nextLine();
                    while(isStatement()) {
                        statement();
 #ifdef DEBUG
                        printf("\n");
 #endif
                        nextLine();
                    }
                    goBack();
                } else {
                    ERRO
                }
            } else {
                ERRO
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
}


int isForStatement() {
    return isFor();
}

void forStatement() {
    if(isFor()) {
 #ifdef DEBUG
        printf(" for ");
 #endif
        nextLine();
        if(isExprList()) {
            exprlist();
            nextLine();
            if(isIn()) {
 #ifdef DEBUG
                printf(" in ");
 #endif
                nextLine();
                if(isTestlist()) {
                    testlist();
                    nextLine();
                    if(isDoisPontos()) {
 #ifdef DEBUG
                        printf(" :\n");
 #endif
                        nextLine();
                        if(isStatement()) {
                            statement();
 #ifdef DEBUG
                            printf("\n");
 #endif
                            nextLine();
                            while(isStatement()) {
                                statement();
 #ifdef DEBUG
                                printf("\n");
 #endif
                                nextLine();
                            }
                            if(isElse()) {
 #ifdef DEBUG
                                printf(" else ");
 #endif
                                nextLine();
                                if(isDoisPontos()) {
 #ifdef DEBUG
                                    printf(" :\n");
 #endif
                                    nextLine();
                                    if(isStatement()) {
                                        statement();
 #ifdef DEBUG
                                        printf("\n");
 #endif
                                        nextLine();
                                        while(isStatement()) {
                                            statement();
 #ifdef DEBUG
                                            printf("\n");
 #endif
                                            nextLine();
                                        }
                                        goBack();
                                    } else {
                                        ERRO
                                    }
                                } else {
                                    ERRO
                                }
                            } else {
                                goBack();
                            }
                        } else {
                            ERRO
                        }
                    }else{
                        ERRO
                    }
                } else {
                    ERRO
                }
            }else{
                ERRO
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
}

int isWhileStatement() {
    return isWhile();
}

void whileStatement() {
    if(isWhile()) {
 #ifdef DEBUG
        printf(" while ");
 #endif
        nextLine();
        if(isTest()) {
            Tipo tipo = test();
            if(tipo == VAZIO || tipo == NI){
                printf("ERRO SEMANTICO, Identificador invalido\n");
                exit(4);
            }
            nextLine();
            if(isDoisPontos()) {
 #ifdef DEBUG
                printf(" :\n");
 #endif
                nextLine();
                if(isStatement()) {
                    statement();
 #ifdef DEBUG
                    printf("\n");
 #endif
                    nextLine();
                    while(isStatement()) {
                        statement();
 #ifdef DEBUG
                        printf("\n");
 #endif
                        nextLine();
                    }
                    if(isElse()) {
 #ifdef DEBUG
                        printf(" else ");
 #endif
                        nextLine();
                        if(isDoisPontos()) {
 #ifdef DEBUG
                            printf(" :\n");
 #endif
                            nextLine();
                            if(isStatement()) {
                                statement();
 #ifdef DEBUG
                                printf("\n");
 #endif
                                nextLine();
                                while(isStatement()) {
                                    statement();
 #ifdef DEBUG
                                    printf("\n");
 #endif
                                    nextLine();
                                }
                                goBack();
                            } else {
                                ERRO
                            }
                        }
                    } else {
                        goBack();
                    }
                }
            } else {
                ERRO
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
}

int isIfStatement() {
    return isIf();
}

void ifStatement() {
    if(isIf()) {
 #ifdef DEBUG
        printf(" if ");
 #endif
        nextLine();
        if(isTest()) {
            Tipo tipo = test();
            if(tipo == VAZIO || tipo == NI){
                printf("ERRO SEMANTICO, Identificador invalido\n");
                exit(4);
            }
            nextLine();
            if(isDoisPontos()) {
 #ifdef DEBUG
                printf(" :\n");
 #endif
                nextLine();
                if(isStatement()) {
                    statement();
 #ifdef DEBUG
                    printf("\n");
 #endif
                    nextLine();
                    while(isStatement()) {
                        statement();
 #ifdef DEBUG
                        printf("\n");
 #endif
                        nextLine();
                    }
                    while(isElif()) {
 #ifdef DEBUG
                        printf(" elif ");
 #endif
                        nextLine();
                        if(isTest()) {
                            tipo = test();
                            if(tipo == VAZIO || tipo == NI){
                                printf("ERRO SEMANTICO, Identificador invalido\n");
                                exit(4);
                            }
                            nextLine();
                            if(isDoisPontos()) {
 #ifdef DEBUG
                                printf(" :\n");
 #endif
                                if(isStatement()) {
                                    statement();
 #ifdef DEBUG
                                    printf("\n");
 #endif
                                    nextLine();
                                    while(isStatement()) {
                                        statement();
 #ifdef DEBUG
                                        printf("\n");
 #endif
                                        nextLine();
                                    }
                                    goBack();
                                } else {
                                    ERRO
                                }
                            } else {
                                ERRO
                            }
                        } else {
                            ERRO
                        }
                    }
                    if(isElse()) {
 #ifdef DEBUG
                        printf(" else ");
 #endif
                        nextLine();
                        if(isDoisPontos()) {
 #ifdef DEBUG
                            printf(" :\n");
 #endif
                            nextLine();
                            if(isStatement()) {
                                statement();
 #ifdef DEBUG
                                printf("\n");
 #endif
                                nextLine();
                                while(isStatement()) {
                                    statement();
 #ifdef DEBUG
                                    printf("\n");
 #endif
                                    nextLine();
                                }
                                goBack();
                            } else {
                                ERRO
                            }
                        }
                    } else {
                        goBack();
                    }
                } else {
                    ERRO
                }
            }else{
                ERRO
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
}

int isCompoundStatement() {
    return isIfStatement() || isWhileStatement() || isForStatement() || isWithStatement() || isFuncDef();
}

void compoundStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART COMPOUND_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isIfStatement()) {
        ifStatement();
    } else if(isWhileStatement()) {
        whileStatement();
    } else if(isForStatement()) {
        forStatement();
    } else if(isWithStatement()) {
        withStatement();
    } else if (isFuncDef()) {
        funcDef();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    printf("%sSTART COMPOUND_STATEMENT\n", getTabs());
    tab++;
 #endif
}

int isLenStatement(){
    return isLen();
}

void lenStatement(){
    if(isLen()){
 #ifdef DEBUG
        printf(" len ");
 #endif
        nextLine();
        if(isAbreParenteses()){
 #ifdef DEBUG
            printf(" ( ");
 #endif
            nextLine();
            if(isTest()){
                Tipo tipo = test();
                if(tipo != LISTA && tipo != TUPLA && tipo != LITERAL){
                    printf("ERRO SEMANTICO, Identificador invalido\n");
                    exit(4);
                }
                nextLine();
                if(isFechaParenteses()){
 #ifdef DEBUG
                    printf(" ) ");
 #endif
                }else{
                    ERRO
                }
            }else{
                ERRO
            }
        }else{
            ERRO
        }
    }else{
        ERRO
    }
}

int isInputStatement() {
    return isInput();
}

void inputStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART INPUT_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isInput()) {
 #ifdef DEBUG
        printf(" input ");
 #endif
        nextLine();
        if(isAbreParenteses()) {
 #ifdef DEBUG
            printf(" ( ");
 #endif
            nextLine();
            if(isTest()) {
                Tipo tipo = test();
                if(tipo == VAZIO || tipo == NI){
                    printf("ERRO SEMANTICO, Identificador invalido\n");
                    exit(4);
                }
                nextLine();
            }
            if(isFechaParenteses()) {
 #ifdef DEBUG
                printf(" ) ");
 #endif
            } else {
                ERRO
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND INPUT_STATEMENT\n", getTabs());
 #endif
}

int isRangeStatement() {
    return isRange();
}

void rangeStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART RANGESTATEMENT\n", getTabs());
    tab++;
 #endif
    if(isRange()) {
 #ifdef DEBUG
        printf(" range ");
 #endif
        nextLine();
        if(isAbreParenteses()) {
 #ifdef DEBUG
            printf(" ( ");
 #endif
            nextLine();
            if(isTest()) {
                Tipo tipo = test();
                if(tipo == VAZIO || tipo == NI){
                    printf("ERRO SEMANTICO, Identificador invalido\n");
                    exit(4);
                }
                nextLine();
                if(isVirgula()) {
 #ifdef DEBUG
                    printf(" , ");
 #endif
                    nextLine();
                    if(isTest()) {
                        tipo = test();
                        if(tipo == VAZIO || tipo == NI){
                            printf("ERRO SEMANTICO, Identificador invalido\n");
                            exit(4);
                        }
                        nextLine();
                    } else {
                        ERRO
                    }
                }
                if(isFechaParenteses()) {
 #ifdef DEBUG
                    printf(" ) ");
 #endif
                } else {
                    ERRO
                }
            } else {
                ERRO
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND RANGE_STATEMENT\n", getTabs());
 #endif
}

int isExecStatement() {
    return isExec();
}

void execStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART EXEC_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isExec()) {
 #ifdef DEBUG
        printf(" exec ");
 #endif
        nextLine();
        if(isExpr()) {
            Tipo tipo = expr();
            if(tipo == VAZIO || tipo == NI){
                printf("ERRO SEMANTICO, Identificador invalido\n");
                exit(4);
            }
            nextLine();
            if(isIn()){
 #ifdef DEBUG
                printf(" in ");
 #endif
                nextLine();
                if(isTest()){
                    test();
                    nextLine();
                    if(isVirgula()){
 #ifdef DEBUG
                        printf(" , ");
 #endif
                        nextLine();
                        if(isTest()){
                            test();
                        }else{
                            ERRO
                        }
                    }else{
                        goBack();
                    }
                }else{
                    ERRO
                }
            }else{
                goBack();
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND EXEC_STATEMENT\n", getTabs());
 #endif
}

int isPrintStatement() {
    return isPrint();
}

void printStatement() {
    Tipo tipo;
 #ifdef DEBUG_ALL
    printf("%sSTART PRINT_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isPrint()) {
 #ifdef DEBUG
        printf(" print ");
 #endif
        nextLine();
        if(isAbreParenteses()) {
 #ifdef DEBUG
            printf(" ( ");
 #endif
            nextLine();
            if(isTestlist()) {
                tipo = testlist();
                if(tipo == VAZIO || tipo == NI){
                    printf("ERRO SEMANTICO, Identificador invalido\n");
                    exit(4);
                }
                nextLine();
            }
            if(isFechaParenteses()) {
 #ifdef DEBUG
                printf(" ) ");
 #endif
            } else {
                ERRO
            }
        } else if(isTestlist()) {
            tipo = testlist();
            if(tipo == VAZIO || tipo == NI){
                printf("ERRO SEMANTICO, Identificador invalido\n");
                exit(4);
            }
        } else {
            ERRO
        }
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND PRINT_STATEMENT\n", getTabs());
 #endif
}

int isFuncStatement(){
    return isPrintStatement() || isExecStatement() || isRangeStatement() || isInputStatement() || isLenStatement();
}

void funcStatement(){
 #ifdef DEBUG_ALL
    printf("%sSTART FUNC_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isPrintStatement()) {
        printStatement();
    } else if(isExecStatement()) {
        execStatement();
    } else if(isRangeStatement()) {
        rangeStatement();
    } else if(isInputStatement()) {
        inputStatement();
    }else if(isLenStatement()){
        lenStatement();
    }else{
        ERRO
    }
 #ifdef DEBUG_ALL
    printf("%sEND FUNC_STATEMENT\n", getTabs());
    tab++;
 #endif
}

int isReturnStatement() {
    return isReturn();
}

void returnStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART RETURN_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isReturn()) {
 #ifdef DEBUG
        printf(" return ");
 #endif
        nextLine();
        if(isTestlist()) {
            Tipo tipo = testlist();
            if(tipo == VAZIO || tipo == NI){
                printf("ERRO SEMANTICO, Identificador invalido\n");
                exit(4);
            }
 #ifdef DEBUG_ALL
            printf("RETURN_STATEMENT\n");
 #endif
        }
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND RETURN_STATEMENT\n", getTabs());
 #endif
}

int isContinueStatement() {
    return isContinue();
}

void continueStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART CONTINUE_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isContinue()) {
 #ifdef DEBUG
        printf(" continue ");
 #endif
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND CONTINUE_STATEMENT\n", getTabs());
 #endif
}

int isBreakStatement() {
    return isBreak();
}

void breakStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART BREAK_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isBreak()) {
 #ifdef DEBUG
        printf(" break ");
 #endif
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND BREAK_STATEMENT\n", getTabs());
 #endif
}

int isRaiseStatement(){
    return isRaise();
}

void raiseStatement(){
    if(isRaise()){
 #ifdef DEBUG
        printf(" raise ");
 #endif
        nextLine();
        if(isTest()){
            test();
            nextLine();
            if(isVirgula()){
 #ifdef DEBUG
                printf(" , ");
 #endif
                nextLine();
                if(isTest()){
                    test();
                    nextLine();
                    if(isVirgula()){
 #ifdef DEBUG
                        printf(" , ");
 #endif
                        nextLine();
                        if(isTest()){
                            test();
                        }else{
                            ERRO
                        }
                    }else{
                        goBack();
                    }
                }else{
                    ERRO
                }
            }else{
                goBack();
            }
        }else{
            goBack();
        }
    }else{
        ERRO
    }
}

int isFlowStatement() {
    return isBreakStatement() || isContinueStatement() || isReturnStatement() || isRaiseStatement();
}

void flowStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART FLOW_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isBreakStatement()) {
        breakStatement();
    } else if(isContinueStatement()) {
        continueStatement();
    } else if(isReturnStatement()) {
        returnStatement();
    }else if(isRaiseStatement()){
        raiseStatement();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND FLOW_STATEMENT\n", getTabs());
 #endif
}

int isExprStatement() {
    return isTestlist();
}

void exprStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART EXPR_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isTestlist()) {
        int id = -1;
        if(isIdentificador()){
            id = strToInt(value)-1;
        }
        testlist();
        nextLine();
        while(isIgual()) {
 #ifdef DEBUG
            printf(" = ");
 #endif
            if(id == -1){
                printf("ERRO SEMANTICO, atribuicao a literal\n");
                exit(3);
            }
            nextLine();
            if(isTestlist()) {
                identificadores[id] = testlist();
                if(identificadores[id] == LISTA || identificadores[id] == TUPLA){
                    listIds[id] = temp;
                }
                nextLine();
            } else {
                ERRO
            }
        }
        goBack();
    }
    atribuicao();
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND EXPR_STATEMENT\n", getTabs());
 #endif
}

int isSmallStatement() {
    return isExprStatement() || isFlowStatement() || isFuncStatement();
}

void smallStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART SMALL_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isExprStatement()) {
        exprStatement();
    } else if(isFlowStatement()) {
        flowStatement();
    }else if(isFuncStatement()){
        funcStatement();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND SMALL_STATEMENT\n", getTabs());
 #endif
}

int isSimpleStatement() {
    return isSmallStatement();
}

void simpleStatement() {
 #ifdef DEBUG_ALL
    printf("%sSTART SIMPLE_STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isSmallStatement()) {
        smallStatement();
        nextLine();
        while (isPontoVirgula()) {
 #ifdef DEBUG
            printf(" ; ");
 #endif
            nextLine();
            if (isSmallStatement()) {
                smallStatement();
                nextLine();
            } else {
                goBack();
                return;
            }
        }
        goBack();
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND SIMPLE_STATEMENT\n", getTabs());
 #endif
}

int isStatement() {
    return isSimpleStatement() || isCompoundStatement();
}

void statement() {
 #ifdef DEBUG_ALL
    printf("%sSTART STATEMENT\n", getTabs());
    tab++;
 #endif
    if(isSimpleStatement()) {
        simpleStatement();
    } else if(isCompoundStatement()) {
        compoundStatement();
    } else {
        ERRO
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND STATEMENT\n", getTabs());
 #endif
}

void fileInput() {
 #ifdef DEBUG_ALL
    printf("%sSTART FILE_INPUT\n", getTabs());
    tab++;
 #endif
    nextLine();
    do {
        statement();
 #ifdef DEBUG
        printf("\n");
 #endif
        nextLine();
    } while(isStatement());
    if(!endOfFile()){
        printf("\nERRO SINTATICO: NOT A STATEMENT, %d - %s", lineNumber, line);fprintf(out, "ERRO SINTATICO: NOT A STATEMENT, %d - %s\n", lineNumber, line);exit(2);
    }
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND FILE_INPUT\n", getTabs());
 #endif
}

int getNumIds(){
    int maxId = 0;
    do{
        nextLine();
        if(isIdentificador()){
            int id = strToInt(value);
            if(id>maxId){
                maxId = id;
            }
        }
    }while(!endOfFile());
    return maxId;
}

void iniciaIds(){
    int ids = getNumIds();
    fseek(in, 0, SEEK_SET);
 #ifdef DEBUG
    printf("ids %d\n", ids);
 #endif
    identificadores = malloc(ids*sizeof(Tipo));
    listIds = malloc(ids*sizeof(Lista));
    int i;
    for (i = 0; i < ids; ++i){
        identificadores[i] = NI;
        listIds[i].numElementos = 0;
    }
}

int main() {
 #ifdef DEBUG_ALL
    printf("%sSTART LEXICO\n", getTabs());
    tab++;
 #endif
    lexico(FILE_LEX_IN, FILE_LEX_OUT);
 #ifdef DEBUG_ALL
    tab--;
    printf("%sEND LEXICO\n", getTabs());
 #endif
    in = fopen(FILE_LEX_OUT, "r");
    out = fopen(FILE_SYNTACTIC_OUT, "w");
    iniciaIds();
    fileInput();
    fprintf(out, "%s\n", "Sucesso");
    printf("\n%s\n", "Sucesso");
    return 0;
}
