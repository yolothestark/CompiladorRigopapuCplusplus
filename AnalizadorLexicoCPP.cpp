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
#include "GeneradorCodigoIntermedio.hpp"
#include "OptimizadorCodigo.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;

// Ejecuta una prueba directa de optimización sobre tuplas de Código Intermedio (TAC)
void probarReglaTAC(const string& titulo, const vector<InstruccionTAC>& tuplasEntrada) {
    cout << "\n================================================================================" << endl;
    cout << "  " << titulo << endl;
    cout << "================================================================================" << endl;

    OptimizadorCodigo optimizador;
    optimizador.optimizar(tuplasEntrada);
    optimizador.imprimirComparativa();
}

// Ejecuta el flujo completo del compilador: Léxico -> Sintáctico/Semántico -> TAC -> Optimizador
void probarFlujoCompletoCompilador(const string& titulo, const string& codigoFuente) {
    cout << "\n================================================================================" << endl;
    cout << "  " << titulo << endl;
    cout << "================================================================================" << endl;

    cout << "\n[1] CODIGO FUENTE DE ENTRADA:" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << codigoFuente << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    AnalizadorLexico lexico;
    vector<Token> tokens = lexico.analizar(codigoFuente);

    cout << "\n[2] FASE LEXICA:" << endl;
    if (lexico.tieneErrores()) {
        for (const auto& err : lexico.obtenerErrores()) {
            cout << "  [!] Error Lexico (Linea " << err.linea << ", Col " << err.columna
                 << "): " << err.mensaje << " ('" << err.valor << "')" << endl;
        }
        return;
    }
    cout << "  [+] Analisis lexico completado sin errores (" << tokens.size() << " tokens generados)." << endl;

    AnalizadorSintactico sintactico;
    sintactico.analizar(tokens);

    cout << "\n[3] FASE SINTACTICA Y SEMANTICA:" << endl;
    if (sintactico.tieneErrores()) {
        for (const auto& err : sintactico.obtenerErrores()) {
            cout << "  [!] [" << err.tipo << "] Linea " << err.linea
                 << ", Col " << err.columna << ": " << err.mensaje << endl;
        }
        return;
    }
    cout << "  [+] Validacion sintactica y semantica exitosa (0 errores)." << endl;

    cout << "\n[4] GENERACION Y OPTIMIZACION DE CODIGO INTERMEDIO (3 DIRECCIONES):" << endl;
    const vector<InstruccionTAC>& tacOriginal = sintactico.obtenerGenerador().obtenerInstrucciones();

    OptimizadorCodigo optimizador;
    optimizador.optimizar(tacOriginal);
    optimizador.imprimirComparativa();
}

int main() {
    std::cout << "Anguiano Garcia Angel Yahir Guadalupe, Figueroa Robles Axel Israel, Molina Alvarado Alvaro Moises, Torres Martinez Miguel Angel" << std::endl;

    SetConsoleOutputCP(CP_UTF8);

    cout << "\n================================================================================" << endl;
    cout << "  OPTIMIZACION DE CODIGO INTERMEDIO - COMPILADOR C++" << endl;
    cout << "================================================================================" << endl;

    // =========================================================================
    // PRUEBA 1: REGLA 1 - IDENTIFICACIÓN DE CÓDIGO MUERTO (SALTOS GOTO)
    // =========================================================================
    vector<InstruccionTAC> casoRegla1 = {
        {"a = 10", "Asigna el valor 10 a a", false},
        {"b = 20", "Asigna el valor 20 a b", false},
        {"goto L_FIN", "Salto incondicional hacia L_FIN", false},
        {"a = 999", "Codigo muerto (inaccesible tras goto)", false},
        {"b = 888", "Codigo muerto (inaccesible tras goto)", false},
        {"L_FIN:", "Declaracion de etiqueta L_FIN", false},
        {"t1 = a + b", "Calcula a + b en t1", false},
        {"result = t1", "Asigna resultado final a result", false}
    };
    probarReglaTAC("PRUEBA 1: REGLA 1 - IDENTIFICACION DE CODIGO MUERTO (SALTOS GOTO)", casoRegla1);

    // =========================================================================
    // PRUEBA 2: REGLA 2 - ANÁLISIS DE VARIABLES NO UTILIZADAS
    // =========================================================================
    vector<InstruccionTAC> casoRegla2 = {
        {"a = 15", "Asigna el valor 15 a a", false},
        {"b = 25", "Asigna el valor 25 a b", false},
        {"varSinUso1 = 100", "Variable asignada pero nunca leida", false},
        {"varSinUso2 = 500", "Variable asignada pero nunca leida", false},
        {"t1 = a + b", "Calcula a + b en t1", false},
        {"result = t1", "Asigna el resultado final a result", false}
    };
    probarReglaTAC("PRUEBA 2: REGLA 2 - ANALISIS DE VARIABLES NO UTILIZADAS", casoRegla2);

    // =========================================================================
    // PRUEBA 3: REGLA 3 - ELIMINACIÓN DE CÁLCULOS REPETIDOS (REDUNDANTES)
    // Ejemplo exacto: t1 = a + b; t2 = a + b; result = t1; -> t1 = a + b; result = t1;
    // =========================================================================
    vector<InstruccionTAC> casoRegla3 = {
        {"t1 = a + b", "Primer calculo de a + b en t1", false},
        {"t2 = a + b", "Calculo repetido de a + b en t2", false},
        {"result = t1", "Asigna t1 al resultado final", false}
    };
    probarReglaTAC("PRUEBA 3: REGLA 3 - ELIMINACION DE CALCULOS REPETIDOS (REDUNDANTES)", casoRegla3);

    // =========================================================================
    // PRUEBA 4: REGLA 4 - ELIMINACIÓN DE ASIGNACIONES INNECESARIAS
    // Ejemplo exacto: temp = a; temp = b; result = temp; -> result = b;
    // =========================================================================
    vector<InstruccionTAC> casoRegla4 = {
        {"temp = a", "Asigna a en temp (sera sobrescrita)", false},
        {"temp = b", "Sobrescribe temp con el valor de b", false},
        {"result = temp", "Asigna temp al resultado final", false}
    };
    probarReglaTAC("PRUEBA 4: REGLA 4 - ELIMINACION DE ASIGNACIONES INNECESARIAS", casoRegla4);

    // =========================================================================
    // PRUEBA 5: INTEGRACIÓN COMPLETA DEL COMPILADOR (LAS 4 REGLAS COMBINADAS)
    // =========================================================================
    string codigoFuenteIntegral = R"(
        int a = 10;
        int b = 20;

        // Regla 2: Variable asignada pero nunca utilizada
        int basura = 999;

        // Regla 3: Calculos repetidos (a + b dos veces sin cambiar a ni b)
        int suma1 = a + b;
        int suma2 = a + b;

        // Regla 1: Salto incondicional con instrucciones muertas antes de L_SALIDA
        goto L_SALIDA;
        a = 500;
        b = 600;
        L_SALIDA:

        // Regla 4: Asignaciones innecesarias (sobrescritura inmediata y copia)
        int temp = a;
        temp = suma1;
        int result = temp;
    )";
    probarFlujoCompletoCompilador(
        "PRUEBA 5: PIPELINE COMPLETO DEL COMPILADOR (LAS 4 REGLAS INTEGRADAS)",
        codigoFuenteIntegral
    );

    std::cout << "Anguiano Garcia Angel Yahir Guadalupe, Figueroa Robles Axel Israel, Molina Alvarado Alvaro Moises, Torres Martinez Miguel Angel" << std::endl;
    return 0;
}
