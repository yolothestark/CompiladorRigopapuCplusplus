// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include "AnalizadorLexico.h"
#include "AnalizadorSintactico.h"
#include "GeneradorCodigoIntermedio.hpp"
#include "OptimizadorCodigo.hpp"
#include "GeneradorCodigoMaquina.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using namespace std;

// ============================================================================
// Ejecuta el pipeline completo del compilador desde código fuente C++ hasta
// la generación de código máquina, tabla de símbolos de direcciones y ejecución
// ============================================================================
void probarFlujoCompletoGeneracionCodigo(const string& titulo,
                                         const string& codigoFuente,
                                         const string& archivoEjecutable) {
    cout << "\n================================================================================" << endl;
    cout << "  " << titulo << endl;
    cout << "================================================================================" << endl;

    cout << "\n[FASE 1] CODIGO FUENTE DE ENTRADA:" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << codigoFuente << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    AnalizadorLexico lexico;
    vector<Token> tokens = lexico.analizar(codigoFuente);

    cout << "\n[FASE 2] ANALISIS LEXICO:" << endl;
    if (lexico.tieneErrores()) {
        for (const auto& err : lexico.obtenerErrores()) {
            cout << "  [!] Error Lexico (Linea " << err.linea << ", Col " << err.columna
                 << "): " << err.mensaje << " ('" << err.valor << "')" << endl;
        }
        cout << "  [X] Generacion de codigo maquina abortada por errores lexicos." << endl;
        return;
    }
    cout << "  [+] Analisis lexico completado sin errores (" << tokens.size() << " tokens generados)." << endl;

    AnalizadorSintactico sintactico;
    sintactico.analizar(tokens);

    cout << "\n[FASE 3] ANALISIS SINTACTICO Y SEMANTICO:" << endl;
    if (sintactico.tieneErrores()) {
        for (const auto& err : sintactico.obtenerErrores()) {
            cout << "  [!] [" << err.tipo << "] Linea " << err.linea
                 << ", Col " << err.columna << ": " << err.mensaje << endl;
        }
        cout << "  [X] Generacion de codigo maquina abortada por errores sintacticos/semanticos." << endl;
        return;
    }
    cout << "  [+] Validacion sintactica y semantica exitosa (0 errores)." << endl;

    cout << "\n[FASE 4] GENERACION Y OPTIMIZACION DE CODIGO INTERMEDIO (TAC):" << endl;
    const vector<InstruccionTAC>& tacOriginal = sintactico.obtenerGenerador().obtenerInstrucciones();
    OptimizadorCodigo optimizadorTAC;
    vector<InstruccionTAC> tacOptimizado = optimizadorTAC.optimizar(tacOriginal);
    optimizadorTAC.imprimirComparativa();

    cout << "\n[FASE 5] GENERACION DE CODIGO MAQUINA, DIRECCIONAMIENTO Y EJECUCION:" << endl;
    GeneradorCodigoMaquina generadorMaquina;
    if (!generadorMaquina.generarDesdeTAC(tacOptimizado, false)) {
        generadorMaquina.imprimirErrores();
        return;
    }

    generadorMaquina.imprimirMapeoInstrucciones();
    generadorMaquina.imprimirOptimizacionInstrucciones();
    generadorMaquina.imprimirAsignacionDireccionesYTablaSimbolos();
    generadorMaquina.configurarEntornoYGenerarEjecutable(archivoEjecutable);
    generadorMaquina.imprimirPreparacionYEjecucion(archivoEjecutable);
}

