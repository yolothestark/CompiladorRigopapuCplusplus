// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

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
    cout << "3. Molina Alvarado Alvaro Moises" << endl;
    cout << "4. Torres Martinez Miguel Angel" << endl;
    cout << "==========================================================" << endl;
    cout << "  PRACTICA: VALIDACION DE TIPOS EN EXPRESIONES (SEMANTICO)" << endl;
    cout << "==========================================================" << endl << endl;
}

int main() {
    imprimirEncabezadoIntegrantes();

    string codigoPrueba = R"(
        // ==========================================
        // 1. CASOS VALIDOS (Tipos y Expresiones)
        // ==========================================
        int edad = 25;
        float promedio = 8.5;
        boolean activo = true;
        string nombre = "Juan";

        int suma = 5 + 3;
        float calculo = (10.0 + 20.0) * (30.0 - 15.0);
        boolean testLogico = (edad > 18) && activo;
        float conversionImplicita = edad; // Valido: int a float

        // ==========================================
        // 2. CASOS INVALIDOS (Errores Semánticos)
        // ==========================================
        int errSuma = edad + nombre;          // Error: Sumar entero con cadena
        int errDiv = 10 / 0;                  // Error: Division por cero
        boolean errLogico = (edad > 18) && 1; // Error: Operador logico con un numero
        boolean errComp = (nombre == 5);      // Error: Comparacion entre string y numero
        int errTipo = "Hola";                 // Error: Asignacion de string a int

        // ==========================================
        // 3. SOPORTE A PRACTICAS ANTERIORES
        // ==========================================
        class MiClase { 
            int miMetodo(int a, float b) { 
                int x = 0;
                x = x + 1;
            } 
        }
    )";

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

    AnalizadorSintactico sintactico;
    sintactico.analizar(tokens);

    cout << "\n==========================================================" << endl;
    cout << "      REPORTE DE ERRORES SINTACTICOS Y SEMANTICOS         " << endl;
    cout << "==========================================================" << endl;

    if (sintactico.tieneErrores()) {
        const auto& errores = sintactico.obtenerErrores();
        cout << "Se encontraron " << errores.size() << " error(es):\n" << endl;
        
        for (const auto& err : errores) {
            cout << ">> " << err.mensaje << " [Linea " << err.linea << ", Columna " << err.columna << "]" << endl;
        }
    } else {
        cout << "¡Analisis exitoso! No se encontraron errores." << endl;
    }

    cout << "\n";
    imprimirEncabezadoIntegrantes();

    return 0;
}