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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;

void imprimirIntegrantes(const string& momento) {
    cout << "======================================================================" << endl;
    cout << "  INTEGRANTES DEL EQUIPO (" << momento << " DE EJECUCION):" << endl;
    cout << "  1. Anguiano Garcia Angel Yahir Guadalupe" << endl;
    cout << "  2. Figueroa Robles Axel Israel" << endl;
    cout << "  3. Molina Alvarado Alvaro Moises" << endl;
    cout << "  4. Torres Martinez Miguel Angel" << endl;
    cout << "======================================================================" << endl;
}

void probarCodigo(const string& titulo, const string& codigoFuente) {
    cout << "\n**********************************************************************" << endl;
    cout << "  " << titulo << endl;
    cout << "**********************************************************************" << endl;

    cout << "\n[1] CODIGO FUENTE DE ENTRADA:" << endl;
    cout << "----------------------------------------------------------------------" << endl;
    cout << codigoFuente << endl;
    cout << "----------------------------------------------------------------------" << endl;

    AnalizadorLexico lexico;
    vector<Token> tokens = lexico.analizar(codigoFuente);

    cout << "\n[2] FASE LEXICA:" << endl;
    if (lexico.tieneErrores()) {
        cout << "  [!] Se detectaron errores lexicos:" << endl;
        for (const auto& err : lexico.obtenerErrores()) {
            cout << "      - Linea " << err.linea << ", Col " << err.columna 
                 << ": " << err.mensaje << " ('" << err.valor << "')" << endl;
        }
    } else {
        cout << "  [+] Analisis lexico completado sin errores (" << tokens.size() << " tokens generados)." << endl;
    }

    AnalizadorSintactico sintactico;
    sintactico.analizar(tokens);

    cout << "\n[3] FASE SINTACTICA Y SEMANTICA:" << endl;
    if (sintactico.tieneErrores()) {
        const auto& errores = sintactico.obtenerErrores();
        cout << "  [!] Se detectaron " << errores.size() << " error(es):\n" << endl;
        for (const auto& err : errores) {
            cout << "      >> [" << err.tipo << "] Linea " << err.linea 
                 << ", Columna " << err.columna << ": " << err.mensaje << endl;
        }
        cout << "\n[4] GENERACION DE CODIGO INTERMEDIO (TRES DIRECCIONES):" << endl;
        cout << "----------------------------------------------------------------------" << endl;
        cout << "  [X] Generacion abortada: Se requiere corregir los errores previos." << endl;
        cout << "----------------------------------------------------------------------\n" << endl;
    } else {
        cout << "  [+] Validacion sintactica y semantica exitosa (0 errores)." << endl;
        cout << "\n[4] CODIGO INTERMEDIO GENERADO (REPRESENTACION DE 3 DIRECCIONES):" << endl;
        cout << "----------------------------------------------------------------------" << endl;
        sintactico.imprimirCodigoIntermedio();
        cout << "----------------------------------------------------------------------\n" << endl;
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    // PRIMERA INSTRUCCIÓN DE IMPRESIÓN CON LOS NOMBRES COMPLETOS EN ORDEN ALFABÉTICO
    imprimirIntegrantes("INICIO");

    cout << "\n======================================================================" << endl;
    cout << "  ACTIVIDAD 11.- CODIGO INTERMEDIO PARA CONTROL DE FLUJO" << endl;
    cout << "======================================================================" << endl;

    // =========================================================================
    // PRUEBA 1 (VÁLIDA): ESTRUCTURAS CONDICIONALES (if-else y switch)
    // =========================================================================
    string pruebaCondicionales = R"(
        int x = 10;
        int y = 5;
        int z = 0;

        if (x > y) {
            z = x;
        } else {
            z = y;
        }

        int n = 1;
        string result = "";

        switch (n) {
            case 1:
                result = "Uno";
                break;
            case 2:
                result = "Dos";
                break;
            default:
                result = "Otro";
        }
    )";

    // =========================================================================
    // PRUEBA 2 (VÁLIDA): ESTRUCTURAS DE BUCLES (while, for y do-while)
    // =========================================================================
    string pruebaBucles = R"(
        int i = 0;
        while (i < 10) {
            i = i + 1;
        }

        int sum = 0;
        for (int k = 0; k < 5; k++) {
            sum = sum + k;
        }

        int count = 0;
        do {
            count = count + 1;
        } while (count < 10);
    )";

    // =========================================================================
    // PRUEBA 3 (VÁLIDA): DEFINICIÓN Y LLAMADA DE MÉTODOS + CLASES Y OBJETOS
    // =========================================================================
    string pruebaMetodosYClases = R"(
        int suma(int a, int b) {
            return a + b;
        }

        int result = 0;
        result = suma(5, 3);

        class Persona {
            int edad;
            void celebrarCumpleaños() {
                edad = edad + 1;
            }
        }

        Persona p = new Persona();
        p.celebrarCumpleaños();
    )";

    // =========================================================================
    // PRUEBA 4 (INVÁLIDA): PRUEBA DE REGLAS INVÁLIDAS (ERRORES EN CADA REGLA)
    // =========================================================================
    string pruebaInvalida = R"(
        class ModuloErrores {
            int obtenerEdad() {
                return "Veinte";
            }

            int multiplicar(int a, int b) {
                return a * b;
            }

            void probarReglasInvalidas() {
                int opcion = 1;

                // 1. Condicion no booleana en if
                if ("texto_invalido") {
                    opcion = 2;
                }

                // 2. Switch con case de tipo incompatible y case duplicado
                switch (opcion) {
                    case "uno":
                        opcion = 10;
                        break;
                    case 1:
                        opcion = 20;
                        break;
                    case 1:
                        opcion = 30;
                        break;
                }

                // 3. Break fuera de bucle o switch
                break;

                // 4. Condicion no booleana en while
                while (100) {
                    opcion = opcion + 1;
                }

                // 5. Condicion no booleana en for
                for (int i = 0; "no_bool"; i++) {
                    opcion = opcion + 1;
                }

                // 6. Condicion no booleana en do-while
                do {
                    opcion = opcion + 1;
                } while (50);

                // 7. Llamada a metodo con tipo y numero de argumentos incorrectos
                int prod1 = multiplicar(10, "invalido");
                int prod2 = multiplicar(5, 4, 3);

                // 8. Instanciacion de clase no declarada
                 Vehiculo v = new Vehiculo();
            }
        }
    )";

    probarCodigo("PRUEBA 1 (VALIDA): CONDICIONALES (if-else y switch)", pruebaCondicionales);
    probarCodigo("PRUEBA 2 (VALIDA): BUCLES (while, for y do-while)", pruebaBucles);
    probarCodigo("PRUEBA 3 (VALIDA): METODOS, CLASES Y OBJETOS", pruebaMetodosYClases);
    probarCodigo("PRUEBA 4 (INVALIDA): DETECCION DE ERRORES EN TODAS LAS REGLAS", pruebaInvalida);

    // ÚLTIMA INSTRUCCIÓN DE IMPRESIÓN CON LOS NOMBRES COMPLETOS EN ORDEN ALFABÉTICO
    imprimirIntegrantes("FINAL");

    return 0;
}