int main() {
    std::cout << "Anguiano Garcia Angel Yahir Guadalupe, Figueroa Robles Axel Israel, Molina Alvarado Alvaro Moises, Torres Martinez Miguel Angel" << std::endl;

    SetConsoleOutputCP(CP_UTF8);

    cout << "\n================================================================================" << endl;
    cout << "  ACTIVIDAD 13.- GENERACION DE CODIGO (CODIGO MAQUINA Y EJECUCION) - C++" << endl;
    cout << "================================================================================" << endl;

    // =========================================================================
    // PRUEBA 1 (VALIDA): PUNTO 1.1 - TRADUCCIÓN DE CÓDIGO INTERMEDIO A
    // INSTRUCCIONES DE MÁQUINA (MAPEO DE INSTRUCCIONES)
    // Ejemplo exacto de la práctica:
    //   t1 = a + b
    //   t2 = t1 * c
    //   result = t2
    // -> LOAD R1, a | ADD R1, b | MUL R1, c | STORE result, R1
    // =========================================================================
    cout << "\n================================================================================" << endl;
    cout << "  PRUEBA 1 (VALIDA): 1.1. MAPEO DE INSTRUCCIONES (TAC A CODIGO MAQUINA)" << endl;
    cout << "================================================================================" << endl;
    vector<InstruccionTAC> casoMapeo = {
        {"t1 = a + b", "Suma a y b en el temporal t1", false},
        {"t2 = t1 * c", "Multiplica t1 por c en el temporal t2", false},
        {"result = t2", "Asigna el resultado final a result", false}
    };
    GeneradorCodigoMaquina genMapeo;
    genMapeo.generarDesdeTAC(casoMapeo, false, {{"a", 4}, {"b", 6}, {"c", 5}});
    genMapeo.imprimirMapeoInstrucciones();

    // =========================================================================
    // PRUEBA 2 (VALIDA): PUNTO 1.2 - OPTIMIZACIÓN DE INSTRUCCIONES DE MÁQUINA
    // Ejemplo exacto de la práctica:
    //   t1 = a + b
    //   t2 = t1 + c
    //   result = t2
    // -> ADD R1, a | ADD R1, b | ADD R1, c | STORE result, R1
    // =========================================================================
    cout << "\n================================================================================" << endl;
    cout << "  PRUEBA 2 (VALIDA): 1.2. OPTIMIZACION DE INSTRUCCIONES DE MAQUINA" << endl;
    cout << "================================================================================" << endl;
    vector<InstruccionTAC> casoOptMaquina = {
        {"t1 = a + b", "Suma inicial de a y b en t1", false},
        {"t2 = t1 + c", "Suma encadenada de t1 y c en t2", false},
        {"result = t2", "Almacena t2 en result", false}
    };
    GeneradorCodigoMaquina genOptMaquina;
    genOptMaquina.generarDesdeTAC(casoOptMaquina, true, {{"a", 10}, {"b", 20}, {"c", 30}});
    genOptMaquina.imprimirOptimizacionInstrucciones();

    // =========================================================================
    // PRUEBA 3 (VALIDA): PUNTO 2.1 Y 2.2 - ASIGNACIÓN DE DIRECCIONES DE MEMORIA
    // Y GENERACIÓN DE TABLA DE SÍMBOLOS
    // Ejemplo exacto de la práctica:
    //   L1: t1 = a + b
    //   L2: t2 = t1 * c
    //   result = t2
    // -> 0x1000: LOAD R1, 0x2000 | 0x1004: ADD R1, 0x2004
    //    0x1008: MUL R1, 0x2008  | 0x100C: STORE 0x3000, R1
    // Tabla de símbolos: a=0x2000, b=0x2004, c=0x2008, result=0x3000
    // =========================================================================
    cout << "\n================================================================================" << endl;
    cout << "  PRUEBA 3 (VALIDA): 2. ASIGNACION DE DIRECCIONES DE MEMORIA Y TABLA DE SIMBOLOS" << endl;
    cout << "================================================================================" << endl;
    vector<InstruccionTAC> casoDirecciones = {
        {"L1: t1 = a + b", "Etiqueta L1: suma a + b en t1", false},
        {"L2: t2 = t1 * c", "Etiqueta L2: multiplica t1 * c en t2", false},
        {"result = t2", "Guarda el resultado final en result", false}
    };
    GeneradorCodigoMaquina genDirecciones;
    genDirecciones.generarDesdeTAC(casoDirecciones, false, {{"a", 10}, {"b", 20}, {"c", 3}});
    genDirecciones.imprimirAsignacionDireccionesYTablaSimbolos();

    // =========================================================================
    // PRUEBA 4 (VALIDA): PUNTO 3.1 Y 3.2 - PREPARACIÓN PARA LA EJECUCIÓN
    // (CREACIÓN DE ARCHIVO EJECUTABLE, CONFIGURACIÓN DE ENTORNO Y EJECUCIÓN)
    // =========================================================================
    cout << "\n================================================================================" << endl;
    cout << "  PRUEBA 4 (VALIDA): 3. PREPARACION PARA LA EJECUCION Y SIMULACION DE CPU" << endl;
    cout << "================================================================================" << endl;
    genDirecciones.configurarEntornoYGenerarEjecutable("programa_ejecutable_p4.rpe", {{"a", 10}, {"b", 20}, {"c", 3}});
    genDirecciones.imprimirPreparacionYEjecucion("programa_ejecutable_p4.rpe");

    // =========================================================================
    // PRUEBA 5 (VALIDA): FLUJO INTEGRAL COMPLETO DEL COMPILADOR C++
    // (Código Fuente -> Léxico -> Sintáctico/Semántico -> TAC -> Optimizador TAC
    //  -> Código Máquina -> Direccionamiento -> Archivo Ejecutable -> CPU)
    // =========================================================================
    string codigoFuenteIntegral = R"(
        int a = 12;
        int b = 8;
        int c = 4;

        // Variable sin uso (sera limpiada en fase de optimizacion TAC)
        int basura = 777;

        // Expresion aritmetica combinada
        int suma = a + b;
        int producto = suma * c;

        // Salto con codigo muerto intermedio
        goto L_CALCULO_FINAL;
        a = 999;
        L_CALCULO_FINAL:

        int result = producto - 10;
    )";
    probarFlujoCompletoGeneracionCodigo(
        "PRUEBA 5 (VALIDA): PIPELINE COMPLETO DEL COMPILADOR (FUENTE C++ A EJECUTABLE)",
        codigoFuenteIntegral,
        "ejecutable_integral_p5.rpe"
    );

    // =========================================================================
    // PRUEBA 6 (INVALIDA): DETECCIÓN DE ERRORES EN GENERACIÓN DE CÓDIGO MÁQUINA
    // Y EN PIPELINE DEL COMPILADOR (PRUEBAS INVÁLIDAS DE TODAS LAS REGLAS)
    // =========================================================================
    cout << "\n================================================================================" << endl;
    cout << "  PRUEBA 6A (INVALIDA): ERRORES DE TRADUCCION ISA Y DIRECCIONAMIENTO DE MEMORIA" << endl;
    cout << "================================================================================" << endl;
    vector<InstruccionTAC> casoInvalidoMaquina = {
        {"t1 = a @ b", "Error: Operador '@' no existe en el conjunto de instrucciones de la ALU", false},
        {"t2 = t1 / 0", "Error: Division por cero al mapear instruccion DIV R1, 0", false},
        {"goto L_NO_DECLARADA", "Error: Salto JMP hacia una etiqueta sin direccion en 0x1000+", false},
        {"123invalido = t2", "Error: Simbolo de destino invalido para la Tabla de Simbolos (0x2000+)", false}
    };
    GeneradorCodigoMaquina genInvalido;
    if (!genInvalido.generarDesdeTAC(casoInvalidoMaquina)) {
        genInvalido.imprimirErrores();
    }

    string codigoFuenteInvalido = R"(
        int a = 10;
        int b = "texto_incompatible";
        int c = a / 0;
        result = noDeclarada + 5;
    )";
    probarFlujoCompletoGeneracionCodigo(
        "PRUEBA 6B (INVALIDA): CODIGO FUENTE CON ERRORES QUE IMPIDEN GENERAR EJECUTABLE",
        codigoFuenteInvalido,
        "ejecutable_invalido.rpe"
    );

    std::cout << "Anguiano Garcia Angel Yahir Guadalupe, Figueroa Robles Axel Israel, Molina Alvarado Alvaro Moises, Torres Martinez Miguel Angel" << std::endl;
    return 0;
}
