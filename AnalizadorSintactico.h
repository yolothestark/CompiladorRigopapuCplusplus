// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#pragma once

#include <string>
#include <vector>
#include "AnalizadorLexico.h"
#include "TablaSimbolos.hpp"

using namespace std;

struct ErrorSintactico {
    int linea;
    int columna;
    string tipo; 
    string mensaje;
};

class AnalizadorSintactico {
private:
    vector<Token> tokens;
    size_t indiceActual;
    vector<ErrorSintactico> errores;

    TablaSimbolos tablaSimbolos;

    bool dentroDeFuncion;
    TipoDato tipoRetornoFuncionActual;
    string nombreFuncionActual;
    int nivelBucles;

    Token tokenActual() const;
    Token tokenAnterior() const;
    bool estaAlFinal() const;
    Token avanzar();
    bool comprobar(const string& tipo, const string& valor = "") const;
    bool coincidir(const string& tipo, const string& valor = "");
    
    void reportarError(const string& mensaje);
    void reportarErrorSemantico(const string& mensaje, int linea, int columna);
    void sincronizar();

    void programa();
    void sentencia();
    void estructuraControl();
    void sentenciaIf();
    void sentenciaWhile();
    void sentenciaFor();
    void sentenciaReturn();
    
    void bloque();
    
    TipoDato expresion();
    TipoDato expresionSimple();
    TipoDato termino();
    TipoDato factor();
    TipoDato llamadaFuncion(const string& nombre, int linea, int columna);
    
    void inicializacion();
    void actualizacion();

    void declaracionClase();
    void cuerpoClase();
    void declaracionMetodoOVariable();
    void declaracionMetodo(TipoDato tipoRetorno, const string& nombre, int linea, int columna);
    void parametros(vector<pair<string, TipoDato>>& listaParams);
    void parametro(vector<pair<string, TipoDato>>& listaParams);
    
    bool esTipoDato() const;
    TipoDato tipoDatoDesdeString(const string& str) const;

public:
    AnalizadorSintactico();
    void analizar(const vector<Token>& tokensEntrada);
    const vector<ErrorSintactico>& obtenerErrores() const;
    bool tieneErrores() const;
    void imprimirErrores() const;
};