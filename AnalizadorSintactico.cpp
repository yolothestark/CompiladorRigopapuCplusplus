// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#include "AnalizadorSintactico.h"
#include <iostream>
#include <unordered_set>

using namespace std;

AnalizadorSintactico::AnalizadorSintactico()
    : indiceActual(0),
      dentroDeFuncion(false),
      tipoRetornoFuncionActual(TipoDato::VOID),
      nombreFuncionActual(""),
      claseActual(""),
      nivelBucles(0),
      nivelSwitch(0) {
    tablaSimbolos.configurarNotificadorError([this](const string& msg, int linea, int col) {
        this->reportarErrorSemantico(msg, linea, col);
    });
}

TipoDato AnalizadorSintactico::tipoDatoDesdeString(const string& tipoStr) const {
    if (tipoStr == "int") return TipoDato::INT;
    if (tipoStr == "float") return TipoDato::FLOAT;
    if (tipoStr == "char") return TipoDato::CHAR;
    if (tipoStr == "void") return TipoDato::VOID;
    if (tipoStr == "boolean" || tipoStr == "bool") return TipoDato::BOOLEAN;
    if (tipoStr == "string") return TipoDato::STRING;
    return TipoDato::DESCONOCIDO;
}

const vector<ErrorSintactico>& AnalizadorSintactico::obtenerErrores() const {
    return errores;
}

const GeneradorCodigoIntermedio& AnalizadorSintactico::obtenerGenerador() const {
    return generador;
}

bool AnalizadorSintactico::tieneErrores() const {
    return !errores.empty();
}

void AnalizadorSintactico::imprimirErrores() const {
    if (errores.empty()) {
        cout << "\n>>> [EXITO] Analisis finalizado sin errores.\n";
        return;
    }
    cout << "\n>>> [ERRORES DETECTADOS]:\n";
    for (const auto& err : errores) {
        cout << "  - [" << err.tipo << "] Linea " << err.linea 
             << ", Columna " << err.columna << ": " << err.mensaje << "\n";
    }
}

