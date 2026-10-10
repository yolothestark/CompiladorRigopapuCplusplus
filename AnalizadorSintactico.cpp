// Anguiano Garcia Angel Yahir Guadalupe, Figueroa Robles Axel Israel, Molina Alvarado Alvaro Moises, Torres Martinez Miguel Angel
#include "AnalizadorSintactico.h"
#include "TablaSimbolos.hpp" // NUEVO: Incluir la tabla de simbolos

using namespace std;

TablaSimbolos tabla; // NUEVO: Instancia global de la tabla de simbolos

// NUEVO: Funcion auxiliar para convertir el string del token al Enum
TipoDato obtenerTipoDato(const string& tipoStr) {
    if (tipoStr == "int") return TipoDato::INT;
    if (tipoStr == "float") return TipoDato::FLOAT;
    if (tipoStr == "void") return TipoDato::VOID;
    return TipoDato::DESCONOCIDO;
}

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
            comprobar("PALABRA_CLAVE", "void") ||
            comprobar("PALABRA_CLAVE", "boolean") ||
            comprobar("PALABRA_CLAVE", "class") ||
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
        if (comprobar("PALABRA_CLAVE", "class")) {
            declaracionClase();
        } else {
            sentencia();
        }
    }
}

bool AnalizadorSintactico::esTipoDato() const {
    return comprobar("PALABRA_CLAVE", "int") || 
           comprobar("PALABRA_CLAVE", "float") || 
           comprobar("PALABRA_CLAVE", "void") || 
           comprobar("PALABRA_CLAVE", "char") || 
           comprobar("PALABRA_CLAVE", "boolean");
}

void AnalizadorSintactico::declaracionClase() {
    coincidir("PALABRA_CLAVE", "class");
    
    if (!coincidir("IDENTIFICADOR")) {
        reportarError("Se esperaba un identificador para el nombre de la clase");
    }
    
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Cuerpo de clase invalido en la declaracion de 'class' (Falta '{')");
    }
    
    cuerpoClase();
    
    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Cuerpo de clase invalido en la declaracion de 'class' (Falta '}')");
    }
}

void AnalizadorSintactico::cuerpoClase() {
    while (!estaAlFinal() && !comprobar("SIMBOLO", "}")) {
        if (esTipoDato()) {
            declaracionMetodoOVariable();
        } else {
            reportarError("Se esperaba una declaracion de metodo o variable dentro de la clase");
            sincronizar(); 
        }
    }
}

void AnalizadorSintactico::declaracionMetodoOVariable() {
    Token tokenTipo = tokenActual(); // NUEVO: Guardamos el tipo de dato
    avanzar(); // Consumir el tipo
    
    Token tokenVar = tokenActual(); // NUEVO: Guardamos el nombre
    if (!coincidir("IDENTIFICADOR")) {
        reportarError("Se esperaba un identificador despues del tipo");
        return;
    }

    // NUEVO: Insercion en Tabla de Simbolos (Variables globales o metodos)
    tabla.insertarSimbolo(tokenVar.valor, obtenerTipoDato(tokenTipo.valor), tokenVar.linea, tokenVar.columna);
    
    if (comprobar("SIMBOLO", "(")) {
        // Es un metodo
        avanzar(); // Consumir '('
        parametros();
        
        if (!coincidir("SIMBOLO", ")")) {
            reportarError("Faltan parentesis en la declaracion del metodo");
        }
        
        if (!comprobar("SIMBOLO", "{")) {
            reportarError("Cuerpo de metodo invalido en la declaracion de metodo (Falta '{')");
            sincronizar();
            return;
        }
        bloque(); // cuerpo_metodo
    } else {
        // Es una variable
        if (coincidir("OPERADOR_ASIGNACION", "=")) {
            expresion();
        }
        if (!coincidir("SIMBOLO", ";")) {
            reportarError("Se esperaba ';' al final de la declaracion de variable");
        }
    }
}

void AnalizadorSintactico::parametros() {
    if (!comprobar("SIMBOLO", ")")) { 
        parametro();
        while (coincidir("SIMBOLO", ",")) {
            parametro();
        }
    }
}

void AnalizadorSintactico::parametro() {
    if (esTipoDato()) {
        Token tokenTipo = tokenActual(); // NUEVO
        avanzar(); // Consumir tipo
        
        Token tokenVar = tokenActual(); // NUEVO
        if (!coincidir("IDENTIFICADOR")) {
            reportarError("Parametro invalido (Falta identificador)");
        } else {
