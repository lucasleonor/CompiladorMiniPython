#include <string.h>

extern char *type;
extern char *value;

int isReservada(){
    return !strcmp("reservada", type);
}

int isDelimitador(){
    return !strcmp("delimitador", type);
}

int isOperador(){
    return !strcmp("operador", type);
}

int isIdentificador(){
    return !strcmp("identificador", type);
}

int isLiteral(){
    return !strcmp("cadeia_caracteres", type);
}

int isInteiro(){
    return !strcmp("inteiro", type);
}

int isReal(){
    return !strcmp("real", type);
}

int isNumerico(){
    return isInteiro() || isReal() ;
}

int isFor(){
    return isReservada() && !strcmp("for", value);
}

int isIn(){
    return isReservada() && !strcmp("in", value);
}

int isWhile(){
    return isReservada() && !strcmp("while", value);
}

int isIf(){
    return isReservada() && !strcmp("if", value);
}

int isElif(){
    return isReservada() && !strcmp("elif", value);
}

int isElse(){
    return isReservada() && !strcmp("else", value);
}

int isPrint(){
    return isReservada() && !strcmp("print", value);
}

int isBreak(){
    return isReservada() && !strcmp("break", value);
}

int isContinue(){
    return isReservada() && !strcmp("continue", value);
}

int isReturn(){
    return isReservada() && !strcmp("return", value);
}

int isExec(){
    return isReservada() && !strcmp("exec", value);
}

int isRange(){
    return isReservada() && !strcmp("range", value);
}

int isInput(){
    return isReservada() && !strcmp("input", value);
}

int isWith(){
    return isReservada() && !strcmp("with", value);
}

int isAs(){
    return isReservada() && !strcmp("as", value);
}

int isIs(){
    return isReservada() && !strcmp("is", value);
}

int isFrom(){
    return isReservada() && !strcmp("from", value);
}

int isLen(){
    return isReservada() && !strcmp("len", value);
}

int isRaise(){
    return isReservada() && !strcmp("raise", value);
}

int isDef(){
    return isReservada() && !strcmp("def", value);
}

int isTrue(){
    return isReservada() && !strcmp("True", value);
}

int isFalse(){
    return isReservada() && !strcmp("False", value);
}

int isOr(){
    return isReservada() && !strcmp("or", value);
}

int isNot(){
    return isReservada() && !strcmp("not", value);
}

int isAnd(){
    return isReservada() && !strcmp("and", value);
}

int isAbreParenteses(){
    return isDelimitador() && !strcmp("abre_parenteses", value);
}

int isFechaParenteses(){
    return isDelimitador() && !strcmp("fecha_parenteses", value);
}

int isAbreChave(){
    return isDelimitador() && !strcmp("abre_chave", value);
}

int isFechaChave(){
    return isDelimitador() && !strcmp("fecha_chave", value);
}

int isVirgula(){
    return isDelimitador() && !strcmp("virgula", value);
}

int isDoisPontos(){
    return isDelimitador() && !strcmp("dois_pontos", value);
}

int isPonto(){
    return isDelimitador() && !strcmp("ponto", value);
}

int isIgual(){
    return isDelimitador() && !strcmp("igual", value);
}

int isPontoVirgula(){
    return isDelimitador() && !strcmp("ponto_virgula", value);
}

int isAbreColchete(){
    return isDelimitador() && !strcmp("abre_colchete", value);
}

int isFechaColchete(){
    return isDelimitador() && !strcmp("fecha_colchete", value);
}

int isMenos(){
    return isOperador() && !strcmp("menos", value);
}

int isMais(){
    return isOperador() && !strcmp("mais", value);
}

int isDivisao(){
    return isOperador() && !strcmp("divisao", value);
}

int isPotencia(){
    return isOperador() && !strcmp("potencia", value);
}

int isMaior(){
    return isOperador() && !strcmp("maior", value);
}

int isMaiorIgual(){
    return isOperador() && !strcmp("maior_igual", value);
}

int isMenorIgual(){
    return isOperador() && !strcmp("menor_igual", value);
}

int isDiferente(){
    return isOperador() && !strcmp("diferente", value);
}

int isResto(){
    return isOperador() && !strcmp("resto", value);
}

int isVezes(){
    return isOperador() && !strcmp("vezes", value);
}

int isComplemento(){
    return isOperador() && !strcmp("complemento", value);
}

int isMenor(){
    return isOperador() && !strcmp("menor", value);
}

int isComparaIgual(){
    return isOperador() && !strcmp("compara_igual", value);
}
