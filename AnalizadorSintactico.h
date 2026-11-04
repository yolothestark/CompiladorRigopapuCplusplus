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
#include "GeneradorCodigoIntermedio.hpp"

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct ErrorSintactico {
    int linea;
    int columna;
    std::string tipo; 
    std::string mensaje;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct ResultadoExpresion {
    TipoDato tipo;
    std::string lugar;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
class AnalizadorSintactico {
private:
    std::vector<Token> tokens;
    size_t indiceActual;
    std::vector<ErrorSintactico> errores;

    TablaSimbolos tablaSimbolos;
    GeneradorCodigoIntermedio generador;

    bool dentroDeFuncion;
    TipoDato tipoRetornoFuncionActual;
    std::string nombreFuncionActual;
    std::string claseActual;
    int nivelBucles;
    int nivelSwitch;
    std::vector<std::string> pilaEtiquetasFinBucle;

    Token tokenActual() const;
    Token tokenAnterior() const;
    Token tokenSiguiente() const;
    bool estaAlFinal() const;
    Token avanzar();
    bool comprobar(const std::string& tipo, const std::string& valor = "") const;
    bool coincidir(const std::string& tipo, const std::string& valor = "");
    
    void reportarError(const std::string& mensaje);
    void reportarErrorSemantico(const std::string& mensaje, int linea, int columna);
    void sincronizar();

    void programa();
    void sentencia();
    void estructuraControl();
    void sentenciaIf();
    void sentenciaSwitch();
    void sentenciaWhile();
    void sentenciaFor();
    void sentenciaDoWhile();
    void sentenciaReturn();
    void sentenciaBreak();
    
    void bloque();
    
    ResultadoExpresion expresion();
    ResultadoExpresion expresionSimple();
    ResultadoExpresion llamadaFuncion(const std::string& nombre, const std::string& objetoReceptor, int linea, int columna);
    
    void inicializacion();
    void actualizacion();

    void declaracionClase();
    void cuerpoClase();
    void declaracionMetodoOVariable();
    void declaracionObjetoOExpresion();
    void declaracionMetodo(TipoDato tipoRetorno, const std::string& nombre, int linea, int columna);
    void parametros(std::vector<std::pair<std::string, TipoDato>>& listaParams);
    void parametro(std::vector<std::pair<std::string, TipoDato>>& listaParams);
    
    bool esTipoDato() const;
    TipoDato tipoDatoDesdeString(const std::string& str) const;

public:
    AnalizadorSintactico();
    void analizar(const std::vector<Token>& tokensEntrada);
    const std::vector<ErrorSintactico>& obtenerErrores() const;
    const GeneradorCodigoIntermedio& obtenerGenerador() const;
    bool tieneErrores() const;
    void imprimirErrores() const;
    void imprimirCodigoIntermedio() const;
};