void AnalizadorSintactico::imprimirCodigoIntermedio() const {
    generador.imprimirCodigoIntermedio();
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

Token AnalizadorSintactico::tokenSiguiente() const {
    if (indiceActual + 1 < tokens.size()) {
        return tokens[indiceActual + 1];
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
    errores.push_back({t.linea, t.columna, "Sintactico", mensaje});
}

void AnalizadorSintactico::reportarErrorSemantico(const string& mensaje, int linea, int columna) {
    errores.push_back({linea, columna, "Semantico", mensaje});
}

void AnalizadorSintactico::sincronizar() {
    avanzar();
    while (!estaAlFinal()) {
        if (tokenAnterior().valor == ";") return;

        if (comprobar("PALABRA_CLAVE", "if") ||
            comprobar("PALABRA_CLAVE", "switch") ||
            comprobar("PALABRA_CLAVE", "while") ||
            comprobar("PALABRA_CLAVE", "for") ||
            comprobar("PALABRA_CLAVE", "do") ||
            comprobar("PALABRA_CLAVE", "int") ||
            comprobar("PALABRA_CLAVE", "float") ||
            comprobar("PALABRA_CLAVE", "char") ||
            comprobar("PALABRA_CLAVE", "void") ||
            comprobar("PALABRA_CLAVE", "boolean") ||
            comprobar("PALABRA_CLAVE", "string") ||
            comprobar("PALABRA_CLAVE", "class") ||
            comprobar("PALABRA_CLAVE", "return") ||
            comprobar("PALABRA_CLAVE", "break")) {
            return;
        }
        avanzar();
    }
}

void AnalizadorSintactico::analizar(const vector<Token>& tokensEntrada) {
    tokens = tokensEntrada;
    indiceActual = 0;
    errores.clear();
    tablaSimbolos.reiniciar();
    generador.reiniciar();
    dentroDeFuncion = false;
    nombreFuncionActual = "";
    claseActual = "";
    tipoRetornoFuncionActual = TipoDato::VOID;
    nivelBucles = 0;
    nivelSwitch = 0;
    pilaEtiquetasFinBucle.clear();

    programa();
}

void AnalizadorSintactico::programa() {
    while (!estaAlFinal()) {
        if (comprobar("PALABRA_CLAVE", "class")) {
            declaracionClase();
        } else if (esTipoDato()) {
            declaracionMetodoOVariable();
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
    
    Token tokenNombre = tokenActual();
    if (!coincidir("IDENTIFICADOR")) {
        reportarError("Se esperaba un identificador para el nombre de la clase.");
        return;
    }

    string nombreClase = tokenNombre.valor;
    tablaSimbolos.insertarClase(nombreClase, tokenNombre.linea, tokenNombre.columna);
    generador.emitirSeccion("Definicion de la clase " + nombreClase);
    
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Cuerpo de clase invalido en la declaracion de 'class' (Falta '{').");
    }
    
    string clasePrevia = claseActual;
    claseActual = nombreClase;
    tablaSimbolos.entrarAmbito();

    cuerpoClase();

    tablaSimbolos.salirAmbito();
    claseActual = clasePrevia;
    
    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Cuerpo de clase invalido en la declaracion de 'class' (Falta '}').");
    }
}

void AnalizadorSintactico::cuerpoClase() {
    while (!estaAlFinal() && !comprobar("SIMBOLO", "}")) {
        if (esTipoDato()) {
            declaracionMetodoOVariable();
        } else {
            reportarError("Se esperaba una declaracion de metodo o atributo dentro de la clase.");
            sincronizar(); 
        }
    }
}

void AnalizadorSintactico::declaracionMetodoOVariable() {
    Token tokenTipo = tokenActual();
    avanzar(); 
    
    Token tokenVar = tokenActual();
    if (!coincidir("IDENTIFICADOR")) {
        reportarError("Se esperaba un identificador despues del tipo de dato.");
        return;
    }

    TipoDato tipoDeclarado = tipoDatoDesdeString(tokenTipo.valor);

    if (comprobar("SIMBOLO", "(")) {
        declaracionMetodo(tipoDeclarado, tokenVar.valor, tokenVar.linea, tokenVar.columna);
    } else {
        tablaSimbolos.insertarSimbolo(tokenVar.valor, tipoDeclarado, tokenVar.linea, tokenVar.columna);

        if (coincidir("OPERADOR_ASIGNACION", "=")) {
            ResultadoExpresion resExpr = expresion();
            tablaSimbolos.verificarAsignacion(tokenVar.valor, resExpr.tipo, tokenVar.linea, tokenVar.columna);
            if (!resExpr.lugar.empty()) {
                generador.emitir(tokenVar.valor + " = " + resExpr.lugar, 
                                 "Asigna el valor " + resExpr.lugar + " a " + tokenVar.valor);
            }
        }
        if (!coincidir("SIMBOLO", ";")) {
            reportarError("Se esperaba ';' al final de la declaracion de variable.");
        }
    }
}

void AnalizadorSintactico::declaracionMetodo(TipoDato tipoRetorno, const string& nombre, int linea, int columna) {
    coincidir("SIMBOLO", "(");

    vector<pair<string, TipoDato>> listaParametros;
    parametros(listaParametros);

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Falta ')' en la declaracion de parametros del metodo.");
    }

    vector<TipoDato> tiposFirma;
    for (const auto& p : listaParametros) {
        tiposFirma.push_back(p.second);
    }

    string etiquetaMetodo = generador.nuevaEtiqueta();
    tablaSimbolos.insertarFuncion(nombre, tipoRetorno, tiposFirma, linea, columna, etiquetaMetodo, claseActual);

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Cuerpo de metodo invalido en la declaracion (Falta '{').");
        sincronizar();
        return;
    }

    if (claseActual.empty()) {
        generador.emitirSeccion("Definicion del metodo " + nombre);
    }
    generador.emitirEtiqueta(etiquetaMetodo, "Etiqueta " + etiquetaMetodo + " marca el inicio del metodo " + nombre);

    bool contextoPrevioFuncion = dentroDeFuncion;
    TipoDato retornoPrevio = tipoRetornoFuncionActual;
    string nombrePrevio = nombreFuncionActual;

    dentroDeFuncion = true;
    tipoRetornoFuncionActual = tipoRetorno;
    nombreFuncionActual = nombre;

    tablaSimbolos.entrarAmbito();
    for (const auto& p : listaParametros) {
        tablaSimbolos.insertarSimbolo(p.first, p.second, linea, columna);
    }

    bloque();

    tablaSimbolos.salirAmbito();

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
            reportarError("Parametro invalido (Falta identificador).");
        } else {
            listaParams.push_back({tokenVar.valor, tipoDatoDesdeString(tokenTipo.valor)});
        }
    } else {
        reportarError("Parametro invalido (Se esperaba un tipo de dato).");
        if (!estaAlFinal() && !comprobar("SIMBOLO", ")") && !comprobar("SIMBOLO", ",")) {
            avanzar();
        }
    }
}

void AnalizadorSintactico::sentencia() {
    if (comprobar("PALABRA_CLAVE", "if") || 
        comprobar("PALABRA_CLAVE", "switch") ||
        comprobar("PALABRA_CLAVE", "while") || 
        comprobar("PALABRA_CLAVE", "for") ||
        comprobar("PALABRA_CLAVE", "do")) {
        estructuraControl();
    } 
    else if (comprobar("PALABRA_CLAVE", "return")) {
        sentenciaReturn();
    }
    else if (comprobar("PALABRA_CLAVE", "break")) {
        sentenciaBreak();
    }
    else if (comprobar("SIMBOLO", "{")) {
        tablaSimbolos.entrarAmbito();
        bloque();
        tablaSimbolos.salirAmbito();
    }
    else if (comprobar("IDENTIFICADOR") && tokenSiguiente().tipo == "IDENTIFICADOR") {
        // Declaración de instancia de clase: Persona p = new Persona();
        declaracionObjetoOExpresion();
    }
    else {
        expresion();
        if (!coincidir("SIMBOLO", ";")) {
            reportarError("Se esperaba ';' al final de la sentencia.");
            sincronizar();
        }
    }
}

