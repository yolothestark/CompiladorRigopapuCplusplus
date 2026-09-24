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

void imprimirEncabezadoIntegrantes() {
    cout << "==========================================================" << endl;
    cout << "Integrantes del equipo:" << endl;
    cout << "1. Anguiano Garcia Angel Yahir Guadalupe" << endl;
    cout << "2. Figueroa Robles Axel Israel" << endl;
    cout << "3. Torres Martinez Miguel Angel" << endl;
    cout << "4. Molina Alvarado Alvaro Moises" << endl;
    cout << "==========================================================" << endl << endl;
}

int main()
{
    // Requisito: Nombre de integrantes al inicio del main
    imprimirEncabezadoIntegrantes();

    // Casos de prueba requeridos por la Actividad 5
    string codigoPrueba = R"(
        // --- CASOS VALIDOS ---
        if (x > 0) { x = x + 1; } else { x = x - 1; }
        while (y < 10) { y = y + 1; }
        for (i = 0; i < 10; i = i + 1) { suma = suma + i; }

        // --- CASOS INVALIDOS (SINTAXIS ERRONEA) ---
        if (x > 0 { x = x + 1; }
        while x < 10) { y = y + 1; }
        for i = 0; i < 10; i = i + 1) { suma = suma + i; }
    )";

    // 1. Fase de Análisis Léxico
    AnalizadorLexico lexico;
    vector<Token> tokens = lexico.analizar(codigoPrueba);

    cout << "==========================================================" << endl;
    cout << "                 TOKENS GENERADOS (LEXICO)                " << endl;
    cout << "==========================================================" << endl;
    cout << left << setw(22) << "TIPO" << setw(18) << "VALOR" << setw(10) << "LINEA" << "COLUMNA" << endl;
    cout << "----------------------------------------------------------" << endl;

    for (const Token& t : tokens) {
        cout << left << setw(22) << t.tipo 
             << setw(18) << t.valor 
             << setw(10) << t.linea 
             << t.columna << endl;
    }

    if (lexico.tieneErrores()) {
        cout << "\n[!] Se detectaron errores lexico(s) previos." << endl;
    } else {
        cout << "\n[+] Analisis lexico completado sin errores." << endl;
    }

    // 2. Fase de Análisis Sintáctico
    AnalizadorSintactico sintactico;
    sintactico.analizar(tokens);

    cout << "\n==========================================================" << endl;
    cout << "           REPORTE DE ERRORES SINTACTICOS                 " << endl;
    cout << "==========================================================" << endl;

    if (sintactico.tieneErrores()) {
        const auto& errores = sintactico.obtenerErrores();
        cout << "Se encontraron " << errores.size() << " error(es) sintactico(s):\n" << endl;
        
        for (const auto& err : errores) {
            cout << ">> " << err.tipo << " en [Linea: " << err.linea << ", Columna: " << err.columna << "]" << endl;
            cout << "   - Detalle: " << err.mensaje << endl;
            cout << "----------------------------------------------------------" << endl;
        }
    } else {
        cout << "¡Analisis sintactico exitoso! No se encontraron errores." << endl;
    }

    // Requisito: Nombre de integrantes al final del main
    cout << "\n==========================================================" << endl;
    cout << "Fin de ejecucion - Integrantes del equipo:" << endl;
    cout << "1. Anguiano Garcia Angel Yahir Guadalupe" << endl;
    cout << "2. Figueroa Robles Axel Israel" << endl;
    cout << "3. Torres Martinez Miguel Angel" << endl;
    cout << "4. Molina Alvarado Alvaro Moises" << endl;
    cout << "==========================================================" << endl;

    return 0;
}