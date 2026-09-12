// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#pragma once

#include <string>
#include <vector>

struct Token {
    std::string tipo;
    std::string valor;
    int linea;
    int columna;
};

struct ErrorLexico {
    int linea;
    int columna;
    std::string tipo;
    std::string mensaje;
    std::string valor;
};

class AnalizadorLexico {
private:
    std::vector<Token> tokens;
    std::vector<ErrorLexico> errores;

public:
    std::vector<Token> analizar(const std::string& codigo);
    const std::vector<ErrorLexico>& obtenerErrores() const;
    bool tieneErrores() const;
};