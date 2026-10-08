// Anguiano Garcia Angel Yahir Guadalupe, Figueroa Robles Axel Israel, Molina Alvarado Alvaro Moises, Torres Martinez Miguel Angel
#ifndef TABLASIMBOLOS_HPP
#define TABLASIMBOLOS_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

enum class TipoDato { INT, FLOAT, VOID, DESCONOCIDO };

struct Simbolo {
    std::string nombre;
    TipoDato tipo;
    std::string alcance;
    int linea;
    int columna;
};

class TablaSimbolos {
private:
    std::vector<std::unordered_map<std::string, Simbolo>> pilaAmbitos;

public:
    TablaSimbolos() { 
        entrarAmbito(); 
    }

    void entrarAmbito() {
        pilaAmbitos.push_back(std::unordered_map<std::string, Simbolo>());
    }

    void salirAmbito() {
        if (pilaAmbitos.size() > 1) pilaAmbitos.pop_back();
    }

    bool insertarSimbolo(const std::string& nombre, TipoDato tipo, int linea, int columna) {
        auto& ambitoActual = pilaAmbitos.back();
        
        if (ambitoActual.find(nombre) != ambitoActual.end()) {
            std::cout << "Error Semántico: El identificador '" << nombre 
                      << "' ya ha sido declarado en este ámbito (Línea " << linea 
                      << ", Columna " << columna << ").\n";
            return false;
        }

        std::string alcance = (pilaAmbitos.size() == 1) ? "global" : "local";
        ambitoActual[nombre] = {nombre, tipo, alcance, linea, columna};
        return true;
    }

    Simbolo* buscarSimbolo(const std::string& nombre, int linea, int columna) {
        for (auto it = pilaAmbitos.rbegin(); it != pilaAmbitos.rend(); ++it) {
            if (it->find(nombre) != it->end()) {
                return &((*it)[nombre]);
            }
        }
        std::cout << "Error Semántico: Identificador '" << nombre 
                  << "' no declarado en la línea " << linea 
                  << ", columna " << columna << ".\n";
        return nullptr;
    }

    bool verificarAsignacion(const std::string& nombreVariable, TipoDato tipoValor, int linea, int columna) {
        Simbolo* sim = buscarSimbolo(nombreVariable, linea, columna);
        if (sim == nullptr) return false;

        if (sim->tipo != tipoValor) {
            std::cout << "Error Semántico: Tipo de dato incompatible en la asignación a '" 
                      << nombreVariable << "' en la línea " << linea 
                      << ", columna " << columna << ".\n";
            return false;
        }
        return true;
    }
};

#endif
