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
#include <windows.h>

using namespace std;

void imprimirEncabezadoIntegrantes() {
    cout << "==========================================================" << endl;
    cout << "Integrantes del equipo:" << endl;
    cout << "1. Anguiano Garcia Angel Yahir Guadalupe" << endl;
    cout << "2. Figueroa Robles Axel Israel" << endl;
    cout << "3. Molina Alvarado Alvaro Moises" << endl;
    cout << "4. Torres Martinez Miguel Angel" << endl;
    cout << "==========================================================" << endl;
    cout << " PRACTICA: ANALISIS SEMANTICO (CONTROL Y METODOS)" << endl;
    cout << "==========================================================" << endl << endl;
}

void probarCodigo(const string& titulo, const string& codigoFuente) {
    cout << "**********************************************************" << endl;
    cout << "  " << titulo << endl;
    cout << "**********************************************************" << endl;

    AnalizadorLexico lexico;
    vector<Token> tokens = lexico.analizar(codigoFuente);

    if (lexico.tieneErrores()) {
        cout << "[!] Se detectaron errores léxicos previos." << endl;
    } else {
        cout << "[+] Análisis léxico completado sin errores (" << tokens.size() << " tokens)." << endl;
    }

    AnalizadorSintactico sintactico;
    sintactico.analizar(tokens);

    cout << "\n----------------------------------------------------------" << endl;
    cout << "           REPORTE SINTACTICO Y SEMANTICO                 " << endl;
    cout << "----------------------------------------------------------" << endl;

    if (sintactico.tieneErrores()) {
        const auto& errores = sintactico.obtenerErrores();
        cout << "Se detectaron " << errores.size() << " error(es):\n" << endl;
        for (const auto& err : errores) {
            cout << ">> [" << err.tipo << "] " << err.mensaje 
                 << " [Linea " << err.linea << ", Columna " << err.columna << "]" << endl;
        }
    } else {
        cout << ">>> [EXITO]: Codigo valido. No se detectaron errores sintacticos ni semanticos." << endl;
    }
    cout << "\n\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    imprimirEncabezadoIntegrantes();

    // =========================================================================
    // CASO 1: CODIGO VALIDO
    // Debe compilar con 0 errores (cumple flujo, tipos y firmas de métodos).
    // =========================================================================
    string casoValido = R"(
        class ModuloValido {
            int sumar(int a, int b) {
                return a + b;
            }

            boolean esMayor(int edad) {
                return edad >= 18;
            }

            void ejecutar() {
                int x = 15;
                int res = sumar(5, 3);
                boolean autorizado = esMayor(20);

                if (x > 10) {
                    x = x + 1;
                } else {
                    x = 0;
                }

                while (x < 20) {
                    x = x + 1;
                }
            }
        }
    )";

    // =========================================================================
    // CASO 2: CODIGO INVALIDO (PRUEBA DE ERRORES EXIGIDOS POR LA PRACTICA)
    // 1. Condición de control no booleana (if con texto/entero).
    // 2. Estructura de control sin llave de apertura o cierre.
    // 3. Return con tipo de dato incompatible al declarado.
    // 4. Llamada a método con tipo de argumento incorrecto.
    // 5. Llamada a método con cantidad incorrecta de argumentos.
    // =========================================================================
    string casoInvalido = R"(
        class ModuloErrores {
            int obtenerEdad() {
                return "Veinte"; 
            }

            int multiplicar(int a, int b) {
                return a * b;
            }

            void probarErrores() {
                // 1. Condicion no booleana en estructura de control
                if ("texto") {
                    int a = 1;
                }

                // 2. Llamada con tipo de argumento incorrecto
                int prod = multiplicar(10, "invalido");

                // 3. Llamada con cantidad de argumentos incorrecta
                int prod2 = multiplicar(5, 4, 3);

                // 4. Condicion no booleana en bucle while
                while (100) {
                    int cont = 0;
                }
            }
        }
    )";

    probarCodigo("PRUEBA 1: CASO DE EJECUCION VALIDO", casoValido);
    probarCodigo("PRUEBA 2: CASO DE EJECUCION CON ERRORES SEMANTICOS", casoInvalido);

    return 0;
}