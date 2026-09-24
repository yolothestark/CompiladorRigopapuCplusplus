// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#include "AnalizadorSintactico.h"

const vector<ErrorSintactico>& AnalizadorSintactico::obtenerErrores() const {
    return errores;
}

bool AnalizadorSintactico::tieneErrores() const {
    return !errores.empty();
}

Token AnalizadorSintactico::tokenActual() const {
    if (indiceActual < tokens.size()) {
        return tokens[indiceActual];
    }
    return {"EOF", "", -1, -1};
}

Token AnalizadorSintactico::tokenAnterior() const {
    if (indiceActual > 0) {
        return tokens[indiceActual - 1];
    }
    return {"EOF", "", -1, -1};
}

bool AnalizadorSintactico::estaAlFinal() const {
    return indiceActual >= tokens.size();
}

Token AnalizadorSintactico::avanzar() {
    if (!estaAlFinal()) {
        indiceActual++;
    }
    return tokenAnterior();
}

bool AnalizadorSintactico::comprobar(const string& tipo, const string& valor) const {
    if (estaAlFinal()) return false;
    if (tokenActual().tipo != tipo) return false;
    if (!valor.empty() && tokenActual().valor != valor) return false;
    return true;
}

bool AnalizadorSintactico::coincidir(const string& tipo, const string& valor) {
    if (comprobar(tipo, valor)) {
        avanzar();
        return true;
    }
    return false;
}

void AnalizadorSintactico::reportarError(const string& mensaje) {
    Token t = tokenActual();
    if (t.tipo == "EOF" && !tokens.empty()) {
        t = tokens.back();
    }
    errores.push_back({t.linea, t.columna, "Error Sintactico", mensaje});
}

void AnalizadorSintactico::sincronizar() {
    avanzar();
    while (!estaAlFinal()) {
        if (tokenAnterior().valor == ";") return;

        if (comprobar("PALABRA_CLAVE", "if") ||
            comprobar("PALABRA_CLAVE", "while") ||
            comprobar("PALABRA_CLAVE", "for") ||
            comprobar("PALABRA_CLAVE", "int") ||
            comprobar("PALABRA_CLAVE", "float") ||
            comprobar("PALABRA_CLAVE", "char") ||
            comprobar("PALABRA_CLAVE", "return")) {
            return;
        }
        avanzar();
    }
}

void AnalizadorSintactico::analizar(const vector<Token>& tokensEntrada) {
    tokens = tokensEntrada;
    indiceActual = 0;
    errores.clear();

    programa();
}

void AnalizadorSintactico::programa() {
    while (!estaAlFinal()) {
        sentencia();
    }
}

void AnalizadorSintactico::sentencia() {
    if (comprobar("PALABRA_CLAVE", "if") || 
        comprobar("PALABRA_CLAVE", "while") || 
        comprobar("PALABRA_CLAVE", "for")) {
        estructuraControl();
    } 
    else if (comprobar("SIMBOLO", "{")) {
        bloque();
    } 
    else {
        // Sentencia simple o expresión finalizada en ';'
        expresion();
        if (!coincidir("SIMBOLO", ";")) {
            reportarError("Se esperaba ';' al final de la sentencia.");
            sincronizar();
        }
    }
}

void AnalizadorSintactico::estructuraControl() {
    if (comprobar("PALABRA_CLAVE", "if")) {
        sentenciaIf();
    } else if (comprobar("PALABRA_CLAVE", "while")) {
        sentenciaWhile();
    } else if (comprobar("PALABRA_CLAVE", "for")) {
        sentenciaFor();
    }
}

// sentencia_if -> 'if' '(' expresion ')' bloque ( 'else' bloque )?
void AnalizadorSintactico::sentenciaIf() {
    coincidir("PALABRA_CLAVE", "if");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues del 'if'.");
    }

    expresion();

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condicion del 'if'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Bloque de sentencias faltante o invalido en la estructura 'if'.");
        sincronizar();
        return;
    }
    bloque();

    if (coincidir("PALABRA_CLAVE", "else")) {
        if (!comprobar("SIMBOLO", "{")) {
            reportarError("Bloque de sentencias faltante o invalido en la estructura 'else'.");
            sincronizar();
            return;
        }
        bloque();
    }
}

// sentencia_while -> 'while' '(' expresion ')' bloque
void AnalizadorSintactico::sentenciaWhile() {
    coincidir("PALABRA_CLAVE", "while");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues del 'while'.");
    }

    expresion();

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condicion del 'while'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Bloque de sentencias faltante en la estructura 'while'.");
        sincronizar();
        return;
    }
    bloque();
}

// sentencia_for -> 'for' '(' inicializacion ';' expresion ';' actualizacion ')' bloque
void AnalizadorSintactico::sentenciaFor() {
    coincidir("PALABRA_CLAVE", "for");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues del 'for'.");
    }

    // Inicialización
    if (!comprobar("SIMBOLO", ";")) {
        inicializacion();
    }
    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Componente invalido o falta ';' tras la inicializacion en la estructura 'for'.");
    }

    // Condición
    if (!comprobar("SIMBOLO", ";")) {
        expresion();
    }
    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Componente invalido o falta ';' tras la condicion en la estructura 'for'.");
    }

    // Actualización
    if (!comprobar("SIMBOLO", ")")) {
        actualizacion();
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la actualizacion del 'for'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Bloque de sentencias faltante en la estructura 'for'.");
        sincronizar();
        return;
    }
    bloque();
}

// bloque -> '{' ( sentencia )* '}'
void AnalizadorSintactico::bloque() {
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Se esperaba '{' al inicio del bloque.");
        return;
    }

    while (!comprobar("SIMBOLO", "}") && !estaAlFinal()) {
        sentencia();
    }

    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Se esperaba '}' al cerrar el bloque.");
    }
}

void AnalizadorSintactico::inicializacion() {
    expresion();
}

void AnalizadorSintactico::actualizacion() {
    expresion();
}

// Reconoce expresiones básicas (asignaciones, comparaciones, etc.)
void AnalizadorSintactico::expresion() {
    expresionSimple();

    if (comprobar("OPERADOR_RELACIONAL") || comprobar("OPERADOR_ASIGNACION") || comprobar("OPERADOR_LOGICO")) {
        avanzar();
        expresionSimple();
    }
}

void AnalizadorSintactico::expresionSimple() {
    if (comprobar("IDENTIFICADOR") || comprobar("ENTERO") || comprobar("DECIMAL") || comprobar("CADENA")) {
        avanzar();
        if (comprobar("OPERADOR_INCREMENTO") || comprobar("OPERADOR_ARITMETICO")) {
            avanzar();
            if (comprobar("IDENTIFICADOR") || comprobar("ENTERO") || comprobar("DECIMAL")) {
                avanzar();
            }
        }
    } else if (comprobar("PALABRA_CLAVE", "int") || comprobar("PALABRA_CLAVE", "float") || comprobar("PALABRA_CLAVE", "char")) {
        avanzar(); // Tipo de dato en declaración
        if (comprobar("IDENTIFICADOR")) {
            avanzar();
            if (coincidir("OPERADOR_ASIGNACION", "=")) {
                expresion();
            }
        }
    } else {
        reportarError("Expresion o componente sintactico invalido ('" + tokenActual().valor + "').");
    }
}