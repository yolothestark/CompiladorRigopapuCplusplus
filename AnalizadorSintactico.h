// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#pragma once

#include <string>
#include <vector>
#include "AnalizadorLexico.h"

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

    // Métodos auxiliares de recorrido
    Token tokenActual() const;
    Token tokenAnterior() const;
    bool estaAlFinal() const;
    Token avanzar();
    bool comprobar(const string& tipo, const string& valor = "") const;
    bool coincidir(const string& tipo, const string& valor = "");
    void reportarError(const string& mensaje);

    // Sincronización para recuperación tras un error sintáctico
    void sincronizar();

    // Reglas gramaticales (Estructuras de Control)
    void programa();
    void sentencia();
    void estructuraControl();
    void sentenciaIf();
    void sentenciaWhile();
    void sentenciaFor();
    
    // Reglas para componentes
    void bloque();
    void expresion();
    void expresionSimple();
    void inicializacion();
    void actualizacion();

public:
    void analizar(const vector<Token>& tokensEntrada);
    const vector<ErrorSintactico>& obtenerErrores() const;
    bool tieneErrores() const;
};