void AnalizadorSintactico::declaracionObjetoOExpresion() {
    Token tokenClase = avanzar(); // Nombre de la clase (ej. Persona)
    Token tokenObj = avanzar();   // Nombre del objeto (ej. p)

    if (!tablaSimbolos.existeClase(tokenClase.valor)) {
        reportarErrorSemantico("La clase '" + tokenClase.valor + "' no ha sido declarada.", 
                               tokenClase.linea, tokenClase.columna);
    }

    tablaSimbolos.insertarSimbolo(tokenObj.valor, TipoDato::OBJETO, tokenObj.linea, tokenObj.columna, tokenClase.valor);

    if (coincidir("OPERADOR_ASIGNACION", "=")) {
        if (!coincidir("PALABRA_CLAVE", "new")) {
            reportarError("Se esperaba la palabra clave 'new' para instanciar el objeto '" + tokenObj.valor + "'.");
        }
        Token tokenClaseNew = tokenActual();
        if (!coincidir("IDENTIFICADOR")) {
            reportarError("Se esperaba el nombre de la clase despues de 'new'.");
        } else if (tokenClaseNew.valor != tokenClase.valor) {
            reportarErrorSemantico("El constructor '" + tokenClaseNew.valor + 
                                   "' no coincide con el tipo de clase '" + tokenClase.valor + "'.",
                                   tokenClaseNew.linea, tokenClaseNew.columna);
        }

        if (!coincidir("SIMBOLO", "(") || !coincidir("SIMBOLO", ")")) {
            reportarError("Se esperaban parentesis '()' en el constructor de '" + tokenClase.valor + "'.");
        }

        generador.emitirSeccion("Creacion del objeto " + tokenClase.valor);
        generador.emitir("obj = create " + tokenClase.valor, 
                         "Representa la creacion del objeto " + tokenObj.valor);
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' al final de la creacion del objeto.");
    }
}

void AnalizadorSintactico::sentenciaReturn() {
    Token tokenRet = tokenActual();
    coincidir("PALABRA_CLAVE", "return");

    if (!dentroDeFuncion) {
        reportarErrorSemantico("Sentencia 'return' fuera del cuerpo de un metodo.", 
                               tokenRet.linea, tokenRet.columna);
    }

    ResultadoExpresion resRet = {TipoDato::VOID, ""};
    if (!comprobar("SIMBOLO", ";")) {
        resRet = expresion();
    }

    if (dentroDeFuncion) {
        if (resRet.tipo != tipoRetornoFuncionActual && resRet.tipo != TipoDato::DESCONOCIDO) {
            reportarErrorSemantico("El metodo '" + nombreFuncionActual + "' debe retornar '" +
                                   tipoDatoToString(tipoRetornoFuncionActual) + "' pero retorno '" +
                                   tipoDatoToString(resRet.tipo) + "'.", 
                                   tokenRet.linea, tokenRet.columna);
        }
    }

    if (!resRet.lugar.empty()) {
        generador.emitir("return " + resRet.lugar, "Retorna el valor de " + resRet.lugar);
    } else {
        generador.emitir("return", "Retorno del metodo void");
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' al final de la sentencia 'return'.");
    }
}

void AnalizadorSintactico::sentenciaBreak() {
    Token tokenBrk = tokenActual();
    coincidir("PALABRA_CLAVE", "break");

    if (nivelBucles == 0 && nivelSwitch == 0) {
        reportarErrorSemantico("La sentencia 'break' solo puede usarse dentro de un bucle o estructura 'switch'.",
                               tokenBrk.linea, tokenBrk.columna);
    }

    if (nivelSwitch > 0) {
        generador.emitir("goto <BREAK_SWITCH>", "Salta al final de la estructura switch");
    } else if (!pilaEtiquetasFinBucle.empty()) {
        string etiqFin = pilaEtiquetasFinBucle.back();
        generador.emitir("goto " + etiqFin, "Sale del bucle actual hacia " + etiqFin);
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' despues de 'break'.");
    }
}

void AnalizadorSintactico::estructuraControl() {
    if (comprobar("PALABRA_CLAVE", "if")) {
        sentenciaIf();
    } else if (comprobar("PALABRA_CLAVE", "switch")) {
        sentenciaSwitch();
    } else if (comprobar("PALABRA_CLAVE", "while")) {
        sentenciaWhile();
    } else if (comprobar("PALABRA_CLAVE", "for")) {
        sentenciaFor();
    } else if (comprobar("PALABRA_CLAVE", "do")) {
        sentenciaDoWhile();
    }
}

