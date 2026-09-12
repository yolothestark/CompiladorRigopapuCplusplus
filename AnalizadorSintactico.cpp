// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#include "AnalizadorSintactico.h"

using namespace std;

AnalizadorSintactico::AnalizadorSintactico() : posicionActual(0) {}

Token AnalizadorSintactico::tokenActual() const {
    if (posicionActual < tokens.size()) {
        return tokens[posicionActual];
    }
    int ultimaLinea = tokens.empty() ? 1 : tokens.back().linea;
    int ultimaCol = tokens.empty() ? 1 : tokens.back().columna;
    return {"EOF", "FIN_DE_ARCHIVO", ultimaLinea, ultimaCol};
}

Token AnalizadorSintactico::tokenAnterior() const {
    if (posicionActual > 0 && !tokens.empty()) {
        return tokens[posicionActual - 1];
    }
    return {"NULO", "", 1, 1};
}

bool AnalizadorSintactico::estaAlFinal() const {
    return posicionActual >= tokens.size();
}

bool AnalizadorSintactico::comprobar(const string& tipo) const {
    if (estaAlFinal()) return false;
    return tokenActual().tipo == tipo;
}

bool AnalizadorSintactico::comprobarValor(const string& valor) const {
    if (estaAlFinal()) return false;
    return tokenActual().valor == valor;
}

Token AnalizadorSintactico::avanzar() {
    if (!estaAlFinal()) posicionActual++;
    return tokenAnterior();
}

void AnalizadorSintactico::registrarError(const string& mensaje, const Token& tok) {
    errores.push_back({tok.linea, tok.columna, mensaje, tok.valor});
}

void AnalizadorSintactico::sincronizar() {
    avanzar();

    while (!estaAlFinal()) {
        if (tokenAnterior().valor == ";") return;
        avanzar();
    }
}

bool AnalizadorSintactico::analizar(const vector<Token>& tokensEntrada) {
    tokens = tokensEntrada;
    posicionActual = 0;
    errores.clear();

    programa();

    return errores.empty();
}

void AnalizadorSintactico::programa() {
    while (!estaAlFinal()) {
        expresionStmt();
    }
}

void AnalizadorSintactico::expresionStmt() {
    expresion();

    if (comprobarValor(";")) {
        avanzar();
    } else {
        Token tokErr = tokenAnterior();
        registrarError("Error Sintactico: Se esperaba ';' al final de la expresion", tokenActual());
        sincronizar();
    }
}

// expresion -> expresion_logica
void AnalizadorSintactico::expresion() {
    expresionLogica();
}

// expresion_logica -> expresion_relacional ( ('&&' | '||') expresion_relacional )*
void AnalizadorSintactico::expresionLogica() {
    expresionRelacional();

    while (comprobarValor("&&") || comprobarValor("||")) {
        Token op = avanzar();
        if (comprobarValor(";") || estaAlFinal()) {
            registrarError("Error Sintactico: Operador logico '" + op.valor + "' sin termino valido a la derecha", tokenActual());
            return;
        }
        expresionRelacional();
    }
}

// expresion_relacional -> expresion_aritmetica [ ('==' | '!=' | '>' | '<' | '>=' | '<=') expresion_aritmetica ]
void AnalizadorSintactico::expresionRelacional() {
    expresionAritmetica();

    if (comprobarValor("==") || comprobarValor("!=") || comprobarValor(">") || 
        comprobarValor("<") || comprobarValor(">=") || comprobarValor("<=")) {
        Token op = avanzar();
        
        if (comprobarValor(";") || estaAlFinal()) {
            registrarError("Error Sintactico: Comparacion invalida. Operador '" + op.valor + "' sin expresion a la derecha", tokenActual());
            return;
        }
        
        expresionAritmetica();
    }
}

// expresion_aritmetica -> termino_aritmetico ( ('+' | '-') termino_aritmetico )*
void AnalizadorSintactico::expresionAritmetica() {
    terminoAritmetico();

    while (comprobarValor("+") || comprobarValor("-")) {
        Token op = avanzar();
        
        if (comprobarValor(";") || estaAlFinal()) {
            registrarError("Error Sintactico: Operador aritmetico '" + op.valor + "' sin termino valido a la derecha", tokenActual());
            return;
        }

        if (comprobarValor("+") || comprobarValor("-") || comprobarValor("*") || comprobarValor("/")) {
            registrarError("Error Sintactico: Operador duplicado o sin termino intermedio '" + tokenActual().valor + "'", tokenActual());
            sincronizar();
            return;
        }

        terminoAritmetico();
    }
}

// termino_aritmetico -> factor_aritmetico ( ('*' | '/') factor_aritmetico )*
void AnalizadorSintactico::terminoAritmetico() {
    factorAritmetico();

    while (comprobarValor("*") || comprobarValor("/")) {
        Token op = avanzar();

        if (comprobarValor(";") || estaAlFinal()) {
            registrarError("Error Sintactico: Operador aritmetico '" + op.valor + "' sin factor valido a la derecha", tokenActual());
            return;
        }

        if (comprobarValor("+") || comprobarValor("-") || comprobarValor("*") || comprobarValor("/")) {
            registrarError("Error Sintactico: Operador duplicado o sin termino intermedio '" + tokenActual().valor + "'", tokenActual());
            sincronizar();
            return;
        }

        factorAritmetico();
    }
}

// factor_aritmetico -> IDENTIFICADOR | ENTERO | DECIMAL | '(' expresion ')'
void AnalizadorSintactico::factorAritmetico() {
    if (comprobar("IDENTIFICADOR") || comprobar("ENTERO") || comprobar("DECIMAL")) {
        avanzar();
    } else if (comprobarValor("(")) {
        Token parentesisApertura = avanzar();
        
        expresion();

        if (comprobarValor(")")) {
            avanzar();
        } else {
            registrarError("Error Sintactico: Parentesis desbalanceados. Se esperaba ')' para cerrar el parentesis de linea " + 
                           to_string(parentesisApertura.linea), tokenActual());
            sincronizar();
        }
    } else {
        registrarError("Error Sintactico: Se esperaba un operando valido (identificador, numero o expresion entre parentesis)", tokenActual());
        sincronizar();
    }
}

const vector<ErrorSintactico>& AnalizadorSintactico::obtenerErrores() const {
    return errores;
}

bool AnalizadorSintactico::tieneErrores() const {
    return !errores.empty();
}