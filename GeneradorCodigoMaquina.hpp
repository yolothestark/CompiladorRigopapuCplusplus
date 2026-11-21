// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#ifndef GENERADOR_CODIGO_MAQUINA_HPP
#define GENERADOR_CODIGO_MAQUINA_HPP

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "GeneradorCodigoIntermedio.hpp"

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct InstruccionMaquina {
    std::string etiqueta;          // Etiqueta asociada a esta direccion (ej. L1, L2)
    std::string mnemonico;         // LOAD, ADD, SUB, MUL, DIV, STORE, CMP, JMP, JZ, JNZ, CALL, RET, ALLOC
    std::string op1;               // Primer operando simbolico (ej. R1, result, L_FIN)
    std::string op2;               // Segundo operando simbolico (ej. a, b, c, R1)
    std::string comentario;        // Comentario explicativo de la instruccion
    uint32_t direccionMemoria = 0; // Direccion en segmento de codigo (0x1000, 0x1004, ...)
    std::string op1Direccion;      // Primer operando con direccion resuelta (ej. R1, 0x3000)
    std::string op2Direccion;      // Segundo operando con direccion resuelta (ej. 0x2000, R1)
    uint32_t palabraBinaria = 0;   // Codificacion binaria de 32 bits de la instruccion
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct SimboloDireccionMemoria {
    std::string simbolo;           // Nombre de variable o etiqueta (ej. a, b, c, result, L1)
    uint32_t direccion;            // Direccion asignada (ej. 0x2000, 0x2004, 0x3000, 0x1000)
    std::string segmento;          // .data (0x2000), .result (0x3000), .text (0x1000)
    std::string categoria;         // Variable, Resultado, Etiqueta
    int valorInicial = 0;          // Valor inicial en memoria para ejecucion
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct RegistroOptimizacionMaquina {
    std::string regla;
    std::string instruccionOriginal;
    std::string detalleTransformacion;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct ErrorCodigoMaquina {
    int indiceTupla;
    std::string tupla;
    std::string tipo;
    std::string mensaje;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct CabeceraEjecutable {
    std::string magicNumber = "7F 52 50 45 (RPE - RigoPapu Executable)";
    std::string arquitectura = "RigoPapu-32 RISC Processor (32-bit)";
    uint32_t puntoEntrada = 0x1000;
    uint32_t baseCodigo = 0x1000;
    uint32_t baseDatos = 0x2000;
    uint32_t baseResultados = 0x3000;
    uint32_t tamanoCodigoBytes = 0;
    uint32_t tamanoDatosBytes = 0;
    uint32_t totalSimbolos = 0;
    std::string permisosCodigo = "r-x (Lectura y Ejecucion)";
    std::string permisosDatos = "rw- (Lectura y Escritura)";
    std::vector<std::string> libreriasCargadas;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct PasoEjecucionProcesador {
    int ciclo;
    uint32_t pc;
    std::string instruccionEnsamblador;
    int registroR1;
    std::string cambioMemoria;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
class GeneradorCodigoMaquina {
private:
    std::vector<InstruccionTAC> tacEntrada;
    std::vector<InstruccionMaquina> codigoMaquinaBase;
    std::vector<InstruccionMaquina> codigoMaquinaMapeado;
    std::vector<InstruccionMaquina> codigoMaquinaOptimizado;
    std::vector<SimboloDireccionMemoria> tablaSimbolosMemoria;
    std::unordered_map<std::string, uint32_t> mapaDirecciones;
    std::vector<RegistroOptimizacionMaquina> bitacoraOptimizacion;
    std::vector<ErrorCodigoMaquina> errores;
    CabeceraEjecutable cabecera;
    std::vector<PasoEjecucionProcesador> trazaEjecucion;
    std::unordered_map<uint32_t, int> memoriaEjecucion;

    bool validarInstruccionesTAC(const std::vector<InstruccionTAC>& instrucciones);
    void generarMapeoInstrucciones(const std::vector<InstruccionTAC>& instrucciones);
    void aplicarOptimizacionInstrucciones(bool modoAcumulacionAditiva = false);
    void resolverDireccionesYTablaSimbolos(std::vector<InstruccionMaquina>& instrucciones,
                                           const std::unordered_map<std::string, int>& valoresIniciales = {});
    uint32_t codificarInstruccionBinaria(const InstruccionMaquina& inst) const;

public:
    GeneradorCodigoMaquina() = default;

    // Procesa el código intermedio completo: valida, traduce, optimiza y asigna direcciones
    bool generarDesdeTAC(const std::vector<InstruccionTAC>& instrucciones,
                         bool modoAcumulacionAditiva = false,
                         const std::unordered_map<std::string, int>& valoresIniciales = {});

    // 3. Preparación para la ejecución: configura entorno, crea archivo ejecutable y simula CPU
    bool configurarEntornoYGenerarEjecutable(const std::string& nombreArchivoEjecutable,
                                             const std::unordered_map<std::string, int>& valoresEntrada = {});

    bool tieneErrores() const;
    const std::vector<ErrorCodigoMaquina>& obtenerErrores() const;
    const std::vector<InstruccionMaquina>& obtenerCodigoMapeado() const;
    const std::vector<InstruccionMaquina>& obtenerCodigoOptimizado() const;
    const std::vector<SimboloDireccionMemoria>& obtenerTablaSimbolosMemoria() const;

    // Métodos de impresión estructurada para el reporte y video demostrativo
    static std::string formatearHex(uint32_t valor, int digitos = 4);
    void imprimirMapeoInstrucciones() const;
    void imprimirOptimizacionInstrucciones() const;
    void imprimirAsignacionDireccionesYTablaSimbolos() const;
    void imprimirPreparacionYEjecucion(const std::string& rutaEjecutable) const;
    void imprimirErrores() const;
};

#endif
