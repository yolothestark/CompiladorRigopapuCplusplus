// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#include <iostream>
#include <string>
#include <iomanip>
#include "AnalizadorLexico.h"
#include "AnalizadorSintactico.h"

using namespace std;

int main()
{
    cout << "==========================================================" << endl;
    cout << "Integrantes del equipo:" << endl;
    cout << "1. Anguiano Garcia Angel Yahir Guadalupe" << endl;
    cout << "2. Figueroa Robles Axel Israel" << endl;
    cout << "3. Torres Martinez Miguel Angel" << endl;
    cout << "4. Molina Alvarado Alvaro Moises" << endl;
    cout << "==========================================================" << endl;

    cout << "\n==========================================================" << endl;
    cout << "  PRACTICA 4: SINTACTICO DE EXPRESIONES ARITMETICAS/LOGICAS " << endl;
    cout << "==========================================================" << endl << endl;

    // Código de prueba extendido con MÁS ERRORES SINTÁCTICOS
    string codigoPrueba = R"(
    // --- EXPRESIONES VÁLIDAS ---
    x + y * 5;
    (a - b) / c;
    10 > 5 && 3 <= 4;
    (x + y) * (z - w);

    // --- LISTA EXTENDIDA DE ERRORES SINTÁCTICOS ---
    x + * y;              // Error 1: Operador duplicado
    10 >;                 // Error 2: Comparacion sin expresion a la derecha
    (a - b;               // Error 3: Parentesis sin cerrar
    5 / (2 + );           // Error 4: Expresion incompleta dentro de parentesis
    a + b -;              // Error 5: Expresion termina en operador
    x &&;                 // Error 6: Operador logico sin termino derecho
    ((x + 4) * 2;         // Error 7: Falta un parentesis de cierre
    * 5 + 3;              // Error 8: Inicia con un operador de multiplicacion
)";

    // 1. ANÁLISIS LÉXICO
    AnalizadorLexico lexer;
    vector<Token> tokens = lexer.analizar(codigoPrueba);

    cout << left << setw(20) << "TOKEN" << setw(20) << "VALOR" << setw(10) << "LINEA" << "COLUMNA" << endl;
    cout << "----------------------------------------------------------" << endl;

    for (const Token& tok : tokens) {
        cout << left << setw(20) << tok.tipo 
             << setw(20) << tok.valor 
             << setw(10) << tok.linea 
             << tok.columna << endl;
    }

    cout << "==========================================================" << endl;
    cout << "Total de tokens validos: " << tokens.size() << endl;
    cout << "==========================================================" << endl << endl;

    if (lexer.tieneErrores()) {
        cout << "==========================================================" << endl;
        cout << "               REPORTE DE ERRORES LEXICOS                 " << endl;
        cout << "==========================================================" << endl;
        for (const auto& err : lexer.obtenerErrores()) {
            cout << ">> Error Lexico en [Linea: " << err.linea << ", Columna: " << err.columna << "]" << endl;
            cout << "   - Problema: " << err.mensaje << endl;
            cout << "   - Valor leido: " << err.valor << endl;
            cout << "----------------------------------------------------------" << endl;
        }
    }

    // 2. ANÁLISIS SINTÁCTICO DE EXPRESIONES
    AnalizadorSintactico parser;
    parser.analizar(tokens);

    cout << "\n==========================================================" << endl;
    cout << "             REPORTE DE ERRORES SINTACTICOS               " << endl;
    cout << "==========================================================" << endl;

    if (parser.tieneErrores()) {
        const auto& erroresSint = parser.obtenerErrores();
        cout << "Se encontraron " << erroresSint.size() << " error(es) sintactico(s):\n" << endl;
        for (const auto& err : erroresSint) {
            cout << ">> Error Sintactico en [Linea: " << err.linea << ", Columna: " << err.columna << "]" << endl;
            cout << "   - Problema: " << err.mensaje << endl;
            cout << "   - Token leido: '" << err.tokenEncontrado << "'" << endl;
            cout << "----------------------------------------------------------" << endl;
        }
    } else {
        cout << "¡Analisis Sintactico Exitoso! Todas las expresiones son validas." << endl;
    }

    cout << "\n==========================================================" << endl;
    cout << "Fin de la ejecucion." << endl;
    cout << "==========================================================" << endl;

    return 0;
}