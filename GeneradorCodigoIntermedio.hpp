// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#ifndef GENERADOR_CODIGO_INTERMEDIO_HPP
#define GENERADOR_CODIGO_INTERMEDIO_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct InstruccionTAC {
    std::string codigo;
    std::string comentario;
    bool esComentarioSeccion;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
class GeneradorCodigoIntermedio {
private:
    int contadorTemporales;
    int contadorEtiquetas;
    std::vector<std::vector<InstruccionTAC>> pilaBuffers;

public:
    GeneradorCodigoIntermedio() : contadorTemporales(1), contadorEtiquetas(1) {
        pilaBuffers.push_back(std::vector<InstruccionTAC>());
    }

    void reiniciar() {
        contadorTemporales = 1;
        contadorEtiquetas = 1;
        pilaBuffers.clear();
        pilaBuffers.push_back(std::vector<InstruccionTAC>());
    }

    void reiniciarEtiquetas() {
        contadorEtiquetas = 1;
    }

    std::string nuevoTemporal() {
        return "t" + std::to_string(contadorTemporales++);
    }

    std::string nuevaEtiqueta() {
        return "L" + std::to_string(contadorEtiquetas++);
    }

    void emitir(const std::string& codigo, const std::string& comentario = "") {
        pilaBuffers.back().push_back({codigo, comentario, false});
    }

    void emitirEtiqueta(const std::string& etiqueta, const std::string& comentario = "") {
        pilaBuffers.back().push_back({etiqueta + ":", comentario, false});
    }

    void emitirSeccion(const std::string& titulo) {
        pilaBuffers.back().push_back({"# " + titulo, "", true});
    }

    void iniciarBuffer() {
        pilaBuffers.push_back(std::vector<InstruccionTAC>());
    }

    std::vector<InstruccionTAC> extraerBuffer() {
        if (pilaBuffers.size() <= 1) {
            return {};
        }
        std::vector<InstruccionTAC> buffer = pilaBuffers.back();
        pilaBuffers.pop_back();
        return buffer;
    }

    void agregarInstrucciones(const std::vector<InstruccionTAC>& instrucciones) {
        for (const auto& inst : instrucciones) {
            pilaBuffers.back().push_back(inst);
        }
    }

    const std::vector<InstruccionTAC>& obtenerInstrucciones() const {
        return pilaBuffers.front();
    }

    void imprimirCodigoIntermedio() const {
        const auto& instrucciones = pilaBuffers.front();
        if (instrucciones.empty()) {
            std::cout << "  (No se generaron instrucciones de codigo intermedio)\n";
            return;
        }

        for (const auto& inst : instrucciones) {
            if (inst.esComentarioSeccion) {
                std::cout << "  " << inst.codigo << "\n";
            } else {
                std::cout << "  " << std::left << std::setw(24) << inst.codigo;
                if (!inst.comentario.empty()) {
                    std::cout << "# " << inst.comentario;
                }
                std::cout << "\n";
            }
        }
    }
};

#endif
