// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#include "AnalizadorSintactico.h"
#include <iostream>

using namespace std;

// Constructor: inicializa el estado léxico, sintáctico y el contexto semántico
AnalizadorSintactico::AnalizadorSintactico()
    : indiceActual(0),
      dentroDeFuncion(false),
      tipoRetornoFuncionActual(TipoDato::VOID),
      nombreFuncionActual(""),
      nivelBucles(0) {}

TipoDato AnalizadorSintactico::tipoDatoDesdeString(const string& tipoStr) const {
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

void AnalizadorSintactico::imprimirErrores() const {
    if (errores.empty()) {
        cout << "\n>>> [ÉXITO] Análisis finalizado sin errores.\n";
        return;
    }
    cout << "\n>>> [ERRORES DETECTADOS]:\n";
    for (const auto& err : errores) {
        cout << "  - [" << err.tipo << "] Línea " << err.linea 
             << ", Columna " << err.columna << ": " << err.mensaje << "\n";
    }
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
    errores.push_back({t.linea, t.columna, "Sintáctico", mensaje});
}

void AnalizadorSintactico::reportarErrorSemantico(const string& mensaje, int linea, int columna) {
    errores.push_back({linea, columna, "Semántico", mensaje});
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
    dentroDeFuncion = false;
    nombreFuncionActual = "";
    tipoRetornoFuncionActual = TipoDato::VOID;
    nivelBucles = 0;

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
        reportarError("Cuerpo de clase inválido en la declaración de 'class' (Falta '{')");
    }
    
    cuerpoClase();
    
    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Cuerpo de clase inválido en la declaración de 'class' (Falta '}')");
    }
}

void AnalizadorSintactico::cuerpoClase() {
    while (!estaAlFinal() && !comprobar("SIMBOLO", "}")) {
        if (esTipoDato()) {
            declaracionMetodoOVariable();
        } else {
            reportarError("Se esperaba una declaración de método o variable dentro de la clase");
            sincronizar(); 
        }
    }
}

void AnalizadorSintactico::declaracionMetodoOVariable() {
    Token tokenTipo = tokenActual();
    avanzar(); 
    
    Token tokenVar = tokenActual();
    if (!coincidir("IDENTIFICADOR")) {
        reportarError("Se esperaba un identificador después del tipo de dato.");
        return;
    }

    TipoDato tipoDeclarado = tipoDatoDesdeString(tokenTipo.valor);

    // Detección de método vs variable
    if (comprobar("SIMBOLO", "(")) {
        declaracionMetodo(tipoDeclarado, tokenVar.valor, tokenVar.linea, tokenVar.columna);
    } else {
        tablaSimbolos.insertarSimbolo(tokenVar.valor, tipoDeclarado, tokenVar.linea, tokenVar.columna);

        if (coincidir("OPERADOR_ASIGNACION", "=")) {
            TipoDato tipoExpr = expresion();
            tablaSimbolos.verificarAsignacion(tokenVar.valor, tipoExpr, tokenVar.linea, tokenVar.columna);
        }
        if (!coincidir("SIMBOLO", ";")) {
            reportarError("Se esperaba ';' al final de la declaración de variable.");
        }
    }
}

void AnalizadorSintactico::declaracionMetodo(TipoDato tipoRetorno, const string& nombre, int linea, int columna) {
    coincidir("SIMBOLO", "(");

    vector<pair<string, TipoDato>> listaParametros;
    parametros(listaParametros);

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Falta ')' en la declaración de parámetros del método.");
    }

    // Extraer solo los tipos para la firma en la tabla
    vector<TipoDato> tiposFirma;
    for (const auto& p : listaParametros) {
        tiposFirma.push_back(p.second);
    }

    // Registrar la función en la tabla de símbolos
    tablaSimbolos.insertarFuncion(nombre, tipoRetorno, tiposFirma, linea, columna);

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Cuerpo de método inválido en la declaración (Falta '{').");
        sincronizar();
        return;
    }

    // Configurar contexto semántico del método
    bool contextoPrevioFuncion = dentroDeFuncion;
    TipoDato retornoPrevio = tipoRetornoFuncionActual;
    string nombrePrevio = nombreFuncionActual;

    dentroDeFuncion = true;
    tipoRetornoFuncionActual = tipoRetorno;
    nombreFuncionActual = nombre;

    // Abrir ámbito e insertar parámetros como variables locales
    tablaSimbolos.entrarAmbito();
    for (const auto& p : listaParametros) {
        tablaSimbolos.insertarSimbolo(p.first, p.second, linea, columna);
    }

    bloque(); // Parsea el cuerpo del método

    tablaSimbolos.salirAmbito();

    // Restaurar contexto previo
    dentroDeFuncion = contextoPrevioFuncion;
    tipoRetornoFuncionActual = retornoPrevio;
    nombreFuncionActual = nombrePrevio;
}