void AnalizadorSintactico::sentenciaIf() {
    Token tokenIf = tokenActual();
    coincidir("PALABRA_CLAVE", "if");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues del 'if'.");
    }

    ResultadoExpresion cond = expresion();
    if (cond.tipo != TipoDato::BOOLEAN && cond.tipo != TipoDato::DESCONOCIDO) {
        reportarErrorSemantico("La condicion del 'if' debe ser de tipo boolean (recibido: " + 
                               tipoDatoToString(cond.tipo) + ").", tokenIf.linea, tokenIf.columna);
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condicion del 'if'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'if'.");
        sincronizar();
        return;
    }

    // Capturar instrucciones del bloque if en un buffer
    generador.iniciarBuffer();
    tablaSimbolos.entrarAmbito();
    bloque();
    tablaSimbolos.salirAmbito();
    vector<InstruccionTAC> instruccionesIf = generador.extraerBuffer();

    if (coincidir("PALABRA_CLAVE", "else")) {
        if (!comprobar("SIMBOLO", "{")) {
            reportarError("Falta la llave de apertura '{' en la estructura 'else'.");
            sincronizar();
            return;
        }

        string etiqIf = generador.nuevaEtiqueta();
        string etiqFin = generador.nuevaEtiqueta();

        generador.emitir("if " + cond.lugar + " goto " + etiqIf, 
                         "Si " + cond.lugar + " es verdadero, salta a la etiqueta " + etiqIf);

        // Emitir instrucciones del bloque else cuando la condición es falsa
        tablaSimbolos.entrarAmbito();
        bloque();
        tablaSimbolos.salirAmbito();

        generador.emitir("goto " + etiqFin, 
                         "Salta a la etiqueta " + etiqFin + " despues del bloque else");
        generador.emitirEtiqueta(etiqIf, "Etiqueta " + etiqIf + " para el bloque if");
        generador.agregarInstrucciones(instruccionesIf);
        generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final de la estructura condicional");
    } else {
        string etiqFin = generador.nuevaEtiqueta();
        generador.emitir("if not " + cond.lugar + " goto " + etiqFin, 
                         "Si " + cond.lugar + " es falso, salta a la etiqueta " + etiqFin);
        generador.agregarInstrucciones(instruccionesIf);
        generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final de la estructura condicional");
    }
}

void AnalizadorSintactico::sentenciaSwitch() {
    Token tokenSw = tokenActual();
    coincidir("PALABRA_CLAVE", "switch");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues de 'switch'.");
    }

    ResultadoExpresion exprSw = expresion();
    if (exprSw.tipo == TipoDato::BOOLEAN || exprSw.tipo == TipoDato::VOID) {
        reportarErrorSemantico("La expresion del 'switch' no puede ser de tipo '" + 
                               tipoDatoToString(exprSw.tipo) + "'.", tokenSw.linea, tokenSw.columna);
    }

    string tempSwitch = generador.nuevoTemporal();
    generador.emitir(tempSwitch + " = " + exprSw.lugar, 
                     tempSwitch + " es una etiqueta temporal para el valor de " + exprSw.lugar);

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la expresion del 'switch'.");
    }

    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'switch'.");
        sincronizar();
        return;
    }

    struct CasoSwitch {
        string valor;
        string etiqueta;
        vector<InstruccionTAC> instrucciones;
    };

    vector<CasoSwitch> casos;
    unordered_set<string> valoresCasosVistos;
    bool tieneDefault = false;
    string etiqDefault = "";
    vector<InstruccionTAC> instruccionesDefault;

    nivelSwitch++;
    tablaSimbolos.entrarAmbito();

    while (!comprobar("SIMBOLO", "}") && !estaAlFinal()) {
        if (coincidir("PALABRA_CLAVE", "case")) {
            Token tokenVal = tokenActual();
            TipoDato tipoCaso = TipoDato::DESCONOCIDO;
            if (comprobar("ENTERO")) {
                tipoCaso = TipoDato::INT;
                avanzar();
            } else if (comprobar("CARACTER")) {
                tipoCaso = TipoDato::CHAR;
                avanzar();
            } else if (comprobar("CADENA")) {
                tipoCaso = TipoDato::STRING;
                avanzar();
            } else {
                reportarError("Se esperaba una constante valida en 'case'.");
                avanzar();
            }

            if (exprSw.tipo != TipoDato::DESCONOCIDO && tipoCaso != TipoDato::DESCONOCIDO && exprSw.tipo != tipoCaso) {
                reportarErrorSemantico("El tipo del 'case' (" + tipoDatoToString(tipoCaso) + 
                                       ") es incompatible con la expresion del 'switch' (" + 
                                       tipoDatoToString(exprSw.tipo) + ").", tokenVal.linea, tokenVal.columna);
            }

            if (valoresCasosVistos.count(tokenVal.valor)) {
                reportarErrorSemantico("Valor de 'case' duplicado (" + tokenVal.valor + ") en 'switch'.",
                                       tokenVal.linea, tokenVal.columna);
            }
            valoresCasosVistos.insert(tokenVal.valor);

            if (!coincidir("SIMBOLO", ":")) {
                reportarError("Se esperaba ':' despues del valor del 'case'.");
            }

            string etiqCaso = generador.nuevaEtiqueta();
            generador.iniciarBuffer();
            while (!comprobar("PALABRA_CLAVE", "case") && 
                   !comprobar("PALABRA_CLAVE", "default") && 
                   !comprobar("SIMBOLO", "}") && !estaAlFinal()) {
                sentencia();
            }
            vector<InstruccionTAC> instsCaso = generador.extraerBuffer();
            casos.push_back({tokenVal.valor, etiqCaso, instsCaso});
        } 
        else if (coincidir("PALABRA_CLAVE", "default")) {
            if (!coincidir("SIMBOLO", ":")) {
                reportarError("Se esperaba ':' despues de 'default'.");
            }
            tieneDefault = true;
            etiqDefault = generador.nuevaEtiqueta();
            generador.iniciarBuffer();
            while (!comprobar("PALABRA_CLAVE", "case") && 
                   !comprobar("PALABRA_CLAVE", "default") && 
                   !comprobar("SIMBOLO", "}") && !estaAlFinal()) {
                sentencia();
            }
            instruccionesDefault = generador.extraerBuffer();
        } 
        else {
            reportarError("Se esperaba 'case' o 'default' dentro de 'switch'.");
            avanzar();
        }
    }

    tablaSimbolos.salirAmbito();
    nivelSwitch--;

    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Falta la llave de cierre '}' en la estructura 'switch'.");
    }

    string etiqFin = generador.nuevaEtiqueta();

    // 1. Emitir saltos condicionales para cada case
    for (const auto& c : casos) {
        generador.emitir("if " + tempSwitch + " == " + c.valor + " goto " + c.etiqueta,
                         "Si " + tempSwitch + " es " + c.valor + ", salta a la etiqueta " + c.etiqueta);
    }

    // 2. Emitir salto a default o al final
    if (tieneDefault) {
        generador.emitir("goto " + etiqDefault, 
                         "Si " + tempSwitch + " no coincide con ningun caso, salta a la etiqueta " + etiqDefault);
    } else {
        generador.emitir("goto " + etiqFin, 
                         "Si " + tempSwitch + " no coincide con ningun caso, salta al final (" + etiqFin + ")");
    }

    // 3. Emitir bloques de cada case
    for (const auto& c : casos) {
        generador.emitirEtiqueta(c.etiqueta, "Etiqueta " + c.etiqueta + " para el caso " + c.valor);
        for (auto inst : c.instrucciones) {
            if (inst.codigo == "goto <BREAK_SWITCH>") {
                inst.codigo = "goto " + etiqFin;
                inst.comentario = "Salta a la etiqueta " + etiqFin + " despues de manejar el caso " + c.valor;
            }
            generador.emitir(inst.codigo, inst.comentario);
        }
    }

    // 4. Emitir bloque default
    if (tieneDefault) {
        generador.emitirEtiqueta(etiqDefault, "Etiqueta " + etiqDefault + " para el caso default");
        for (auto inst : instruccionesDefault) {
            if (inst.codigo == "goto <BREAK_SWITCH>") {
                inst.codigo = "goto " + etiqFin;
                inst.comentario = "Salta a la etiqueta " + etiqFin + " despues del caso default";
            }
            generador.emitir(inst.codigo, inst.comentario);
        }
    }

    // 5. Etiqueta final del switch
    generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final de la estructura switch");
}

