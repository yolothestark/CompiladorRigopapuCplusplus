// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#include <iostream>
#include <string>
#include <vector>
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

int main() {
    imprimirEncabezadoIntegrantes();

    // Casos de prueba requeridos por la practica (Clases y Metodos)
    string codigoPrueba = R"(
        // --- CASOS VALIDOS ---
        class MiClase { 
            int miMetodo(int a, float b) { 
                // cuerpo del metodo 
                x = x + 1;
            } 
            void otroMetodo() { 
                // cuerpo del metodo 
                y = 0;
            }
        }

        // --- CASOS INVALIDOS (SINTAXIS ERRONEA) ---
        class MiClaseInvalida { 
            int miMetodo(int a float b) { // Falta coma entre parametros 
                x = x + 1;
            } 
            void otroMetodo { // Faltan parentesis 
                y = 0;
            }
        }
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
            cout << "Error: " << err.mensaje << " en la linea " << err.linea << ", columna " << err.columna << "." << endl;
        }
    } else {
        cout << "¡Analisis sintactico exitoso! No se encontraron errores." << endl;
    }

    imprimirEncabezadoIntegrantes();

    return 0;
}