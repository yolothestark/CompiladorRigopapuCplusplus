#include <iostream>
#include <string>
#include <iomanip>
#include "AnalizadorLexico.h"

using namespace std;

int main()
{
    string codigo = R"(
    // Prueba de comentario de una linea

    /*
       Prueba de comentario
       de varias lineas
    */

    int edad = 22;
    float promedio = 95.5;
    double precio = 1250.75;

    string nombre = "Angel\nTorres";
    char letra = 'A';

    if (edad >= 18 && promedio >= 60.0) {
        cout << "Alumno aprobado" << endl;
        edad++;
    }
    else {
        cout << "Alumno no aprobado" << endl;
        edad--;
    }

    for (int i = 0; i < 10; i++) {
        precio += 10.50;
    }

    bool activo = true;
    bool terminado = false;

    return 0;
)";

    AnalizadorLexico analizador;

    vector<Token> tokens = analizador.analizar(codigo);

    cout << "========================================" << endl;
    cout << "       ANALIZADOR LEXICO DE C++         " << endl;
    cout << "========================================" << endl;
    cout << endl;

    cout << left;
    cout << setw(25) << "TOKEN" << "VALOR" << endl;
    cout << "---------------------------------------------" << endl;

    for (const Token& token : tokens)
    {
        cout << setw(25) << token.tipo << token.valor << endl;
        cout << "---------------------------------------------" << endl;
    }

    cout << endl;
    cout << "========================================" << endl;
    cout << "Total de tokens: " << tokens.size() << endl;
    cout << "========================================" << endl;

    return 0;
}