// Anguiano Garcia Angel Yahir Guadalupe, Figueroa Robles Axel Israel, Molina Alvarado Alvaro Moises, Torres Martinez Miguel Angel
#include "AnalizadorSintactico.h"
#include "TablaSimbolos.hpp" // NUEVO: Incluir la tabla de simbolos

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
            // NUEVO: Registrar el parámetro en la tabla de símbolos
            tabla.insertarSimbolo(tokenVar.valor, obtenerTipoDato(tokenTipo.valor), tokenVar.linea, tokenVar.columna);
        }
    } else {
        reportarError("Parametro invalido (Se esperaba un tipo de dato)");
        if (!estaAlFinal() && !comprobar("SIMBOLO", ")") && !comprobar("SIMBOLO", ",")) {
            avanzar(); // Consumir token erroneo
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

void AnalizadorSintactico::sentenciaFor() {
    coincidir("PALABRA_CLAVE", "for");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues del 'for'.");
    }

    if (!comprobar("SIMBOLO", ";")) {
        inicializacion();
    }
    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Componente invalido o falta ';' tras la inicializacion en la estructura 'for'.");
    }

    if (!comprobar("SIMBOLO", ";")) {
        expresion();
    }
    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Componente invalido o falta ';' tras la condicion en la estructura 'for'.");
    }

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

void AnalizadorSintactico::bloque() {
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Se esperaba '{' al inicio del bloque.");
        return;
    }

    tabla.entrarAmbito(); // NUEVO: Gestión de Ámbito (Scope Local)

    while (!comprobar("SIMBOLO", "}") && !estaAlFinal()) {
        sentencia();
    }

    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Se esperaba '}' al cerrar el bloque.");
    }
    
    tabla.salirAmbito(); // NUEVO: Destruir el ámbito local
}

void AnalizadorSintactico::inicializacion() {
    expresion();
}

void AnalizadorSintactico::actualizacion() {
    expresion();
}

void AnalizadorSintactico::expresion() {
    Token tokenIzq = tokenActual(); // NUEVO: Para saber a quién le asignamos
    expresionSimple();

    if (comprobar("OPERADOR_RELACIONAL") || comprobar("OPERADOR_ASIGNACION") || comprobar("OPERADOR_LOGICO")) {
        Token tokenOp = tokenActual(); // NUEVO
        avanzar();
        
        Token tokenDer = tokenActual(); // NUEVO: Para saber qué valor tiene
        expresionSimple();
        
        // NUEVO: Verificacion de tipos de datos al detectar una asignación (=)
        if (tokenOp.valor == "=" && tokenIzq.tipo == "IDENTIFICADOR") {
            TipoDato tipoValor = TipoDato::DESCONOCIDO;
            if (tokenDer.tipo == "ENTERO") tipoValor = TipoDato::INT;
            else if (tokenDer.tipo == "DECIMAL") tipoValor = TipoDato::FLOAT;
            
            tabla.verificarAsignacion(tokenIzq.valor, tipoValor, tokenIzq.linea, tokenIzq.columna);
        }
    }
}

void AnalizadorSintactico::expresionSimple() {
    if (comprobar("IDENTIFICADOR") || comprobar("ENTERO") || comprobar("DECIMAL") || comprobar("CADENA")) {
        Token tokenActualInfo = tokenActual(); // NUEVO
        avanzar();
        
        // NUEVO: Si usamos una variable, verificamos que exista en la tabla
        if (tokenActualInfo.tipo == "IDENTIFICADOR") {
            tabla.buscarSimbolo(tokenActualInfo.valor, tokenActualInfo.linea, tokenActualInfo.columna);
        }

        if (comprobar("OPERADOR_INCREMENTO") || comprobar("OPERADOR_ARITMETICO")) {
            avanzar();
            if (comprobar("IDENTIFICADOR") || comprobar("ENTERO") || comprobar("DECIMAL")) {
                Token tokenDerInfo = tokenActual(); // NUEVO
                avanzar();
                
                // NUEVO: Verificar si el segundo operando existe (si es variable)
                if (tokenDerInfo.tipo == "IDENTIFICADOR") {
                    tabla.buscarSimbolo(tokenDerInfo.valor, tokenDerInfo.linea, tokenDerInfo.columna);
                }
            }
        }
    } else if (comprobar("PALABRA_CLAVE", "int") || comprobar("PALABRA_CLAVE", "float") || comprobar("PALABRA_CLAVE", "char")) {
        Token tokenTipo = tokenActual(); // NUEVO: Guardar token de tipo
        avanzar(); // Tipo de dato en declaración
        
        if (comprobar("IDENTIFICADOR")) {
            Token tokenVar = tokenActual(); // NUEVO: Guardar token de variable
            avanzar();
            
            // NUEVO: Insertar variable local en la tabla
            tabla.insertarSimbolo(tokenVar.valor, obtenerTipoDato(tokenTipo.valor), tokenVar.linea, tokenVar.columna);

            if (coincidir("OPERADOR_ASIGNACION", "=")) {
                expresion();
            }
        }
    } else {
        reportarError("Expresion o componente sintactico invalido ('" + tokenActual().valor + "').");
    }
}
