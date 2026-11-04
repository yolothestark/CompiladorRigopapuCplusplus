// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#pragma once

#include <string>
#include <vector>

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct Token {
    std::string tipo;
    std::string valor;
    int linea;
    int columna;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct ErrorLexico {
    int linea;
    int columna;
    std::string tipo;
    std::string mensaje;
    std::string valor;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
class AnalizadorLexico {
private:
    std::vector<Token> tokens;
    std::vector<ErrorLexico> errores;

public:
    std::vector<Token> analizar(const std::string& codigo);
    const std::vector<ErrorLexico>& obtenerErrores() const;
    bool tieneErrores() const;
};
