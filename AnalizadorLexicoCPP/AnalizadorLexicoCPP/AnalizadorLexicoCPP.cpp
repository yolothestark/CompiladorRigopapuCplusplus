// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#include <iostream>
#include <string>
#include <iomanip>
#include "AnalizadorLexico.h"

using namespace std;

int main()
{
    cout << "==========================================================" << endl;
    cout << "Integrantes del equipo:" << endl;
    cout << "1. Anguiano Garcia Angel Yahir Guadalupe" << endl;
    cout << "2. Figueroa Robles Axel Israel" << endl;
    cout << "3. Torres Martinez Miguel Angel" << endl;
    cout << "4. Molina Alvarado Alvaro Moises" << endl;
    cout << "==========================================================" << endl << endl;

    // Código de prueba que incluye tokens válidos y errores léxicos
    string codigo = R"(
    // Prueba de tokens válidos
    int edad = 22;
    float promedio = 95.5;

    // Pruebas de errores léxicos
    @ // Error: Caracter no reconocido
    int 123invalido = 5; // Error: Identificador mal formado
    float errorNum = 12.34.56; // Error: Literal numérico incorrecto (múltiples puntos)
    string saludo = "Hola mundo sin cerrar
    
    // Continúa con tokens válidos
    char letra = 'A';
)";

    AnalizadorLexico analizador;

    vector<Token> tokens = analizador.analizar(codigo);

    cout << "==========================================================" << endl;
    cout << "                ANALIZADOR LEXICO DE C++                  " << endl;
    cout << "==========================================================" << endl;
    cout << endl;

    cout << left;
    cout << setw(25) << "TOKEN" << setw(20) << "VALOR" << setw(10) << "LINEA" << "COLUMNA" << endl;
    cout << "----------------------------------------------------------" << endl;

    for (const Token& token : tokens)
    {
        cout << setw(25) << token.tipo 
             << setw(20) << token.valor 
             << setw(10) << token.linea 
             << token.columna << endl;
        cout << "----------------------------------------------------------" << endl;
    }

    cout << endl;
    cout << "==========================================================" << endl;
    cout << "Total de tokens válidos: " << tokens.size() << endl;
    cout << "==========================================================" << endl;
    cout << endl;

    // Reporte de errores
    if (analizador.tieneErrores()) {
        const auto& errores = analizador.obtenerErrores();
        cout << "\n==========================================================" << endl;
        cout << "              REPORTE DE ERRORES LEXICOS                  " << endl;
        cout << "==========================================================" << endl;
        cout << "Se encontraron " << errores.size() << " error(es) lexico(s):\n" << endl;
        
        for (const auto& err : errores) {
            cout << ">> " << err.tipo << " en [Linea: " << err.linea << ", Columna: " << err.columna << "]" << endl;
            cout << "   - Problema: " << err.mensaje << endl;
            cout << "   - Valor leido: " << err.valor << endl;
            cout << "----------------------------------------------------------" << endl;
        }
    } else {
        cout << "\nNo se encontraron errores lexicos. ¡Analisis exitoso!" << endl;
    }

    cout << "\n==========================================================" << endl;
    cout << "Fin de ejecucion - Integrantes del equipo:" << endl;
    cout << "1. Anguiano Garcia Angel Yahir Guadalupe" << endl;
    cout << "2. Figueroa Robles Axel Israel" << endl;
    cout << "3. Torres Martinez Miguel Angel" << endl;
    cout << "4. Molina Alvarado Alvaro Moises" << endl;
    cout << "==========================================================" << endl;

    return 0;
}