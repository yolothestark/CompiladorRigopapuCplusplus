// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#ifndef OPTIMIZADOR_CODIGO_HPP
#define OPTIMIZADOR_CODIGO_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "GeneradorCodigoIntermedio.hpp"

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct RegistroOptimizacion {
    std::string regla;
    std::string instruccionOriginal;
    std::string detalleAccion;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
class OptimizadorCodigo {
private:
    std::vector<InstruccionTAC> codigoOriginal;
    std::vector<InstruccionTAC> codigoOptimizado;
    std::vector<RegistroOptimizacion> bitacora;

    // Métodos internos correspondientes a las 4 reglas de optimización
    void eliminarCodigoMuertoSaltos(std::vector<InstruccionTAC>& instrucciones);
    void eliminarVariablesNoUtilizadas(std::vector<InstruccionTAC>& instrucciones);
    void eliminarCalculosRepetidos(std::vector<InstruccionTAC>& instrucciones);
    void eliminarAsignacionesInnecesarias(std::vector<InstruccionTAC>& instrucciones);

public:
    OptimizadorCodigo() = default;

    // Recibe las instrucciones TAC generadas y aplica las 4 reglas de optimización
    std::vector<InstruccionTAC> optimizar(const std::vector<InstruccionTAC>& instruccionesEntrada);

    // Getters de resultados y métricas
    const std::vector<InstruccionTAC>& obtenerCodigoOriginal() const;
    const std::vector<InstruccionTAC>& obtenerCodigoOptimizado() const;
    const std::vector<RegistroOptimizacion>& obtenerBitacora() const;

    // Funciones de impresión tabulada y limpia para consola / capturas de pantalla
    static void imprimirTablaCodigo(const std::string& titulo, const std::vector<InstruccionTAC>& instrucciones);
    void imprimirComparativa() const;
    void imprimirResumenOptimizacion() const;
};

#endif