void AnalizadorSintactico::sentenciaWhile() {
    Token tokenWhile = tokenActual();
    coincidir("PALABRA_CLAVE", "while");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues del 'while'.");
    }

    string etiqInicio = generador.nuevaEtiqueta();
    generador.emitirEtiqueta(etiqInicio, "Etiqueta " + etiqInicio + " marca el inicio del bucle");

    ResultadoExpresion cond = expresion();
    if (cond.tipo != TipoDato::BOOLEAN && cond.tipo != TipoDato::DESCONOCIDO) {
        reportarErrorSemantico("La condicion del 'while' debe ser de tipo boolean (recibido: " + 
                               tipoDatoToString(cond.tipo) + ").", tokenWhile.linea, tokenWhile.columna);
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condicion del 'while'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'while'.");
        sincronizar();
        return;
    }

    string etiqFin = generador.nuevaEtiqueta();
    generador.emitir("if not " + cond.lugar + " goto " + etiqFin, 
                     "Si " + cond.lugar + " es falso, salta a la etiqueta " + etiqFin + " (fin del bucle)");

    nivelBucles++;
    pilaEtiquetasFinBucle.push_back(etiqFin);
    tablaSimbolos.entrarAmbito();
    bloque();
    tablaSimbolos.salirAmbito();
    pilaEtiquetasFinBucle.pop_back();
    nivelBucles--;

    generador.emitir("goto " + etiqInicio, "Salta de vuelta al inicio del bucle");
    generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final del bucle");
}

void AnalizadorSintactico::sentenciaFor() {
    coincidir("PALABRA_CLAVE", "for");
    if (!coincidir("SIMBOLO", "(")) reportarError("Se esperaba '(' despues del 'for'.");
    
    tablaSimbolos.entrarAmbito();

    if (!comprobar("SIMBOLO", ";")) inicializacion();
    if (!coincidir("SIMBOLO", ";")) reportarError("Componente invalido o falta ';' tras inicializacion en 'for'.");
    
    string etiqInicio = generador.nuevaEtiqueta();
    generador.emitirEtiqueta(etiqInicio, "Etiqueta " + etiqInicio + " marca el inicio del bucle");

    ResultadoExpresion cond = {TipoDato::BOOLEAN, "true"};
    if (!comprobar("SIMBOLO", ";")) {
        Token tokenCond = tokenActual();
        cond = expresion();
        if (cond.tipo != TipoDato::BOOLEAN && cond.tipo != TipoDato::DESCONOCIDO) {
            reportarErrorSemantico("La condicion del 'for' debe ser de tipo boolean.", tokenCond.linea, tokenCond.columna);
        }
    }
    if (!coincidir("SIMBOLO", ";")) reportarError("Componente invalido o falta ';' tras condicion en 'for'.");

    string etiqFin = generador.nuevaEtiqueta();
    generador.emitir("if not " + cond.lugar + " goto " + etiqFin,
                     "Si " + cond.lugar + " es falso, salta a la etiqueta " + etiqFin + " (fin del bucle)");

    // Capturar la actualización del for (ej. i++) para emitirla al final del cuerpo
    generador.iniciarBuffer();
    if (!comprobar("SIMBOLO", ")")) actualizacion();
    vector<InstruccionTAC> instruccionesActualizacion = generador.extraerBuffer();

    if (!coincidir("SIMBOLO", ")")) reportarError("Se esperaba ')' tras actualizacion del 'for'.");
    
    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en 'for'.");
        sincronizar();
        tablaSimbolos.salirAmbito();
        return;
    }

    nivelBucles++;
    pilaEtiquetasFinBucle.push_back(etiqFin);
    bloque();
    pilaEtiquetasFinBucle.pop_back();
    nivelBucles--;

    generador.agregarInstrucciones(instruccionesActualizacion);
    generador.emitir("goto " + etiqInicio, "Salta de vuelta al inicio del bucle");
    generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final del bucle");

    tablaSimbolos.salirAmbito();
}