void AnalizadorSintactico::parametros(vector<pair<string, TipoDato>>& listaParams) {
    if (!comprobar("SIMBOLO", ")")) { 
        parametro(listaParams);
        while (coincidir("SIMBOLO", ",")) {
            parametro(listaParams);
        }
    }
}

void AnalizadorSintactico::parametro(vector<pair<string, TipoDato>>& listaParams) {
    if (esTipoDato()) {
        Token tokenTipo = tokenActual();
        avanzar();
        
        Token tokenVar = tokenActual();
        if (!coincidir("IDENTIFICADOR")) {
            reportarError("Parámetro inválido (Falta identificador).");
        } else {
            listaParams.push_back({tokenVar.valor, tipoDatoDesdeString(tokenTipo.valor)});
        }
    } else {
        reportarError("Parámetro inválido (Se esperaba un tipo de dato).");
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
    else if (comprobar("PALABRA_CLAVE", "return")) {
        sentenciaReturn();
    }
    else if (comprobar("SIMBOLO", "{")) {
        tablaSimbolos.entrarAmbito();
        bloque();
        tablaSimbolos.salirAmbito();
    } 
    else {
        expresion();
        if (!coincidir("SIMBOLO", ";")) {
            reportarError("Se esperaba ';' al final de la sentencia.");
            sincronizar();
        }
    }
}

void AnalizadorSintactico::sentenciaReturn() {
    Token tokenRet = tokenActual();
    coincidir("PALABRA_CLAVE", "return");

    if (!dentroDeFuncion) {
        reportarErrorSemantico("Sentencia 'return' fuera del cuerpo de una función.", 
                               tokenRet.linea, tokenRet.columna);
    }

    TipoDato tipoRetornado = TipoDato::VOID;
    if (!comprobar("SIMBOLO", ";")) {
        tipoRetornado = expresion();
    }

    if (dentroDeFuncion) {
        if (tipoRetornado != tipoRetornoFuncionActual) {
            reportarErrorSemantico("El método '" + nombreFuncionActual + "' debe retornar '" +
                                   tipoDatoToString(tipoRetornoFuncionActual) + "' pero retornó '" +
                                   tipoDatoToString(tipoRetornado) + "'.", 
                                   tokenRet.linea, tokenRet.columna);
        }
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' al final de la sentencia 'return'.");
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
    Token tokenIf = tokenActual();
    coincidir("PALABRA_CLAVE", "if");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' después del 'if'.");
    }

    TipoDato tipoCond = expresion();
    if (tipoCond != TipoDato::BOOLEAN && tipoCond != TipoDato::DESCONOCIDO) {
        reportarErrorSemantico("La condición del 'if' debe ser de tipo boolean (recibido: " + 
                               tipoDatoToString(tipoCond) + ").", tokenIf.linea, tokenIf.columna);
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condición del 'if'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'if'.");
        sincronizar();
        return;
    }

    tablaSimbolos.entrarAmbito();
    bloque();
    tablaSimbolos.salirAmbito();

    if (coincidir("PALABRA_CLAVE", "else")) {
        if (!comprobar("SIMBOLO", "{")) {
            reportarError("Falta la llave de apertura '{' en la estructura 'else'.");
            sincronizar();
            return;
        }
        tablaSimbolos.entrarAmbito();
        bloque();
        tablaSimbolos.salirAmbito();
    }
}

void AnalizadorSintactico::sentenciaWhile() {
    Token tokenWhile = tokenActual();
    coincidir("PALABRA_CLAVE", "while");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' después del 'while'.");
    }

    TipoDato tipoCond = expresion();
    if (tipoCond != TipoDato::BOOLEAN && tipoCond != TipoDato::DESCONOCIDO) {
        reportarErrorSemantico("La condición del 'while' debe ser de tipo boolean (recibido: " + 
                               tipoDatoToString(tipoCond) + ").", tokenWhile.linea, tokenWhile.columna);
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condición del 'while'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'while'.");
        sincronizar();
        return;
    }

    nivelBucles++;
    tablaSimbolos.entrarAmbito();
    bloque();
    tablaSimbolos.salirAmbito();
    nivelBucles--;
}

