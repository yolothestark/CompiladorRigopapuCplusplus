// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#include "AnalizadorSintactico.h"
#include "TablaSimbolos.hpp" 

using namespace std;

TablaSimbolos tabla; 

TipoDato obtenerTipoDato(const string& tipoStr) {
    if (tipoStr == "int") return TipoDato::INT;
    if (tipoStr == "float") return TipoDato::FLOAT;
    if (tipoStr == "void") return TipoDato::VOID;
    if (tipoStr == "boolean" || tipoStr == "bool") return TipoDato::BOOLEAN;
    if (tipoStr == "string") return TipoDato::STRING;
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
        return tokens[indiceActual - 1]; // Corregido el salto de línea del error anterior
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
    errores.push_back({t.linea, t.columna, "Error", mensaje});
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
            comprobar("PALABRA_CLAVE", "string") ||
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
           comprobar("PALABRA_CLAVE", "string") ||
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
    Token tokenTipo = tokenActual();
    avanzar(); 
    
    Token tokenVar = tokenActual();
    if (!coincidir("IDENTIFICADOR")) {
        reportarError("Se esperaba un identificador despues del tipo");
        return;
    }

    tabla.insertarSimbolo(tokenVar.valor, obtenerTipoDato(tokenTipo.valor), tokenVar.linea, tokenVar.columna);
    
    if (comprobar("SIMBOLO", "(")) {
        avanzar(); 
        parametros();
        
        if (!coincidir("SIMBOLO", ")")) reportarError("Faltan parentesis en la declaracion del metodo");
        
        if (!comprobar("SIMBOLO", "{")) {
            reportarError("Cuerpo de metodo invalido en la declaracion de metodo (Falta '{')");
            sincronizar();
            return;
        }
        bloque();
    } else {
        if (coincidir("OPERADOR_ASIGNACION", "=")) {
            TipoDato tipoExpr = expresion(); // NUEVO: Capturar el tipo evaluado
            tabla.verificarAsignacion(tokenVar.valor, tipoExpr, tokenVar.linea, tokenVar.columna);
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
        Token tokenTipo = tokenActual();
        avanzar();
        
        Token tokenVar = tokenActual();
        if (!coincidir("IDENTIFICADOR")) {
            reportarError("Parametro invalido (Falta identificador)");
        } else {
            tabla.insertarSimbolo(tokenVar.valor, obtenerTipoDato(tokenTipo.valor), tokenVar.linea, tokenVar.columna);
        }
    } else {
        reportarError("Parametro invalido (Se esperaba un tipo de dato)");
        if (!estaAlFinal() && !comprobar("SIMBOLO", ")") && !comprobar("SIMBOLO", ",")) {
            avanzar();
        }
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

void AnalizadorSintactico::sentenciaIf() {
    coincidir("PALABRA_CLAVE", "if");
    if (!coincidir("SIMBOLO", "(")) reportarError("Se esperaba '(' despues del 'if'.");

    TipoDato tipoCond = expresion(); // NUEVO: Validacion Semántica (if requiere booleano)
    if (tipoCond != TipoDato::BOOLEAN && tipoCond != TipoDato::DESCONOCIDO) {
        reportarError("Error Semantico: La condicion del 'if' debe ser una expresion logica (booleana).");
    }

    if (!coincidir("SIMBOLO", ")")) reportarError("Se esperaba ')' tras la condicion del 'if'.");

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

void AnalizadorSintactico::sentenciaWhile() {
    coincidir("PALABRA_CLAVE", "while");
    if (!coincidir("SIMBOLO", "(")) reportarError("Se esperaba '(' despues del 'while'.");

    TipoDato tipoCond = expresion();
    if (tipoCond != TipoDato::BOOLEAN && tipoCond != TipoDato::DESCONOCIDO) {
        reportarError("Error Semantico: La condicion del 'while' debe ser logica (booleana).");
    }

    if (!coincidir("SIMBOLO", ")")) reportarError("Se esperaba ')' tras la condicion del 'while'.");

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Bloque de sentencias faltante en la estructura 'while'.");
        sincronizar();
        return;
    }
    bloque();
}

void AnalizadorSintactico::sentenciaFor() {
    coincidir("PALABRA_CLAVE", "for");
    if (!coincidir("SIMBOLO", "(")) reportarError("Se esperaba '(' despues del 'for'.");
    
    if (!comprobar("SIMBOLO", ";")) inicializacion();
    if (!coincidir("SIMBOLO", ";")) reportarError("Componente invalido o falta ';' tras inicializacion en 'for'.");
    
    if (!comprobar("SIMBOLO", ";")) expresion();
    if (!coincidir("SIMBOLO", ";")) reportarError("Componente invalido o falta ';' tras condicion en 'for'.");
    
    if (!comprobar("SIMBOLO", ")")) actualizacion();
    if (!coincidir("SIMBOLO", ")")) reportarError("Se esperaba ')' tras actualizacion del 'for'.");
    
    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Bloque de sentencias faltante en 'for'.");
        sincronizar();
        return;
    }
    bloque();
}

void AnalizadorSintactico::bloque() {
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Se esperaba '{' al inicio del bloque.");
        return;
    }

    tabla.entrarAmbito(); // Scope Local

    while (!comprobar("SIMBOLO", "}") && !estaAlFinal()) {
        sentencia();
    }

    if (!coincidir("SIMBOLO", "}")) reportarError("Se esperaba '}' al cerrar el bloque.");
    
    tabla.salirAmbito(); 
}

void AnalizadorSintactico::inicializacion() { expresion(); }
void AnalizadorSintactico::actualizacion() { expresion(); }

TipoDato AnalizadorSintactico::expresion() {
    Token tokenIzq = tokenActual();
    TipoDato tipoIzq = expresionSimple();

    while (comprobar("OPERADOR_RELACIONAL") || comprobar("OPERADOR_ASIGNACION") || comprobar("OPERADOR_LOGICO")) {
        Token tokenOp = tokenActual();
        avanzar();
        
        TipoDato tipoDer = expresionSimple();
        
        if (tokenOp.valor == "=") {
            if (tokenIzq.tipo == "IDENTIFICADOR") {
                tabla.verificarAsignacion(tokenIzq.valor, tipoDer, tokenIzq.linea, tokenIzq.columna);
            }
            tipoIzq = tipoDer;
        } 
        else if (tokenOp.tipo == "OPERADOR_RELACIONAL") {
            bool izqNum = (tipoIzq == TipoDato::INT || tipoIzq == TipoDato::FLOAT);
            bool derNum = (tipoDer == TipoDato::INT || tipoDer == TipoDato::FLOAT);
            
            if (!((izqNum && derNum) || (tipoIzq == tipoDer && tipoIzq != TipoDato::DESCONOCIDO))) {
                reportarError("Error Semantico: No se pueden comparar tipos incompatibles (" + tokenOp.valor + ").");
            }
            tipoIzq = TipoDato::BOOLEAN; 
        }
        else if (tokenOp.tipo == "OPERADOR_LOGICO") {
            if (tipoIzq != TipoDato::BOOLEAN || tipoDer != TipoDato::BOOLEAN) {
                reportarError("Error Semantico: Operador logico '" + tokenOp.valor + "' requiere operandos booleanos.");
            }
            tipoIzq = TipoDato::BOOLEAN;
        }
    }
    return tipoIzq;
}

TipoDato AnalizadorSintactico::expresionSimple() {
    TipoDato tipoBase = TipoDato::DESCONOCIDO;

    if (comprobar("IDENTIFICADOR") || comprobar("ENTERO") || comprobar("DECIMAL") || comprobar("CADENA") || comprobar("PALABRA_CLAVE", "true") || comprobar("PALABRA_CLAVE", "false")) {
        
        Token tokenActualInfo = tokenActual();
        avanzar();
        
        if (tokenActualInfo.tipo == "IDENTIFICADOR") {
            Simbolo* sim = tabla.buscarSimbolo(tokenActualInfo.valor, tokenActualInfo.linea, tokenActualInfo.columna);
            if (sim) tipoBase = sim->tipo;
        } else if (tokenActualInfo.tipo == "ENTERO") {
            tipoBase = TipoDato::INT;
        } else if (tokenActualInfo.tipo == "DECIMAL") {
            tipoBase = TipoDato::FLOAT;
        } else if (tokenActualInfo.tipo == "CADENA") {
            tipoBase = TipoDato::STRING;
        } else if (tokenActualInfo.valor == "true" || tokenActualInfo.valor == "false") {
            tipoBase = TipoDato::BOOLEAN;
        }

        while (comprobar("OPERADOR_ARITMETICO")) {
            Token tokenOp = tokenActual();
            avanzar();

            if (tokenOp.valor == "/" && (tokenActual().valor == "0" || tokenActual().valor == "0.0")) {
                reportarError("Error Semantico: Division por cero detectada.");
            }

            Token tokenDer = tokenActual();
            TipoDato tipoDer = TipoDato::DESCONOCIDO;
            
            if (comprobar("IDENTIFICADOR") || comprobar("ENTERO") || comprobar("DECIMAL")) {
                if (tokenDer.tipo == "IDENTIFICADOR") {
                    Simbolo* sim = tabla.buscarSimbolo(tokenDer.valor, tokenDer.linea, tokenDer.columna);
                    if (sim) tipoDer = sim->tipo;
                } else if (tokenDer.tipo == "ENTERO") {
                    tipoDer = TipoDato::INT;
                } else if (tokenDer.tipo == "DECIMAL") {
                    tipoDer = TipoDato::FLOAT;
                }
                avanzar();
            } else {
                reportarError("Error Sintactico: Se esperaba un numero o variable despues del operador aritmetico.");
            }

            bool izqNum = (tipoBase == TipoDato::INT || tipoBase == TipoDato::FLOAT);
            bool derNum = (tipoDer == TipoDato::INT || tipoDer == TipoDato::FLOAT);

            if (!izqNum || !derNum) {
                reportarError("Error Semantico: No se puede aplicar el operador aritmetico '" + tokenOp.valor + "' a tipos incompatibles.");
                tipoBase = TipoDato::DESCONOCIDO;
            } else {
                if (tipoBase == TipoDato::FLOAT || tipoDer == TipoDato::FLOAT) {
                    tipoBase = TipoDato::FLOAT;
                }
            }
        }

        if (comprobar("OPERADOR_INCREMENTO")) avanzar(); 

    } else if (esTipoDato()) {
        Token tokenTipo = tokenActual();
        avanzar(); 
        
        if (comprobar("IDENTIFICADOR")) {
            Token tokenVar = tokenActual();
            avanzar();
            
            TipoDato nuevoTipo = obtenerTipoDato(tokenTipo.valor);
            tabla.insertarSimbolo(tokenVar.valor, nuevoTipo, tokenVar.linea, tokenVar.columna);
            tipoBase = nuevoTipo;

            if (coincidir("OPERADOR_ASIGNACION", "=")) {
                TipoDato asignado = expresion(); // Captura el tipo de la derecha
                tabla.verificarAsignacion(tokenVar.valor, asignado, tokenVar.linea, tokenVar.columna);
            }
        }
    } else {
        reportarError("Expresion o componente sintactico invalido ('" + tokenActual().valor + "').");
    }
    
    return tipoBase;
}