void AnalizadorSintactico::sentenciaDoWhile() {
    coincidir("PALABRA_CLAVE", "do");

    string etiqInicio = generador.nuevaEtiqueta();
    generador.emitirEtiqueta(etiqInicio, "Etiqueta " + etiqInicio + " marca el inicio del bucle");

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'do-while'.");
        sincronizar();
        return;
    }

    nivelBucles++;
    tablaSimbolos.entrarAmbito();
    bloque();
    tablaSimbolos.salirAmbito();
    nivelBucles--;

    Token tokenWhile = tokenActual();
    if (!coincidir("PALABRA_CLAVE", "while")) {
        reportarError("Se esperaba 'while' despues del bloque 'do'.");
        return;
    }

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues de 'while' en 'do-while'.");
    }

    ResultadoExpresion cond = expresion();
    if (cond.tipo != TipoDato::BOOLEAN && cond.tipo != TipoDato::DESCONOCIDO) {
        reportarErrorSemantico("La condicion del 'do-while' debe ser de tipo boolean (recibido: " + 
                               tipoDatoToString(cond.tipo) + ").", tokenWhile.linea, tokenWhile.columna);
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condicion del 'do-while'.");
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' al final de la estructura 'do-while'.");
    }

    generador.emitir("if " + cond.lugar + " goto " + etiqInicio,
                     "Si " + cond.lugar + " es verdadero, salta de vuelta al inicio del bucle");
}

void AnalizadorSintactico::bloque() {
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Se esperaba '{' al inicio del bloque.");
        return;
    }

    while (!comprobar("SIMBOLO", "}") && !estaAlFinal()) {
        if (esTipoDato()) {
            declaracionMetodoOVariable();
        } else {
            sentencia();
        }
    }

    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Falta la llave de cierre '}' en el bloque.");
    }
}

void AnalizadorSintactico::inicializacion() { expresion(); }
void AnalizadorSintactico::actualizacion() { expresion(); }

ResultadoExpresion AnalizadorSintactico::expresion() {
    Token tokenIzq = tokenActual();
    ResultadoExpresion izq = expresionSimple();

    while (comprobar("OPERADOR_RELACIONAL") || comprobar("OPERADOR_ASIGNACION") || comprobar("OPERADOR_LOGICO")) {
        Token tokenOp = tokenActual();
        avanzar();
        
        if (tokenOp.valor == "=") {
            // Caso especial dentro de bucles para incremento directo: var = var + 1
            if (nivelBucles > 0 && tokenIzq.tipo == "IDENTIFICADOR" &&
                comprobar("IDENTIFICADOR", tokenIzq.valor) &&
                indiceActual + 2 < tokens.size() &&
                tokens[indiceActual + 1].tipo == "OPERADOR_ARITMETICO" &&
                (tokens[indiceActual + 1].valor == "+" || tokens[indiceActual + 1].valor == "-") &&
                tokens[indiceActual + 2].valor == "1") {
                avanzar(); // consume var
                string opArit = avanzar().valor; // consume + o -
                avanzar(); // consume 1
                tablaSimbolos.verificarAsignacion(tokenIzq.valor, TipoDato::INT, tokenIzq.linea, tokenIzq.columna);
                string accion = (opArit == "+") ? "Incrementa " : "Decrementa ";
                generador.emitir(tokenIzq.valor + " = " + tokenIzq.valor + " " + opArit + " 1",
                                 accion + tokenIzq.valor);
                izq = {TipoDato::INT, tokenIzq.valor};
                continue;
            }

            ResultadoExpresion der = expresionSimple();
            if (tokenIzq.tipo == "IDENTIFICADOR") {
                tablaSimbolos.verificarAsignacion(tokenIzq.valor, der.tipo, tokenIzq.linea, tokenIzq.columna);
            }
            if (!izq.lugar.empty() && !der.lugar.empty()) {
                string comentarioAsig;
                if (der.lugar == "return_value") {
                    comentarioAsig = "Asigna el valor retornado por el metodo a " + izq.lugar;
                } else {
                    comentarioAsig = "Asigna el valor de " + der.lugar + " a " + izq.lugar;
                }
                generador.emitir(izq.lugar + " = " + der.lugar, comentarioAsig);
            }
            izq = {der.tipo, izq.lugar};
        } 
        else if (tokenOp.tipo == "OPERADOR_RELACIONAL") {
            ResultadoExpresion der = expresionSimple();
            bool izqNum = (izq.tipo == TipoDato::INT || izq.tipo == TipoDato::FLOAT);
            bool derNum = (der.tipo == TipoDato::INT || der.tipo == TipoDato::FLOAT);
            
            if (!((izqNum && derNum) || (izq.tipo == der.tipo && izq.tipo != TipoDato::DESCONOCIDO))) {
                reportarErrorSemantico("No se pueden comparar tipos incompatibles (" + tokenOp.valor + ").", 
                                       tokenOp.linea, tokenOp.columna);
            }
            string temp = generador.nuevoTemporal();
            string exprCond = izq.lugar + " " + tokenOp.valor + " " + der.lugar;
            generador.emitir(temp + " = " + exprCond, 
                             temp + " es una etiqueta temporal para la condicion " + exprCond);
            izq = {TipoDato::BOOLEAN, temp};
        } 
        else if (tokenOp.tipo == "OPERADOR_LOGICO") {
            ResultadoExpresion der = expresionSimple();
            if (izq.tipo != TipoDato::BOOLEAN || der.tipo != TipoDato::BOOLEAN) {
                reportarErrorSemantico("Operador logico '" + tokenOp.valor + "' requiere operandos booleanos.", 
                                       tokenOp.linea, tokenOp.columna);
            }
            string temp = generador.nuevoTemporal();
            string exprLog = izq.lugar + " " + tokenOp.valor + " " + der.lugar;
            generador.emitir(temp + " = " + exprLog,
                             temp + " es una etiqueta temporal para la operacion logica " + exprLog);
            izq = {TipoDato::BOOLEAN, temp};
        }
    }
    return izq;
}