void AnalizadorSintactico::sentenciaFor() {
    coincidir("PALABRA_CLAVE", "for");
    if (!coincidir("SIMBOLO", "(")) reportarError("Se esperaba '(' después del 'for'.");
    
    tablaSimbolos.entrarAmbito(); // Ámbito para variable de control del for

    if (!comprobar("SIMBOLO", ";")) inicializacion();
    if (!coincidir("SIMBOLO", ";")) reportarError("Componente inválido o falta ';' tras inicialización en 'for'.");
    
    if (!comprobar("SIMBOLO", ";")) {
        Token tokenCond = tokenActual();
        TipoDato tipoCond = expresion();
        if (tipoCond != TipoDato::BOOLEAN && tipoCond != TipoDato::DESCONOCIDO) {
            reportarErrorSemantico("La condición del 'for' debe ser de tipo boolean.", tokenCond.linea, tokenCond.columna);
        }
    }
    if (!coincidir("SIMBOLO", ";")) reportarError("Componente inválido o falta ';' tras condición en 'for'.");
    
    if (!comprobar("SIMBOLO", ")")) actualizacion();
    if (!coincidir("SIMBOLO", ")")) reportarError("Se esperaba ')' tras actualización del 'for'.");
    
    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en 'for'.");
        sincronizar();
        tablaSimbolos.salirAmbito();
        return;
    }

    nivelBucles++;
    bloque();
    nivelBucles--;

    tablaSimbolos.salirAmbito();
}

void AnalizadorSintactico::bloque() {
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Se esperaba '{' al inicio del bloque.");
        return;
    }

    while (!comprobar("SIMBOLO", "}") && !estaAlFinal()) {
        sentencia();
    }

    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Falta la llave de cierre '}' en el bloque.");
    }
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
                tablaSimbolos.verificarAsignacion(tokenIzq.valor, tipoDer, tokenIzq.linea, tokenIzq.columna);
            }
            tipoIzq = tipoDer;
        } 
        else if (tokenOp.tipo == "OPERADOR_RELACIONAL") {
            bool izqNum = (tipoIzq == TipoDato::INT || tipoIzq == TipoDato::FLOAT);
            bool derNum = (tipoDer == TipoDato::INT || tipoDer == TipoDato::FLOAT);
            
            if (!((izqNum && derNum) || (tipoIzq == tipoDer && tipoIzq != TipoDato::DESCONOCIDO))) {
                reportarErrorSemantico("No se pueden comparar tipos incompatibles (" + tokenOp.valor + ").", 
                                       tokenOp.linea, tokenOp.columna);
            }
            tipoIzq = TipoDato::BOOLEAN; 
        } 
        else if (tokenOp.tipo == "OPERADOR_LOGICO") {
            if (tipoIzq != TipoDato::BOOLEAN || tipoDer != TipoDato::BOOLEAN) {
                reportarErrorSemantico("Operador lógico '" + tokenOp.valor + "' requiere operandos booleanos.", 
                                       tokenOp.linea, tokenOp.columna);
            }
            tipoIzq = TipoDato::BOOLEAN;
        }
    }
    return tipoIzq;
}

