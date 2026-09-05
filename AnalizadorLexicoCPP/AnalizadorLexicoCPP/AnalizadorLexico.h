#pragma once

#include <string>
#include <vector>

using namespace std;

struct Token {
    string tipo;
    string valor;
};

class AnalizadorLexico {
private:
    vector<Token> tokens;

public:
    vector<Token> analizar(const string& codigo);
};