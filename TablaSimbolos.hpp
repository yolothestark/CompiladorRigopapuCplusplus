// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#ifndef TABLASIMBOLOS_HPP
#define TABLASIMBOLOS_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

enum class TipoDato { INT, FLOAT, CHAR, BOOLEAN, STRING, VOID, OBJETO, DESCONOCIDO };

inline std::string tipoDatoToString(TipoDato tipo) {
    switch (tipo) {
        case TipoDato::INT: return "int";
        case TipoDato::FLOAT: return "float";
        case TipoDato::CHAR: return "char";
        case TipoDato::BOOLEAN: return "boolean";
        case TipoDato::STRING: return "string";
        case TipoDato::VOID: return "void";
        case TipoDato::OBJETO: return "objeto";
        default: return "desconocido";
    }
}

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct Simbolo {
    std::string nombre;
    TipoDato tipo;
    std::string alcance;
    int linea;
    int columna;
    std::string claseAsociada;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct SimboloFuncion {
    std::string nombre;
    TipoDato tipoRetorno;
    std::vector<TipoDato> tiposParametros;
    int linea;
    int columna;
    std::string etiqueta;
    std::string clasePerteneciente;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
class TablaSimbolos {
private:
    std::vector<std::unordered_map<std::string, Simbolo>> pilaAmbitos;
    std::unordered_map<std::string, SimboloFuncion> tablaFunciones;
    std::unordered_set<std::string> tablaClases;
    std::function<void(const std::string&, int, int)> notificadorError;

    void reportar(const std::string& msg, int linea, int columna) {
        if (notificadorError) {
            notificadorError(msg, linea, columna);
        } else {
            std::cout << "Error Semantico: " << msg << " (Linea " << linea << ", Columna " << columna << ").\n";
        }
    }

public:
    TablaSimbolos() { 
        entrarAmbito(); 
    }

    void configurarNotificadorError(std::function<void(const std::string&, int, int)> callback) {
        notificadorError = callback;
    }

    void reiniciar() {
        pilaAmbitos.clear();
        tablaFunciones.clear();
        tablaClases.clear();
        entrarAmbito();
    }

    void entrarAmbito() {
        pilaAmbitos.push_back(std::unordered_map<std::string, Simbolo>());
    }

    void salirAmbito() {
        if (pilaAmbitos.size() > 1) {
            pilaAmbitos.pop_back();
        }
    }

    bool insertarClase(const std::string& nombreClase, int linea, int columna) {
        if (tablaClases.count(nombreClase)) {
            reportar("La clase '" + nombreClase + "' ya ha sido declarada previamente.", linea, columna);
            return false;
        }
        tablaClases.insert(nombreClase);
        return true;
    }

    bool existeClase(const std::string& nombreClase) const {
        return tablaClases.count(nombreClase) > 0;
    }

    bool insertarSimbolo(const std::string& nombre, TipoDato tipo, int linea, int columna, const std::string& claseAsociada = "") {
        auto& ambitoActual = pilaAmbitos.back();
        
        if (ambitoActual.find(nombre) != ambitoActual.end()) {
            reportar("El identificador '" + nombre + "' ya ha sido declarado en este ambito.", linea, columna);
            return false;
        }

        std::string alcance = (pilaAmbitos.size() == 1) ? "global" : "local";
        ambitoActual[nombre] = {nombre, tipo, alcance, linea, columna, claseAsociada};
        return true;
    }

    Simbolo* buscarSimbolo(const std::string& nombre, int linea, int columna, bool silencioso = false) {
        for (auto it = pilaAmbitos.rbegin(); it != pilaAmbitos.rend(); ++it) {
            if (it->find(nombre) != it->end()) {
                return &((*it)[nombre]);
            }
        }
        if (!silencioso) {
            reportar("Identificador '" + nombre + "' no declarado.", linea, columna);
        }
        return nullptr;
    }

    bool verificarAsignacion(const std::string& nombreVariable, TipoDato tipoValor, int linea, int columna) {
        Simbolo* sim = buscarSimbolo(nombreVariable, linea, columna);
        if (sim == nullptr) return false;

        if (tipoValor == TipoDato::DESCONOCIDO) return false;
        if (sim->tipo == tipoValor) return true;

        if (sim->tipo == TipoDato::FLOAT && tipoValor == TipoDato::INT) {
            return true; 
        }

        reportar("Tipo de dato incompatible en la asignacion a '" + nombreVariable + 
                 "' (esperado: " + tipoDatoToString(sim->tipo) + 
                 ", recibido: " + tipoDatoToString(tipoValor) + ").", linea, columna);
        return false;
    }

    bool insertarFuncion(const std::string& nombre, TipoDato tipoRetorno, 
                         const std::vector<TipoDato>& params, int linea, int columna,
                         const std::string& etiqueta = "", const std::string& clasePerteneciente = "") {
        if (tablaFunciones.find(nombre) != tablaFunciones.end()) {
            reportar("El metodo '" + nombre + "' ya fue declarado previamente.", linea, columna);
            return false;
        }

        tablaFunciones[nombre] = {nombre, tipoRetorno, params, linea, columna, etiqueta, clasePerteneciente};
        return true;
    }

    SimboloFuncion* buscarFuncion(const std::string& nombre, int linea, int columna) {
        auto it = tablaFunciones.find(nombre);
        if (it != tablaFunciones.end()) {
            return &(it->second);
        }
        reportar("El metodo '" + nombre + "' no ha sido declarado.", linea, columna);
        return nullptr;
    }
};

#endif