TipoDato AnalizadorSintactico::expresionSimple() {
    TipoDato tipoBase = TipoDato::DESCONOCIDO;

    // 1. Soporte para paréntesis agrupados: ( 10.0 + 20.0 )
    if (comprobar("SIMBOLO", "(")) {
        avanzar(); // consume '('
        tipoBase = expresion();
        if (!coincidir("SIMBOLO", ")")) {
            reportarError("Se esperaba ')' para cerrar la expresión.");
        }
    }
    else if (comprobar("IDENTIFICADOR")) {
        Token idToken = tokenActual();
        avanzar();

        if (comprobar("SIMBOLO", "(")) {
            tipoBase = llamadaFuncion(idToken.valor, idToken.linea, idToken.columna);
        } else {
            // Variable simple
            Simbolo* sim = tablaSimbolos.buscarSimbolo(idToken.valor, idToken.linea, idToken.columna);
            if (sim) tipoBase = sim->tipo;
        }
    } 
    else if (comprobar("ENTERO")) {
        avanzar();
        tipoBase = TipoDato::INT;
    } 
    else if (comprobar("DECIMAL")) {
        avanzar();
        tipoBase = TipoDato::FLOAT;
    } 
    else if (comprobar("CADENA")) {
        avanzar();
        tipoBase = TipoDato::STRING;
    } 
    else if (comprobar("PALABRA_CLAVE", "true") || comprobar("PALABRA_CLAVE", "false")) {
        avanzar();
        tipoBase = TipoDato::BOOLEAN;
    }
    else if (esTipoDato()) {
        Token tokenTipo = tokenActual();
        avanzar(); 
        
        if (comprobar("IDENTIFICADOR")) {
            Token tokenVar = tokenActual();
            avanzar();
            
            TipoDato nuevoTipo = tipoDatoDesdeString(tokenTipo.valor);
            tablaSimbolos.insertarSimbolo(tokenVar.valor, nuevoTipo, tokenVar.linea, tokenVar.columna);
            tipoBase = nuevoTipo;

            if (coincidir("OPERADOR_ASIGNACION", "=")) {
                TipoDato asignado = expresion();
                tablaSimbolos.verificarAsignacion(tokenVar.valor, asignado, tokenVar.linea, tokenVar.columna);
            }
        }
        return tipoBase;
    } 
    else {
        reportarError("Expresión o componente sintáctico inválido ('" + tokenActual().valor + "').");
        return TipoDato::DESCONOCIDO;
    }

    // Operaciones aritméticas encadenadas (+, -, *, /)
    while (comprobar("OPERADOR_ARITMETICO")) {
        Token tokenOp = tokenActual();
        avanzar();

        if (tokenOp.valor == "/" && (tokenActual().valor == "0" || tokenActual().valor == "0.0")) {
            reportarErrorSemantico("División por cero detectada.", tokenOp.linea, tokenOp.columna);
        }

        Token tokenDer = tokenActual();
        TipoDato tipoDer = TipoDato::DESCONOCIDO;
        
        if (comprobar("IDENTIFICADOR")) {
            Token idDer = tokenActual();
            avanzar();
            if (comprobar("SIMBOLO", "(")) {
                tipoDer = llamadaFuncion(idDer.valor, idDer.linea, idDer.columna);
            } else {
                Simbolo* sim = tablaSimbolos.buscarSimbolo(idDer.valor, idDer.linea, idDer.columna);
                if (sim) tipoDer = sim->tipo;
            }
        } else if (comprobar("ENTERO")) {
            avanzar();
            tipoDer = TipoDato::INT;
        } else if (comprobar("DECIMAL")) {
            avanzar();
            tipoDer = TipoDato::FLOAT;
        } else {
            reportarError("Se esperaba un número o variable después del operador aritmético.");
        }

        bool izqNum = (tipoBase == TipoDato::INT || tipoBase == TipoDato::FLOAT);
        bool derNum = (tipoDer == TipoDato::INT || tipoDer == TipoDato::FLOAT);

        if (!izqNum || !derNum) {
            reportarErrorSemantico("No se puede aplicar el operador aritmético '" + tokenOp.valor + 
                                   "' a tipos incompatibles.", tokenOp.linea, tokenOp.columna);
            tipoBase = TipoDato::DESCONOCIDO;
        } else {
            if (tipoBase == TipoDato::FLOAT || tipoDer == TipoDato::FLOAT) {
                tipoBase = TipoDato::FLOAT;
            }
        }
    }

    if (comprobar("OPERADOR_INCREMENTO")) {
        avanzar();
    }

    return tipoBase;
}

TipoDato AnalizadorSintactico::llamadaFuncion(const string& nombre, int linea, int columna) {
    coincidir("SIMBOLO", "(");

    SimboloFuncion* funcion = tablaSimbolos.buscarFuncion(nombre, linea, columna);
    vector<TipoDato> tiposArgs;

    if (!comprobar("SIMBOLO", ")")) {
        do {
            tiposArgs.push_back(expresion());
        } while (coincidir("SIMBOLO", ","));
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras los argumentos del método.");
    }

    if (funcion == nullptr) {
        return TipoDato::DESCONOCIDO;
    }

    if (tiposArgs.size() != funcion->tiposParametros.size()) {
        reportarErrorSemantico("Llamada al método '" + nombre + "' con " + 
                               to_string(tiposArgs.size()) + " argumentos en lugar de " + 
                               to_string(funcion->tiposParametros.size()) + ".", linea, columna);
        return funcion->tipoRetorno;
    }

    for (size_t i = 0; i < tiposArgs.size(); ++i) {
        if (tiposArgs[i] != funcion->tiposParametros[i] && tiposArgs[i] != TipoDato::DESCONOCIDO) {
            reportarErrorSemantico("Argumento " + to_string(i + 1) + " en llamada a '" + nombre + 
                                   "' es de tipo '" + tipoDatoToString(tiposArgs[i]) + 
                                   "', se esperaba '" + tipoDatoToString(funcion->tiposParametros[i]) + "'.", 
                                   linea, columna);
        }
    }

    return funcion->tipoRetorno;
}