ResultadoExpresion AnalizadorSintactico::expresionSimple() {
    ResultadoExpresion base = {TipoDato::DESCONOCIDO, ""};

    if (comprobar("SIMBOLO", "(")) {
        avanzar();
        base = expresion();
        if (!coincidir("SIMBOLO", ")")) {
            reportarError("Se esperaba ')' para cerrar la expresion.");
        }
    }
    else if (comprobar("IDENTIFICADOR")) {
        Token idToken = tokenActual();
        avanzar();

        if (comprobar("SIMBOLO", "(")) {
            base = llamadaFuncion(idToken.valor, "", idToken.linea, idToken.columna);
        } 
        else if (coincidir("SIMBOLO", ".")) {
            // Uso de miembro o método de objeto: p.celebrarCumpleaños()
            Simbolo* simObj = tablaSimbolos.buscarSimbolo(idToken.valor, idToken.linea, idToken.columna);
            Token miembroToken = tokenActual();
            if (!coincidir("IDENTIFICADOR")) {
                reportarError("Se esperaba el nombre de un metodo o atributo despues de '.'.");
                return {TipoDato::DESCONOCIDO, ""};
            }

            if (simObj && simObj->tipo != TipoDato::OBJETO) {
                reportarErrorSemantico("El identificador '" + idToken.valor + "' no es un objeto.", 
                                       idToken.linea, idToken.columna);
            }

            if (comprobar("SIMBOLO", "(")) {
                base = llamadaFuncion(miembroToken.valor, idToken.valor, miembroToken.linea, miembroToken.columna);
            } else {
                base = {TipoDato::INT, idToken.valor + "." + miembroToken.valor};
            }
        }
        else {
            Simbolo* sim = tablaSimbolos.buscarSimbolo(idToken.valor, idToken.linea, idToken.columna);
            if (sim) base.tipo = sim->tipo;
            base.lugar = idToken.valor;
        }
    } 
    else if (comprobar("ENTERO")) {
        Token t = avanzar();
        base = {TipoDato::INT, t.valor};
    } 
    else if (comprobar("DECIMAL")) {
        Token t = avanzar();
        base = {TipoDato::FLOAT, t.valor};
    } 
    else if (comprobar("CADENA")) {
        Token t = avanzar();
        base = {TipoDato::STRING, t.valor};
    }
    else if (comprobar("CARACTER")) {
        Token t = avanzar();
        base = {TipoDato::CHAR, t.valor};
    }
    else if (comprobar("PALABRA_CLAVE", "true") || comprobar("PALABRA_CLAVE", "false")) {
        Token t = avanzar();
        base = {TipoDato::BOOLEAN, t.valor};
    }
    else if (esTipoDato()) {
        Token tokenTipo = tokenActual();
        avanzar(); 
        
        if (comprobar("IDENTIFICADOR")) {
            Token tokenVar = tokenActual();
            avanzar();
            
            TipoDato nuevoTipo = tipoDatoDesdeString(tokenTipo.valor);
            tablaSimbolos.insertarSimbolo(tokenVar.valor, nuevoTipo, tokenVar.linea, tokenVar.columna);
            base = {nuevoTipo, tokenVar.valor};

            if (coincidir("OPERADOR_ASIGNACION", "=")) {
                ResultadoExpresion asignado = expresion();
                tablaSimbolos.verificarAsignacion(tokenVar.valor, asignado.tipo, tokenVar.linea, tokenVar.columna);
                generador.emitir(tokenVar.valor + " = " + asignado.lugar, "Inicializacion de " + tokenVar.valor);
            }
        }
        return base;
    } 
    else {
        reportarError("Expresion o componente sintactico invalido ('" + tokenActual().valor + "').");
        return {TipoDato::DESCONOCIDO, ""};
    }

    while (comprobar("OPERADOR_ARITMETICO")) {
        Token tokenOp = tokenActual();
        avanzar();

        if (tokenOp.valor == "/" && (tokenActual().valor == "0" || tokenActual().valor == "0.0")) {
            reportarErrorSemantico("Division por cero detectada.", tokenOp.linea, tokenOp.columna);
        }

        ResultadoExpresion der = {TipoDato::DESCONOCIDO, ""};
        
        if (comprobar("IDENTIFICADOR")) {
            Token idDer = tokenActual();
            avanzar();
            if (comprobar("SIMBOLO", "(")) {
                der = llamadaFuncion(idDer.valor, "", idDer.linea, idDer.columna);
            } else {
                Simbolo* sim = tablaSimbolos.buscarSimbolo(idDer.valor, idDer.linea, idDer.columna);
                if (sim) der.tipo = sim->tipo;
                der.lugar = idDer.valor;
            }
        } else if (comprobar("ENTERO")) {
            Token t = avanzar();
            der = {TipoDato::INT, t.valor};
        } else if (comprobar("DECIMAL")) {
            Token t = avanzar();
            der = {TipoDato::FLOAT, t.valor};
        } else if (comprobar("CADENA")) {
            Token t = avanzar();
            der = {TipoDato::STRING, t.valor};
        } else {
            reportarError("Se esperaba un numero o variable despues del operador aritmetico.");
        }

        bool izqNum = (base.tipo == TipoDato::INT || base.tipo == TipoDato::FLOAT);
        bool derNum = (der.tipo == TipoDato::INT || der.tipo == TipoDato::FLOAT);

        if (!izqNum || !derNum) {
            reportarErrorSemantico("No se puede aplicar el operador aritmetico '" + tokenOp.valor + 
                                   "' a tipos incompatibles.", tokenOp.linea, tokenOp.columna);
            base.tipo = TipoDato::DESCONOCIDO;
        } else {
            if (base.tipo == TipoDato::FLOAT || der.tipo == TipoDato::FLOAT) {
                base.tipo = TipoDato::FLOAT;
            }
        }

        string temp = generador.nuevoTemporal();
        string exprArit = base.lugar + " " + tokenOp.valor + " " + der.lugar;
        generador.emitir(temp + " = " + exprArit, 
                         temp + " es una etiqueta temporal para la operacion " + exprArit);
        base.lugar = temp;
    }

    if (comprobar("OPERADOR_INCREMENTO")) {
        Token tokenInc = avanzar();
        string op = (tokenInc.valor == "++") ? "+" : "-";
        string accion = (tokenInc.valor == "++") ? "Incrementa " : "Decrementa ";
        generador.emitir(base.lugar + " = " + base.lugar + " " + op + " 1", accion + base.lugar);
    }

    return base;
}

