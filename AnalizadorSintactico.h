// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#pragma once

#include "AnalizadorLexico.h"
#include <string>
#include <vector>

struct ErrorSintactico {
    int linea;
    int columna;
    std::string mensaje;
    std::string tokenEncontrado;
};

class AnalizadorSintactico {
private:
    std::vector<Token> tokens;
    size_t posicionActual;
    std::vector<ErrorSintactico> errores;

    Token tokenActual() const;
    Token tokenAnterior() const;
    bool estaAlFinal() const;
    bool comprobar(const std::string& tipo) const;
    bool comprobarValor(const std::string& valor) const;
    Token avanzar();

    void registrarError(const std::string& mensaje, const Token& tok);
    void sincronizar();

    // Reglas gramaticales para Expresiones Aritméticas y Lógicas
    void programa();
    void expresionStmt();
    void expresion();
    void expresionLogica();
    void expresionRelacional();
    void expresionAritmetica();
    void terminoAritmetico();
    void factorAritmetico();

public:
    AnalizadorSintactico();
    bool analizar(const std::vector<Token>& tokensEntrada);
    const std::vector<ErrorSintactico>& obtenerErrores() const;
    bool tieneErrores() const;
};