// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#pragma once

#include <string>
#include <vector>

using namespace std;

struct Token {
    string tipo;
    string valor;
    int linea;
    int columna;
};

struct ErrorLexico {
    int linea;
    int columna;
    string tipo;
    string mensaje;
    string valor;
};

class AnalizadorLexico {
private:
    vector<Token> tokens;
    vector<ErrorLexico> errores;

public:
    vector<Token> analizar(const string& codigo);
    const vector<ErrorLexico>& obtenerErrores() const;
    bool tieneErrores() const;
};