ResultadoExpresion AnalizadorSintactico::llamadaFuncion(const string& nombre, const string& objetoReceptor, int linea, int columna) {
    coincidir("SIMBOLO", "(");

    SimboloFuncion* funcion = tablaSimbolos.buscarFuncion(nombre, linea, columna);
    vector<ResultadoExpresion> args;

    if (!comprobar("SIMBOLO", ")")) {
        do {
            args.push_back(expresion());
        } while (coincidir("SIMBOLO", ","));
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras los argumentos del metodo.");
    }

    // Emitir temporales para el paso de parámetros
    for (size_t i = 0; i < args.size(); ++i) {
        string tempParam = generador.nuevoTemporal();
        generador.emitir(tempParam + " = " + args[i].lugar, 
                         tempParam + " es una etiqueta temporal para el parametro " + to_string(i + 1));
    }

    if (funcion == nullptr) {
        return {TipoDato::DESCONOCIDO, "return_value"};
    }

    string etiqDestino = funcion->etiqueta.empty() ? nombre : funcion->etiqueta;
    if (!objetoReceptor.empty()) {
        generador.emitir("call " + etiqDestino, 
                         "Llama al metodo " + nombre + " del objeto " + objetoReceptor);
    } else {
        generador.emitir("call " + etiqDestino, 
                         "Llama al metodo " + nombre + " definido en la etiqueta " + etiqDestino);
    }

    if (args.size() != funcion->tiposParametros.size()) {
        reportarErrorSemantico("Llamada al metodo '" + nombre + "' con " + 
                               to_string(args.size()) + " argumentos en lugar de " + 
                               to_string(funcion->tiposParametros.size()) + ".", linea, columna);
        return {funcion->tipoRetorno, "return_value"};
    }

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].tipo != funcion->tiposParametros[i] && args[i].tipo != TipoDato::DESCONOCIDO) {
            reportarErrorSemantico("Argumento " + to_string(i + 1) + " en llamada a '" + nombre + 
                                   "' es de tipo '" + tipoDatoToString(args[i].tipo) + 
                                   "', se esperaba '" + tipoDatoToString(funcion->tiposParametros[i]) + "'.", 
                                   linea, columna);
        }
    }

    return {funcion->tipoRetorno, "return_value"};
}
