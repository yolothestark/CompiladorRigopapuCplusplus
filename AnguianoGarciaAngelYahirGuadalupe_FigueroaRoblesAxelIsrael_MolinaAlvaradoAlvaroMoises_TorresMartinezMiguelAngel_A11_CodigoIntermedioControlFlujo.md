# Universidad de Guadalajara
## Centro Universitario
### Materia: Seminario de Solución de Problemas de Traductores de Lenguajes II / Compiladores
**Profesor:** Rigoberto Cárdenas Larios

---

## **Actividad 11.- Código Intermedio para Control de Flujo**

### **Integrantes del Equipo (en orden alfabético por primer apellido):**
1. **Anguiano Garcia Angel Yahir Guadalupe**
2. **Figueroa Robles Axel Israel**
3. **Molina Alvarado Alvaro Moises**
4. **Torres Martinez Miguel Angel**

**Fecha de entrega:** 6 de noviembre de 2026

---

## **Enlace Público del Video Descriptivo y Demostrativo (YouTube)**
> **URL del Video:** `[PEGAR AQUÍ EL ENLACE PÚBLICO DE YOUTUBE]`

---

## **1. Introducción y Objetivo**

### **Introducción**
La generación de código intermedio constituye el puente entre el front-end (análisis léxico, sintáctico y semántico) y el back-end del compilador. Mediante el uso de **Código de Tres Direcciones (TAC, *Three-Address Code*)**, las estructuras de alto nivel como condicionales (`if-else`, `switch`), ciclos (`while`, `for`, `do-while`), funciones/métodos y clases/objetos se descomponen en instrucciones elementales de asignación, evaluación con variables temporales (`t1, t2, ...`), etiquetas simbólicas (`L1, L2, ...`) y saltos condicionales o incondicionales (`if ... goto`, `if not ... goto`, `goto`, `call`, `return`).

### **Objetivo**
Generar una representación de tres direcciones para el control de flujo del programa que incluya estructuras condicionales (`if`, `else`, `switch`), bucles (`while`, `for`, `do-while`), y la definición y llamada de métodos y clases, verificando previamente la validez léxica, sintáctica y semántica del código fuente.

---

## **2. Reglas de Traducción a Código Intermedio (Tres Direcciones)**

| Estructura | Regla de Traducción (Esquema TAC) |
| :--- | :--- |
| **`if - else`** | Evalúa la condición en un temporal `t_k = cond`. Emite `if t_k goto L_if`, seguido de las instrucciones del bloque `else`, un salto incondicional `goto L_fin`, la etiqueta `L_if:` con las instrucciones del bloque `if`, y finalmente la etiqueta `L_fin:`. |
| **`switch - case - default`** | Asigna la expresión a un temporal `t_sw = expr`. Por cada `case val_i` emite `if t_sw == val_i goto L_i`, seguido de `goto L_def` (o `goto L_fin`). Luego emite cada bloque `L_i:` finalizando sus sentencias `break` con `goto L_fin`, el bloque `L_def:` y la etiqueta de cierre `L_fin:`. |
| **`while`** | Emite etiqueta de inicio `L_inicio:`, evalúa `t_k = cond`, emite `if not t_k goto L_fin`, ejecuta el cuerpo del ciclo, salta al inicio con `goto L_inicio` y cierra con `L_fin:`. |
| **`for`** | Emite la instrucción de inicialización antes del ciclo, coloca `L_inicio:`, evalúa `t_k = cond`, emite `if not t_k goto L_fin`, emite el cuerpo del bucle, inserta la instrucción de actualización (`i = i + 1`), emite `goto L_inicio` y cierra con `L_fin:`. |
| **`do - while`** | Coloca la etiqueta `L_inicio:`, ejecuta las instrucciones del cuerpo del bucle, evalúa la condición al final en `t_k = cond` y emite el salto condicional `if t_k goto L_inicio`. |
| **Definición y Llamada de Métodos** | Asigna una etiqueta única `L_metodo:` al inicio del método y traduce `return expr` como `t_k = expr` seguido de `return t_k`. En la llamada evalúa cada argumento en temporales (`t_p = arg`), emite `call L_metodo` y asigna el retorno mediante `var = return_value`. |
| **Clases y Objetos** | Registra la clase y sus métodos con etiquetas propias (`L_metodo:`). La instanciación `Clase obj = new Clase();` genera `obj = create Clase`, y la invocación `obj.metodo();` genera `call L_metodo`. |

---

## **3. Código Fuente Desarrollado**

### **3.1. `GeneradorCodigoIntermedio.hpp`**
```cpp
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
```

---

### **3.2. `TablaSimbolos.hpp`**
```cpp
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
```

---

### **3.3. `AnalizadorLexico.h`**
```cpp
// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#pragma once

#include <string>
#include <vector>

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct Token {
    std::string tipo;
    std::string valor;
    int linea;
    int columna;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct ErrorLexico {
    int linea;
    int columna;
    std::string tipo;
    std::string mensaje;
    std::string valor;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
class AnalizadorLexico {
private:
    std::vector<Token> tokens;
    std::vector<ErrorLexico> errores;

public:
    std::vector<Token> analizar(const std::string& codigo);
    const std::vector<ErrorLexico>& obtenerErrores() const;
    bool tieneErrores() const;
};
```

---

### **3.4. `AnalizadorLexico.cpp`**
```cpp
// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#include "AnalizadorLexico.h"
#include <regex>
#include <unordered_set>
#include <cctype>

using namespace std;

const vector<ErrorLexico>& AnalizadorLexico::obtenerErrores() const {
    return errores;
}

bool AnalizadorLexico::tieneErrores() const {
    return !errores.empty();
}

static bool esInicioId(unsigned char c) {
    return isalpha(c) || c == '_' || c >= 128;
}

static bool esCuerpoId(unsigned char c) {
    return isalnum(c) || c == '_' || c >= 128;
}

vector<Token> AnalizadorLexico::analizar(const string& codigo)
{
    vector<Token> resultado;
    errores.clear();

    unordered_set<string> palabrasClave = {
        "auto", "break", "case", "char", "class",
        "const", "continue", "default", "delete", "do",
        "double", "else", "enum", "false", "float",
        "for", "if", "int", "long", "namespace", "new",
        "nullptr", "private", "boolean", "string", "protected", "public", "return",
        "short", "signed", "sizeof", "static", "struct",
        "switch", "this", "throw", "true", "try",
        "typedef", "using", "virtual", "void", "volatile",
        "while"
    };

    regex decimal("[0-9]+\\.[0-9]+");
    regex entero("[0-9]+");
    regex cadena("\"([^\"\\\\]|\\\\.)*\"");
    regex caracter("'([^'\\\\]|\\\\.)'");

    regex err_decimal_multiple("[0-9]+\\.[0-9]+(\\.[0-9]+)+");
    
    size_t posicion = 0;
    int linea_actual = 1;
    int columna_actual = 1;

    auto avanzar = [&](size_t cantidad, const string& texto_consumido) {
        for (size_t i = 0; i < cantidad; i++) {
            if (texto_consumido[i] == '\n') {
                linea_actual++;
                columna_actual = 1;
            } else {
                columna_actual++;
            }
        }
        posicion += cantidad;
    };

    while (posicion < codigo.length())
    {
        if (isspace(static_cast<unsigned char>(codigo[posicion])))
        {
            string espacio = string(1, codigo[posicion]);
            avanzar(1, espacio);
            continue;
        }

        if (posicion + 1 < codigo.length() && codigo[posicion] == '/' && codigo[posicion + 1] == '/')
        {
            size_t start = posicion;
            while (posicion < codigo.length() && codigo[posicion] != '\n') {
                posicion++;
            }
            string consumido = codigo.substr(start, posicion - start);
            posicion = start;
            avanzar(consumido.length(), consumido);
            continue;
        }

        if (posicion + 1 < codigo.length() && codigo[posicion] == '/' && codigo[posicion + 1] == '*')
        {
            size_t start = posicion;
            int start_linea = linea_actual;
            int start_columna = columna_actual;
            posicion += 2;

            while (posicion + 1 < codigo.length() && !(codigo[posicion] == '*' && codigo[posicion + 1] == '/')) {
                posicion++;
            }

            if (posicion + 1 < codigo.length()) {
                posicion += 2;
                string consumido = codigo.substr(start, posicion - start);
                posicion = start;
                avanzar(consumido.length(), consumido);
            } else {
                string consumido = codigo.substr(start);
                errores.push_back({start_linea, start_columna, "Error Lexico", "Comentario multilineal no cerrado", consumido});
                posicion = start;
                avanzar(consumido.length(), consumido);
            }
            continue;
        }

        if (codigo[posicion] == '"') {
            size_t len = 1;
            bool cerrado = false;
            while (posicion + len < codigo.length() && codigo[posicion + len] != '\n') {
                if (codigo[posicion + len] == '\\') {
                    if (posicion + len + 1 < codigo.length()) {
                        len += 2;
                        continue;
                    }
                }
                if (codigo[posicion + len] == '"') {
                    len++;
                    cerrado = true;
                    break;
                }
                len++;
            }

            if (!cerrado) {
                string valor_err = codigo.substr(posicion, len);
                errores.push_back({linea_actual, columna_actual, "Error Lexico", "Cadena mal formada (falta cierre de comillas)", valor_err});
                avanzar(len, valor_err);
                continue;
            }
        }

        smatch coincidencia;
        string restante = codigo.substr(posicion);

        if (regex_search(restante, coincidencia, err_decimal_multiple, regex_constants::match_continuous))
        {
            errores.push_back({linea_actual, columna_actual, "Error Lexico", "Literal numerico incorrecto (multiples puntos decimales)", coincidencia.str()});
            avanzar(coincidencia.length(), coincidencia.str());
            continue;
        }

        if (isdigit(static_cast<unsigned char>(restante[0]))) {
            size_t idx = 0;
            while (idx < restante.length() && isdigit(static_cast<unsigned char>(restante[idx]))) {
                idx++;
            }
            if (idx < restante.length() && esInicioId(static_cast<unsigned char>(restante[idx]))) {
                while (idx < restante.length() && esCuerpoId(static_cast<unsigned char>(restante[idx]))) {
                    idx++;
                }
                string err_val = restante.substr(0, idx);
                errores.push_back({linea_actual, columna_actual, "Error Lexico", "Identificador mal formado (inicia con numeros)", err_val});
                avanzar(err_val.length(), err_val);
                continue;
            }
        }

        if (regex_search(restante, coincidencia, cadena, regex_constants::match_continuous))
        {
            resultado.push_back({"CADENA", coincidencia.str(), linea_actual, columna_actual});
            avanzar(coincidencia.length(), coincidencia.str());
            continue;
        }

        if (regex_search(restante, coincidencia, caracter, regex_constants::match_continuous))
        {
            resultado.push_back({"CARACTER", coincidencia.str(), linea_actual, columna_actual});
            avanzar(coincidencia.length(), coincidencia.str());
            continue;
        }

        if (regex_search(restante, coincidencia, decimal, regex_constants::match_continuous))
        {
            resultado.push_back({"DECIMAL", coincidencia.str(), linea_actual, columna_actual});
            avanzar(coincidencia.length(), coincidencia.str());
            continue;
        }

        if (regex_search(restante, coincidencia, entero, regex_constants::match_continuous))
        {
            resultado.push_back({"ENTERO", coincidencia.str(), linea_actual, columna_actual});
            avanzar(coincidencia.length(), coincidencia.str());
            continue;
        }

        if (esInicioId(static_cast<unsigned char>(restante[0])))
        {
            size_t idx = 1;
            while (idx < restante.length() && esCuerpoId(static_cast<unsigned char>(restante[idx]))) {
                idx++;
            }
            string palabra = restante.substr(0, idx);
            if (palabrasClave.count(palabra))
            {
                resultado.push_back({"PALABRA_CLAVE", palabra, linea_actual, columna_actual});
            }
            else
            {
                resultado.push_back({"IDENTIFICADOR", palabra, linea_actual, columna_actual});
            }
            avanzar(palabra.length(), palabra);
            continue;
        }

        if (posicion + 1 < codigo.length())
        {
            string operador = codigo.substr(posicion, 2);
            bool es_operador_doble = true;

            if (operador == "==" || operador == "!=" || operador == "<=" || operador == ">=")
                resultado.push_back({"OPERADOR_RELACIONAL", operador, linea_actual, columna_actual});
            else if (operador == "&&" || operador == "||")
                resultado.push_back({"OPERADOR_LOGICO", operador, linea_actual, columna_actual});
            else if (operador == "++" || operador == "--")
                resultado.push_back({"OPERADOR_INCREMENTO", operador, linea_actual, columna_actual});
            else if (operador == "+=" || operador == "-=" || operador == "*=" || operador == "/=")
                resultado.push_back({"OPERADOR_ASIGNACION", operador, linea_actual, columna_actual});
            else if (operador == "::")
                resultado.push_back({"OPERADOR_RESOLUCION", operador, linea_actual, columna_actual});
            else
                es_operador_doble = false;

            if (es_operador_doble) {
                avanzar(2, operador);
                continue;
            }
        }

        char simbolo = codigo[posicion];
        string str_simbolo = string(1, simbolo);
        bool procesado = true;

        if (simbolo == '+' || simbolo == '-' || simbolo == '*' || simbolo == '/' || simbolo == '%')
            resultado.push_back({"OPERADOR_ARITMETICO", str_simbolo, linea_actual, columna_actual});
        else if (simbolo == '<' || simbolo == '>')
            resultado.push_back({"OPERADOR_RELACIONAL", str_simbolo, linea_actual, columna_actual});
        else if (simbolo == '!')
            resultado.push_back({"OPERADOR_LOGICO", str_simbolo, linea_actual, columna_actual});
        else if (simbolo == '=')
            resultado.push_back({"OPERADOR_ASIGNACION", str_simbolo, linea_actual, columna_actual});
        else if (simbolo == '(' || simbolo == ')' || simbolo == '{' || simbolo == '}' || 
                 simbolo == '[' || simbolo == ']' || simbolo == ',' || simbolo == ';' || 
                 simbolo == '.' || simbolo == ':')
            resultado.push_back({"SIMBOLO", str_simbolo, linea_actual, columna_actual});
        else
            procesado = false;

        if (procesado) {
            avanzar(1, str_simbolo);
            continue;
        }

        errores.push_back({linea_actual, columna_actual, "Error Lexico", "Caracter no reconocido", str_simbolo});
        avanzar(1, str_simbolo);
    }

    return resultado;
}
```

---

### **3.5. `AnalizadorSintactico.h`**
```cpp
// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#pragma once

#include <string>
#include <vector>
#include "AnalizadorLexico.h"
#include "TablaSimbolos.hpp"
#include "GeneradorCodigoIntermedio.hpp"

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct ErrorSintactico {
    int linea;
    int columna;
    std::string tipo; 
    std::string mensaje;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct ResultadoExpresion {
    TipoDato tipo;
    std::string lugar;
};

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
class AnalizadorSintactico {
private:
    std::vector<Token> tokens;
    size_t indiceActual;
    std::vector<ErrorSintactico> errores;

    TablaSimbolos tablaSimbolos;
    GeneradorCodigoIntermedio generador;

    bool dentroDeFuncion;
    TipoDato tipoRetornoFuncionActual;
    std::string nombreFuncionActual;
    std::string claseActual;
    int nivelBucles;
    int nivelSwitch;
    std::vector<std::string> pilaEtiquetasFinBucle;

    Token tokenActual() const;
    Token tokenAnterior() const;
    Token tokenSiguiente() const;
    bool estaAlFinal() const;
    Token avanzar();
    bool comprobar(const std::string& tipo, const std::string& valor = "") const;
    bool coincidir(const std::string& tipo, const std::string& valor = "");
    
    void reportarError(const std::string& mensaje);
    void reportarErrorSemantico(const std::string& mensaje, int linea, int columna);
    void sincronizar();

    void programa();
    void sentencia();
    void estructuraControl();
    void sentenciaIf();
    void sentenciaSwitch();
    void sentenciaWhile();
    void sentenciaFor();
    void sentenciaDoWhile();
    void sentenciaReturn();
    void sentenciaBreak();
    
    void bloque();
    
    ResultadoExpresion expresion();
    ResultadoExpresion expresionSimple();
    ResultadoExpresion llamadaFuncion(const std::string& nombre, const std::string& objetoReceptor, int linea, int columna);
    
    void inicializacion();
    void actualizacion();

    void declaracionClase();
    void cuerpoClase();
    void declaracionMetodoOVariable();
    void declaracionObjetoOExpresion();
    void declaracionMetodo(TipoDato tipoRetorno, const std::string& nombre, int linea, int columna);
    void parametros(std::vector<std::pair<std::string, TipoDato>>& listaParams);
    void parametro(std::vector<std::pair<std::string, TipoDato>>& listaParams);
    
    bool esTipoDato() const;
    TipoDato tipoDatoDesdeString(const std::string& str) const;

public:
    AnalizadorSintactico();
    void analizar(const std::vector<Token>& tokensEntrada);
    const std::vector<ErrorSintactico>& obtenerErrores() const;
    const GeneradorCodigoIntermedio& obtenerGenerador() const;
    bool tieneErrores() const;
    void imprimirErrores() const;
    void imprimirCodigoIntermedio() const;
};
```

---

### **3.6. `AnalizadorSintactico.cpp`**
```cpp
// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#include "AnalizadorSintactico.h"
#include <iostream>
#include <unordered_set>

using namespace std;

AnalizadorSintactico::AnalizadorSintactico()
    : indiceActual(0),
      dentroDeFuncion(false),
      tipoRetornoFuncionActual(TipoDato::VOID),
      nombreFuncionActual(""),
      claseActual(""),
      nivelBucles(0),
      nivelSwitch(0) {
    tablaSimbolos.configurarNotificadorError([this](const string& msg, int linea, int col) {
        this->reportarErrorSemantico(msg, linea, col);
    });
}

TipoDato AnalizadorSintactico::tipoDatoDesdeString(const string& tipoStr) const {
    if (tipoStr == "int") return TipoDato::INT;
    if (tipoStr == "float") return TipoDato::FLOAT;
    if (tipoStr == "char") return TipoDato::CHAR;
    if (tipoStr == "void") return TipoDato::VOID;
    if (tipoStr == "boolean" || tipoStr == "bool") return TipoDato::BOOLEAN;
    if (tipoStr == "string") return TipoDato::STRING;
    return TipoDato::DESCONOCIDO;
}

const vector<ErrorSintactico>& AnalizadorSintactico::obtenerErrores() const {
    return errores;
}

const GeneradorCodigoIntermedio& AnalizadorSintactico::obtenerGenerador() const {
    return generador;
}

bool AnalizadorSintactico::tieneErrores() const {
    return !errores.empty();
}

void AnalizadorSintactico::imprimirErrores() const {
    if (errores.empty()) {
        cout << "\n>>> [EXITO] Analisis finalizado sin errores.\n";
        return;
    }
    cout << "\n>>> [ERRORES DETECTADOS]:\n";
    for (const auto& err : errores) {
        cout << "  - [" << err.tipo << "] Linea " << err.linea 
             << ", Columna " << err.columna << ": " << err.mensaje << "\n";
    }
}

void AnalizadorSintactico::imprimirCodigoIntermedio() const {
    generador.imprimirCodigoIntermedio();
}

Token AnalizadorSintactico::tokenActual() const {
    if (indiceActual < tokens.size()) {
        return tokens[indiceActual];
    }
    return {"EOF", "", -1, -1};
}

Token AnalizadorSintactico::tokenAnterior() const {
    if (indiceActual > 0) {
        return tokens[indiceActual - 1];
    }
    return {"EOF", "", -1, -1};
}

Token AnalizadorSintactico::tokenSiguiente() const {
    if (indiceActual + 1 < tokens.size()) {
        return tokens[indiceActual + 1];
    }
    return {"EOF", "", -1, -1};
}

bool AnalizadorSintactico::estaAlFinal() const {
    return indiceActual >= tokens.size();
}

Token AnalizadorSintactico::avanzar() {
    if (!estaAlFinal()) {
        indiceActual++;
    }
    return tokenAnterior();
}

bool AnalizadorSintactico::comprobar(const string& tipo, const string& valor) const {
    if (estaAlFinal()) return false;
    if (tokenActual().tipo != tipo) return false;
    if (!valor.empty() && tokenActual().valor != valor) return false;
    return true;
}

bool AnalizadorSintactico::coincidir(const string& tipo, const string& valor) {
    if (comprobar(tipo, valor)) {
        avanzar();
        return true;
    }
    return false;
}

void AnalizadorSintactico::reportarError(const string& mensaje) {
    Token t = tokenActual();
    if (t.tipo == "EOF" && !tokens.empty()) {
        t = tokens.back();
    }
    errores.push_back({t.linea, t.columna, "Sintactico", mensaje});
}

void AnalizadorSintactico::reportarErrorSemantico(const string& mensaje, int linea, int columna) {
    errores.push_back({linea, columna, "Semantico", mensaje});
}

void AnalizadorSintactico::sincronizar() {
    avanzar();
    while (!estaAlFinal()) {
        if (tokenAnterior().valor == ";") return;

        if (comprobar("PALABRA_CLAVE", "if") ||
            comprobar("PALABRA_CLAVE", "switch") ||
            comprobar("PALABRA_CLAVE", "while") ||
            comprobar("PALABRA_CLAVE", "for") ||
            comprobar("PALABRA_CLAVE", "do") ||
            comprobar("PALABRA_CLAVE", "int") ||
            comprobar("PALABRA_CLAVE", "float") ||
            comprobar("PALABRA_CLAVE", "char") ||
            comprobar("PALABRA_CLAVE", "void") ||
            comprobar("PALABRA_CLAVE", "boolean") ||
            comprobar("PALABRA_CLAVE", "string") ||
            comprobar("PALABRA_CLAVE", "class") ||
            comprobar("PALABRA_CLAVE", "return") ||
            comprobar("PALABRA_CLAVE", "break")) {
            return;
        }
        avanzar();
    }
}

void AnalizadorSintactico::analizar(const vector<Token>& tokensEntrada) {
    tokens = tokensEntrada;
    indiceActual = 0;
    errores.clear();
    tablaSimbolos.reiniciar();
    generador.reiniciar();
    dentroDeFuncion = false;
    nombreFuncionActual = "";
    claseActual = "";
    tipoRetornoFuncionActual = TipoDato::VOID;
    nivelBucles = 0;
    nivelSwitch = 0;
    pilaEtiquetasFinBucle.clear();

    programa();
}

void AnalizadorSintactico::programa() {
    while (!estaAlFinal()) {
        if (comprobar("PALABRA_CLAVE", "class")) {
            declaracionClase();
        } else if (esTipoDato()) {
            declaracionMetodoOVariable();
        } else {
            sentencia();
        }
    }
}

bool AnalizadorSintactico::esTipoDato() const {
    return comprobar("PALABRA_CLAVE", "int") || 
           comprobar("PALABRA_CLAVE", "float") || 
           comprobar("PALABRA_CLAVE", "void") || 
           comprobar("PALABRA_CLAVE", "char") || 
           comprobar("PALABRA_CLAVE", "string") || 
           comprobar("PALABRA_CLAVE", "boolean");
}

void AnalizadorSintactico::declaracionClase() {
    coincidir("PALABRA_CLAVE", "class");
    
    Token tokenNombre = tokenActual();
    if (!coincidir("IDENTIFICADOR")) {
        reportarError("Se esperaba un identificador para el nombre de la clase.");
        return;
    }

    string nombreClase = tokenNombre.valor;
    tablaSimbolos.insertarClase(nombreClase, tokenNombre.linea, tokenNombre.columna);
    generador.emitirSeccion("Definicion de la clase " + nombreClase);
    
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Cuerpo de clase invalido en la declaracion de 'class' (Falta '{').");
    }
    
    string clasePrevia = claseActual;
    claseActual = nombreClase;
    tablaSimbolos.entrarAmbito();

    cuerpoClase();

    tablaSimbolos.salirAmbito();
    claseActual = clasePrevia;
    
    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Cuerpo de clase invalido en la declaracion de 'class' (Falta '}').");
    }
}

void AnalizadorSintactico::cuerpoClase() {
    while (!estaAlFinal() && !comprobar("SIMBOLO", "}")) {
        if (esTipoDato()) {
            declaracionMetodoOVariable();
        } else {
            reportarError("Se esperaba una declaracion de metodo o atributo dentro de la clase.");
            sincronizar(); 
        }
    }
}

void AnalizadorSintactico::declaracionMetodoOVariable() {
    Token tokenTipo = tokenActual();
    avanzar(); 
    
    Token tokenVar = tokenActual();
    if (!coincidir("IDENTIFICADOR")) {
        reportarError("Se esperaba un identificador despues del tipo de dato.");
        return;
    }

    TipoDato tipoDeclarado = tipoDatoDesdeString(tokenTipo.valor);

    if (comprobar("SIMBOLO", "(")) {
        declaracionMetodo(tipoDeclarado, tokenVar.valor, tokenVar.linea, tokenVar.columna);
    } else {
        tablaSimbolos.insertarSimbolo(tokenVar.valor, tipoDeclarado, tokenVar.linea, tokenVar.columna);

        if (coincidir("OPERADOR_ASIGNACION", "=")) {
            ResultadoExpresion resExpr = expresion();
            tablaSimbolos.verificarAsignacion(tokenVar.valor, resExpr.tipo, tokenVar.linea, tokenVar.columna);
            if (!resExpr.lugar.empty()) {
                generador.emitir(tokenVar.valor + " = " + resExpr.lugar, 
                                 "Asigna el valor " + resExpr.lugar + " a " + tokenVar.valor);
            }
        }
        if (!coincidir("SIMBOLO", ";")) {
            reportarError("Se esperaba ';' al final de la declaracion de variable.");
        }
    }
}

void AnalizadorSintactico::declaracionMetodo(TipoDato tipoRetorno, const string& nombre, int linea, int columna) {
    coincidir("SIMBOLO", "(");

    vector<pair<string, TipoDato>> listaParametros;
    parametros(listaParametros);

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Falta ')' en la declaracion de parametros del metodo.");
    }

    vector<TipoDato> tiposFirma;
    for (const auto& p : listaParametros) {
        tiposFirma.push_back(p.second);
    }

    string etiquetaMetodo = generador.nuevaEtiqueta();
    tablaSimbolos.insertarFuncion(nombre, tipoRetorno, tiposFirma, linea, columna, etiquetaMetodo, claseActual);

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Cuerpo de metodo invalido en la declaracion (Falta '{').");
        sincronizar();
        return;
    }

    if (claseActual.empty()) {
        generador.emitirSeccion("Definicion del metodo " + nombre);
    }
    generador.emitirEtiqueta(etiquetaMetodo, "Etiqueta " + etiquetaMetodo + " marca el inicio del metodo " + nombre);

    bool contextoPrevioFuncion = dentroDeFuncion;
    TipoDato retornoPrevio = tipoRetornoFuncionActual;
    string nombrePrevio = nombreFuncionActual;

    dentroDeFuncion = true;
    tipoRetornoFuncionActual = tipoRetorno;
    nombreFuncionActual = nombre;

    tablaSimbolos.entrarAmbito();
    for (const auto& p : listaParametros) {
        tablaSimbolos.insertarSimbolo(p.first, p.second, linea, columna);
    }

    bloque();

    tablaSimbolos.salirAmbito();

    dentroDeFuncion = contextoPrevioFuncion;
    tipoRetornoFuncionActual = retornoPrevio;
    nombreFuncionActual = nombrePrevio;
}

void AnalizadorSintactico::parametros(vector<pair<string, TipoDato>>& listaParams) {
    if (!comprobar("SIMBOLO", ")")) { 
        parametro(listaParams);
        while (coincidir("SIMBOLO", ",")) {
            parametro(listaParams);
        }
    }
}

void AnalizadorSintactico::parametro(vector<pair<string, TipoDato>>& listaParams) {
    if (esTipoDato()) {
        Token tokenTipo = tokenActual();
        avanzar();
        
        Token tokenVar = tokenActual();
        if (!coincidir("IDENTIFICADOR")) {
            reportarError("Parametro invalido (Falta identificador).");
        } else {
            listaParams.push_back({tokenVar.valor, tipoDatoDesdeString(tokenTipo.valor)});
        }
    } else {
        reportarError("Parametro invalido (Se esperaba un tipo de dato).");
        if (!estaAlFinal() && !comprobar("SIMBOLO", ")") && !comprobar("SIMBOLO", ",")) {
            avanzar();
        }
    }
}

void AnalizadorSintactico::sentencia() {
    if (comprobar("PALABRA_CLAVE", "if") || 
        comprobar("PALABRA_CLAVE", "switch") ||
        comprobar("PALABRA_CLAVE", "while") || 
        comprobar("PALABRA_CLAVE", "for") ||
        comprobar("PALABRA_CLAVE", "do")) {
        estructuraControl();
    } 
    else if (comprobar("PALABRA_CLAVE", "return")) {
        sentenciaReturn();
    }
    else if (comprobar("PALABRA_CLAVE", "break")) {
        sentenciaBreak();
    }
    else if (comprobar("SIMBOLO", "{")) {
        tablaSimbolos.entrarAmbito();
        bloque();
        tablaSimbolos.salirAmbito();
    }
    else if (comprobar("IDENTIFICADOR") && tokenSiguiente().tipo == "IDENTIFICADOR") {
        declaracionObjetoOExpresion();
    }
    else {
        expresion();
        if (!coincidir("SIMBOLO", ";")) {
            reportarError("Se esperaba ';' al final de la sentencia.");
            sincronizar();
        }
    }
}

void AnalizadorSintactico::declaracionObjetoOExpresion() {
    Token tokenClase = avanzar();
    Token tokenObj = avanzar();

    if (!tablaSimbolos.existeClase(tokenClase.valor)) {
        reportarErrorSemantico("La clase '" + tokenClase.valor + "' no ha sido declarada.", 
                               tokenClase.linea, tokenClase.columna);
    }

    tablaSimbolos.insertarSimbolo(tokenObj.valor, TipoDato::OBJETO, tokenObj.linea, tokenObj.columna, tokenClase.valor);

    if (coincidir("OPERADOR_ASIGNACION", "=")) {
        if (!coincidir("PALABRA_CLAVE", "new")) {
            reportarError("Se esperaba la palabra clave 'new' para instanciar el objeto '" + tokenObj.valor + "'.");
        }
        Token tokenClaseNew = tokenActual();
        if (!coincidir("IDENTIFICADOR")) {
            reportarError("Se esperaba el nombre de la clase despues de 'new'.");
        } else if (tokenClaseNew.valor != tokenClase.valor) {
            reportarErrorSemantico("El constructor '" + tokenClaseNew.valor + 
                                   "' no coincide con el tipo de clase '" + tokenClase.valor + "'.",
                                   tokenClaseNew.linea, tokenClaseNew.columna);
        }

        if (!coincidir("SIMBOLO", "(") || !coincidir("SIMBOLO", ")")) {
            reportarError("Se esperaban parentesis '()' en el constructor de '" + tokenClase.valor + "'.");
        }

        generador.emitirSeccion("Creacion del objeto " + tokenClase.valor);
        generador.emitir("obj = create " + tokenClase.valor, 
                         "Representa la creacion del objeto " + tokenObj.valor);
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' al final de la creacion del objeto.");
    }
}

void AnalizadorSintactico::sentenciaReturn() {
    Token tokenRet = tokenActual();
    coincidir("PALABRA_CLAVE", "return");

    if (!dentroDeFuncion) {
        reportarErrorSemantico("Sentencia 'return' fuera del cuerpo de un metodo.", 
                               tokenRet.linea, tokenRet.columna);
    }

    ResultadoExpresion resRet = {TipoDato::VOID, ""};
    if (!comprobar("SIMBOLO", ";")) {
        resRet = expresion();
    }

    if (dentroDeFuncion) {
        if (resRet.tipo != tipoRetornoFuncionActual && resRet.tipo != TipoDato::DESCONOCIDO) {
            reportarErrorSemantico("El metodo '" + nombreFuncionActual + "' debe retornar '" +
                                   tipoDatoToString(tipoRetornoFuncionActual) + "' pero retorno '" +
                                   tipoDatoToString(resRet.tipo) + "'.", 
                                   tokenRet.linea, tokenRet.columna);
        }
    }

    if (!resRet.lugar.empty()) {
        generador.emitir("return " + resRet.lugar, "Retorna el valor de " + resRet.lugar);
    } else {
        generador.emitir("return", "Retorno del metodo void");
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' al final de la sentencia 'return'.");
    }
}

void AnalizadorSintactico::sentenciaBreak() {
    Token tokenBrk = tokenActual();
    coincidir("PALABRA_CLAVE", "break");

    if (nivelBucles == 0 && nivelSwitch == 0) {
        reportarErrorSemantico("La sentencia 'break' solo puede usarse dentro de un bucle o estructura 'switch'.",
                               tokenBrk.linea, tokenBrk.columna);
    }

    if (nivelSwitch > 0) {
        generador.emitir("goto <BREAK_SWITCH>", "Salta al final de la estructura switch");
    } else if (!pilaEtiquetasFinBucle.empty()) {
        string etiqFin = pilaEtiquetasFinBucle.back();
        generador.emitir("goto " + etiqFin, "Sale del bucle actual hacia " + etiqFin);
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' despues de 'break'.");
    }
}

void AnalizadorSintactico::estructuraControl() {
    if (comprobar("PALABRA_CLAVE", "if")) {
        sentenciaIf();
    } else if (comprobar("PALABRA_CLAVE", "switch")) {
        sentenciaSwitch();
    } else if (comprobar("PALABRA_CLAVE", "while")) {
        sentenciaWhile();
    } else if (comprobar("PALABRA_CLAVE", "for")) {
        sentenciaFor();
    } else if (comprobar("PALABRA_CLAVE", "do")) {
        sentenciaDoWhile();
    }
}

void AnalizadorSintactico::sentenciaIf() {
    Token tokenIf = tokenActual();
    coincidir("PALABRA_CLAVE", "if");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues del 'if'.");
    }

    ResultadoExpresion cond = expresion();
    if (cond.tipo != TipoDato::BOOLEAN && cond.tipo != TipoDato::DESCONOCIDO) {
        reportarErrorSemantico("La condicion del 'if' debe ser de tipo boolean (recibido: " + 
                               tipoDatoToString(cond.tipo) + ").", tokenIf.linea, tokenIf.columna);
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condicion del 'if'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'if'.");
        sincronizar();
        return;
    }

    generador.iniciarBuffer();
    tablaSimbolos.entrarAmbito();
    bloque();
    tablaSimbolos.salirAmbito();
    vector<InstruccionTAC> instruccionesIf = generador.extraerBuffer();

    if (coincidir("PALABRA_CLAVE", "else")) {
        if (!comprobar("SIMBOLO", "{")) {
            reportarError("Falta la llave de apertura '{' en la estructura 'else'.");
            sincronizar();
            return;
        }

        string etiqIf = generador.nuevaEtiqueta();
        string etiqFin = generador.nuevaEtiqueta();

        generador.emitir("if " + cond.lugar + " goto " + etiqIf, 
                         "Si " + cond.lugar + " es verdadero, salta a la etiqueta " + etiqIf);

        tablaSimbolos.entrarAmbito();
        bloque();
        tablaSimbolos.salirAmbito();

        generador.emitir("goto " + etiqFin, 
                         "Salta a la etiqueta " + etiqFin + " despues del bloque else");
        generador.emitirEtiqueta(etiqIf, "Etiqueta " + etiqIf + " para el bloque if");
        generador.agregarInstrucciones(instruccionesIf);
        generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final de la estructura condicional");
    } else {
        string etiqFin = generador.nuevaEtiqueta();
        generador.emitir("if not " + cond.lugar + " goto " + etiqFin, 
                         "Si " + cond.lugar + " es falso, salta a la etiqueta " + etiqFin);
        generador.agregarInstrucciones(instruccionesIf);
        generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final de la estructura condicional");
    }
}

void AnalizadorSintactico::sentenciaSwitch() {
    Token tokenSw = tokenActual();
    coincidir("PALABRA_CLAVE", "switch");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues de 'switch'.");
    }

    ResultadoExpresion exprSw = expresion();
    if (exprSw.tipo == TipoDato::BOOLEAN || exprSw.tipo == TipoDato::VOID) {
        reportarErrorSemantico("La expresion del 'switch' no puede ser de tipo '" + 
                               tipoDatoToString(exprSw.tipo) + "'.", tokenSw.linea, tokenSw.columna);
    }

    string tempSwitch = generador.nuevoTemporal();
    generador.emitir(tempSwitch + " = " + exprSw.lugar, 
                     tempSwitch + " es una etiqueta temporal para el valor de " + exprSw.lugar);

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la expresion del 'switch'.");
    }

    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'switch'.");
        sincronizar();
        return;
    }

    struct CasoSwitch {
        string valor;
        string etiqueta;
        vector<InstruccionTAC> instrucciones;
    };

    vector<CasoSwitch> casos;
    unordered_set<string> valoresCasosVistos;
    bool tieneDefault = false;
    string etiqDefault = "";
    vector<InstruccionTAC> instruccionesDefault;

    nivelSwitch++;
    tablaSimbolos.entrarAmbito();

    while (!comprobar("SIMBOLO", "}") && !estaAlFinal()) {
        if (coincidir("PALABRA_CLAVE", "case")) {
            Token tokenVal = tokenActual();
            TipoDato tipoCaso = TipoDato::DESCONOCIDO;
            if (comprobar("ENTERO")) {
                tipoCaso = TipoDato::INT;
                avanzar();
            } else if (comprobar("CARACTER")) {
                tipoCaso = TipoDato::CHAR;
                avanzar();
            } else if (comprobar("CADENA")) {
                tipoCaso = TipoDato::STRING;
                avanzar();
            } else {
                reportarError("Se esperaba una constante valida en 'case'.");
                avanzar();
            }

            if (exprSw.tipo != TipoDato::DESCONOCIDO && tipoCaso != TipoDato::DESCONOCIDO && exprSw.tipo != tipoCaso) {
                reportarErrorSemantico("El tipo del 'case' (" + tipoDatoToString(tipoCaso) + 
                                       ") es incompatible con la expresion del 'switch' (" + 
                                       tipoDatoToString(exprSw.tipo) + ").", tokenVal.linea, tokenVal.columna);
            }

            if (valoresCasosVistos.count(tokenVal.valor)) {
                reportarErrorSemantico("Valor de 'case' duplicado (" + tokenVal.valor + ") en 'switch'.",
                                       tokenVal.linea, tokenVal.columna);
            }
            valoresCasosVistos.insert(tokenVal.valor);

            if (!coincidir("SIMBOLO", ":")) {
                reportarError("Se esperaba ':' despues del valor del 'case'.");
            }

            string etiqCaso = generador.nuevaEtiqueta();
            generador.iniciarBuffer();
            while (!comprobar("PALABRA_CLAVE", "case") && 
                   !comprobar("PALABRA_CLAVE", "default") && 
                   !comprobar("SIMBOLO", "}") && !estaAlFinal()) {
                sentencia();
            }
            vector<InstruccionTAC> instsCaso = generador.extraerBuffer();
            casos.push_back({tokenVal.valor, etiqCaso, instsCaso});
        } 
        else if (coincidir("PALABRA_CLAVE", "default")) {
            if (!coincidir("SIMBOLO", ":")) {
                reportarError("Se esperaba ':' despues de 'default'.");
            }
            tieneDefault = true;
            etiqDefault = generador.nuevaEtiqueta();
            generador.iniciarBuffer();
            while (!comprobar("PALABRA_CLAVE", "case") && 
                   !comprobar("PALABRA_CLAVE", "default") && 
                   !comprobar("SIMBOLO", "}") && !estaAlFinal()) {
                sentencia();
            }
            instruccionesDefault = generador.extraerBuffer();
        } 
        else {
            reportarError("Se esperaba 'case' o 'default' dentro de 'switch'.");
            avanzar();
        }
    }

    tablaSimbolos.salirAmbito();
    nivelSwitch--;

    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Falta la llave de cierre '}' en la estructura 'switch'.");
    }

    string etiqFin = generador.nuevaEtiqueta();

    for (const auto& c : casos) {
        generador.emitir("if " + tempSwitch + " == " + c.valor + " goto " + c.etiqueta,
                         "Si " + tempSwitch + " es " + c.valor + ", salta a la etiqueta " + c.etiqueta);
    }

    if (tieneDefault) {
        generador.emitir("goto " + etiqDefault, 
                         "Si " + tempSwitch + " no coincide con ningun caso, salta a la etiqueta " + etiqDefault);
    } else {
        generador.emitir("goto " + etiqFin, 
                         "Si " + tempSwitch + " no coincide con ningun caso, salta al final (" + etiqFin + ")");
    }

    for (const auto& c : casos) {
        generador.emitirEtiqueta(c.etiqueta, "Etiqueta " + c.etiqueta + " para el caso " + c.valor);
        for (auto inst : c.instrucciones) {
            if (inst.codigo == "goto <BREAK_SWITCH>") {
                inst.codigo = "goto " + etiqFin;
                inst.comentario = "Salta a la etiqueta " + etiqFin + " despues de manejar el caso " + c.valor;
            }
            generador.emitir(inst.codigo, inst.comentario);
        }
    }

    if (tieneDefault) {
        generador.emitirEtiqueta(etiqDefault, "Etiqueta " + etiqDefault + " para el caso default");
        for (auto inst : instruccionesDefault) {
            if (inst.codigo == "goto <BREAK_SWITCH>") {
                inst.codigo = "goto " + etiqFin;
                inst.comentario = "Salta a la etiqueta " + etiqFin + " despues del caso default";
            }
            generador.emitir(inst.codigo, inst.comentario);
        }
    }

    generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final de la estructura switch");
}

void AnalizadorSintactico::sentenciaWhile() {
    Token tokenWhile = tokenActual();
    coincidir("PALABRA_CLAVE", "while");

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues del 'while'.");
    }

    string etiqInicio = generador.nuevaEtiqueta();
    generador.emitirEtiqueta(etiqInicio, "Etiqueta " + etiqInicio + " marca el inicio del bucle");

    ResultadoExpresion cond = expresion();
    if (cond.tipo != TipoDato::BOOLEAN && cond.tipo != TipoDato::DESCONOCIDO) {
        reportarErrorSemantico("La condicion del 'while' debe ser de tipo boolean (recibido: " + 
                               tipoDatoToString(cond.tipo) + ").", tokenWhile.linea, tokenWhile.columna);
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condicion del 'while'.");
    }

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'while'.");
        sincronizar();
        return;
    }

    string etiqFin = generador.nuevaEtiqueta();
    generador.emitir("if not " + cond.lugar + " goto " + etiqFin, 
                     "Si " + cond.lugar + " es falso, salta a la etiqueta " + etiqFin + " (fin del bucle)");

    nivelBucles++;
    pilaEtiquetasFinBucle.push_back(etiqFin);
    tablaSimbolos.entrarAmbito();
    bloque();
    tablaSimbolos.salirAmbito();
    pilaEtiquetasFinBucle.pop_back();
    nivelBucles--;

    generador.emitir("goto " + etiqInicio, "Salta de vuelta al inicio del bucle");
    generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final del bucle");
}

void AnalizadorSintactico::sentenciaFor() {
    coincidir("PALABRA_CLAVE", "for");
    if (!coincidir("SIMBOLO", "(")) reportarError("Se esperaba '(' despues del 'for'.");
    
    tablaSimbolos.entrarAmbito();

    if (!comprobar("SIMBOLO", ";")) inicializacion();
    if (!coincidir("SIMBOLO", ";")) reportarError("Componente invalido o falta ';' tras inicializacion en 'for'.");
    
    string etiqInicio = generador.nuevaEtiqueta();
    generador.emitirEtiqueta(etiqInicio, "Etiqueta " + etiqInicio + " marca el inicio del bucle");

    ResultadoExpresion cond = {TipoDato::BOOLEAN, "true"};
    if (!comprobar("SIMBOLO", ";")) {
        Token tokenCond = tokenActual();
        cond = expresion();
        if (cond.tipo != TipoDato::BOOLEAN && cond.tipo != TipoDato::DESCONOCIDO) {
            reportarErrorSemantico("La condicion del 'for' debe ser de tipo boolean.", tokenCond.linea, tokenCond.columna);
        }
    }
    if (!coincidir("SIMBOLO", ";")) reportarError("Componente invalido o falta ';' tras condicion en 'for'.");

    string etiqFin = generador.nuevaEtiqueta();
    generador.emitir("if not " + cond.lugar + " goto " + etiqFin,
                     "Si " + cond.lugar + " es falso, salta a la etiqueta " + etiqFin + " (fin del bucle)");

    generador.iniciarBuffer();
    if (!comprobar("SIMBOLO", ")")) actualizacion();
    vector<InstruccionTAC> instruccionesActualizacion = generador.extraerBuffer();

    if (!coincidir("SIMBOLO", ")")) reportarError("Se esperaba ')' tras actualizacion del 'for'.");
    
    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en 'for'.");
        sincronizar();
        tablaSimbolos.salirAmbito();
        return;
    }

    nivelBucles++;
    pilaEtiquetasFinBucle.push_back(etiqFin);
    bloque();
    pilaEtiquetasFinBucle.pop_back();
    nivelBucles--;

    generador.agregarInstrucciones(instruccionesActualizacion);
    generador.emitir("goto " + etiqInicio, "Salta de vuelta al inicio del bucle");
    generador.emitirEtiqueta(etiqFin, "Etiqueta " + etiqFin + " marca el final del bucle");

    tablaSimbolos.salirAmbito();
}

void AnalizadorSintactico::sentenciaDoWhile() {
    coincidir("PALABRA_CLAVE", "do");

    string etiqInicio = generador.nuevaEtiqueta();
    generador.emitirEtiqueta(etiqInicio, "Etiqueta " + etiqInicio + " marca el inicio del bucle");

    if (!comprobar("SIMBOLO", "{")) {
        reportarError("Falta la llave de apertura '{' en la estructura 'do-while'.");
        sincronizar();
        return;
    }

    nivelBucles++;
    tablaSimbolos.entrarAmbito();
    bloque();
    tablaSimbolos.salirAmbito();
    nivelBucles--;

    Token tokenWhile = tokenActual();
    if (!coincidir("PALABRA_CLAVE", "while")) {
        reportarError("Se esperaba 'while' despues del bloque 'do'.");
        return;
    }

    if (!coincidir("SIMBOLO", "(")) {
        reportarError("Se esperaba '(' despues de 'while' en 'do-while'.");
    }

    ResultadoExpresion cond = expresion();
    if (cond.tipo != TipoDato::BOOLEAN && cond.tipo != TipoDato::DESCONOCIDO) {
        reportarErrorSemantico("La condicion del 'do-while' debe ser de tipo boolean (recibido: " + 
                               tipoDatoToString(cond.tipo) + ").", tokenWhile.linea, tokenWhile.columna);
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras la condicion del 'do-while'.");
    }

    if (!coincidir("SIMBOLO", ";")) {
        reportarError("Se esperaba ';' al final de la estructura 'do-while'.");
    }

    generador.emitir("if " + cond.lugar + " goto " + etiqInicio,
                     "Si " + cond.lugar + " es verdadero, salta de vuelta al inicio del bucle");
}

void AnalizadorSintactico::bloque() {
    if (!coincidir("SIMBOLO", "{")) {
        reportarError("Se esperaba '{' al inicio del bloque.");
        return;
    }

    while (!comprobar("SIMBOLO", "}") && !estaAlFinal()) {
        if (esTipoDato()) {
            declaracionMetodoOVariable();
        } else {
            sentencia();
        }
    }

    if (!coincidir("SIMBOLO", "}")) {
        reportarError("Falta la llave de cierre '}' en el bloque.");
    }
}

void AnalizadorSintactico::inicializacion() { expresion(); }
void AnalizadorSintactico::actualizacion() { expresion(); }

ResultadoExpresion AnalizadorSintactico::expresion() {
    Token tokenIzq = tokenActual();
    ResultadoExpresion izq = expresionSimple();

    while (comprobar("OPERADOR_RELACIONAL") || comprobar("OPERADOR_ASIGNACION") || comprobar("OPERADOR_LOGICO")) {
        Token tokenOp = tokenActual();
        avanzar();
        
        if (tokenOp.valor == "=") {
            if (nivelBucles > 0 && tokenIzq.tipo == "IDENTIFICADOR" &&
                comprobar("IDENTIFICADOR", tokenIzq.valor) &&
                indiceActual + 2 < tokens.size() &&
                tokens[indiceActual + 1].tipo == "OPERADOR_ARITMETICO" &&
                (tokens[indiceActual + 1].valor == "+" || tokens[indiceActual + 1].valor == "-") &&
                tokens[indiceActual + 2].valor == "1") {
                avanzar();
                string opArit = avanzar().valor;
                avanzar();
                tablaSimbolos.verificarAsignacion(tokenIzq.valor, TipoDato::INT, tokenIzq.linea, tokenIzq.columna);
                string accion = (opArit == "+") ? "Incrementa " : "Decrementa ";
                generador.emitir(tokenIzq.valor + " = " + tokenIzq.valor + " " + opArit + " 1",
                                 accion + tokenIzq.valor);
                izq = {TipoDato::INT, tokenIzq.valor};
                continue;
            }

            ResultadoExpresion der = expresionSimple();
            if (tokenIzq.tipo == "IDENTIFICADOR") {
                tablaSimbolos.verificarAsignacion(tokenIzq.valor, der.tipo, tokenIzq.linea, tokenIzq.columna);
            }
            if (!izq.lugar.empty() && !der.lugar.empty()) {
                string comentarioAsig;
                if (der.lugar == "return_value") {
                    comentarioAsig = "Asigna el valor retornado por el metodo a " + izq.lugar;
                } else {
                    comentarioAsig = "Asigna el valor de " + der.lugar + " a " + izq.lugar;
                }
                generador.emitir(izq.lugar + " = " + der.lugar, comentarioAsig);
            }
            izq = {der.tipo, izq.lugar};
        } 
        else if (tokenOp.tipo == "OPERADOR_RELACIONAL") {
            ResultadoExpresion der = expresionSimple();
            bool izqNum = (izq.tipo == TipoDato::INT || izq.tipo == TipoDato::FLOAT);
            bool derNum = (der.tipo == TipoDato::INT || der.tipo == TipoDato::FLOAT);
            
            if (!((izqNum && derNum) || (izq.tipo == der.tipo && izq.tipo != TipoDato::DESCONOCIDO))) {
                reportarErrorSemantico("No se pueden comparar tipos incompatibles (" + tokenOp.valor + ").", 
                                       tokenOp.linea, tokenOp.columna);
            }
            string temp = generador.nuevoTemporal();
            string exprCond = izq.lugar + " " + tokenOp.valor + " " + der.lugar;
            generador.emitir(temp + " = " + exprCond, 
                             temp + " es una etiqueta temporal para la condicion " + exprCond);
            izq = {TipoDato::BOOLEAN, temp};
        } 
        else if (tokenOp.tipo == "OPERADOR_LOGICO") {
            ResultadoExpresion der = expresionSimple();
            if (izq.tipo != TipoDato::BOOLEAN || der.tipo != TipoDato::BOOLEAN) {
                reportarErrorSemantico("Operador logico '" + tokenOp.valor + "' requiere operandos booleanos.", 
                                       tokenOp.linea, tokenOp.columna);
            }
            string temp = generador.nuevoTemporal();
            string exprLog = izq.lugar + " " + tokenOp.valor + " " + der.lugar;
            generador.emitir(temp + " = " + exprLog,
                             temp + " es una etiqueta temporal para la operacion logica " + exprLog);
            izq = {TipoDato::BOOLEAN, temp};
        }
    }
    return izq;
}

ResultadoExpresion AnalizadorSintactico::expresionSimple() {
    ResultadoExpresion base = {TipoDato::DESCONOCIDO, ""};

    if (comprobar("SIMBOLO", "(")) {
        avanzar();
        base = expresion();
        if (!coincidir("SIMBOLO", ")")) {
            reportarError("Se esperaba ')' para cerrar la expresion.");
        }
    }
    else if (comprobar("IDENTIFICADOR")) {
        Token idToken = tokenActual();
        avanzar();

        if (comprobar("SIMBOLO", "(")) {
            base = llamadaFuncion(idToken.valor, "", idToken.linea, idToken.columna);
        } 
        else if (coincidir("SIMBOLO", ".")) {
            Simbolo* simObj = tablaSimbolos.buscarSimbolo(idToken.valor, idToken.linea, idToken.columna);
            Token miembroToken = tokenActual();
            if (!coincidir("IDENTIFICADOR")) {
                reportarError("Se esperaba el nombre de un metodo o atributo despues de '.'.");
                return {TipoDato::DESCONOCIDO, ""};
            }

            if (simObj && simObj->tipo != TipoDato::OBJETO) {
                reportarErrorSemantico("El identificador '" + idToken.valor + "' no es un objeto.", 
                                       idToken.linea, idToken.columna);
            }

            if (comprobar("SIMBOLO", "(")) {
                base = llamadaFuncion(miembroToken.valor, idToken.valor, miembroToken.linea, miembroToken.columna);
            } else {
                base = {TipoDato::INT, idToken.valor + "." + miembroToken.valor};
            }
        }
        else {
            Simbolo* sim = tablaSimbolos.buscarSimbolo(idToken.valor, idToken.linea, idToken.columna);
            if (sim) base.tipo = sim->tipo;
            base.lugar = idToken.valor;
        }
    } 
    else if (comprobar("ENTERO")) {
        Token t = avanzar();
        base = {TipoDato::INT, t.valor};
    } 
    else if (comprobar("DECIMAL")) {
        Token t = avanzar();
        base = {TipoDato::FLOAT, t.valor};
    } 
    else if (comprobar("CADENA")) {
        Token t = avanzar();
        base = {TipoDato::STRING, t.valor};
    }
    else if (comprobar("CARACTER")) {
        Token t = avanzar();
        base = {TipoDato::CHAR, t.valor};
    }
    else if (comprobar("PALABRA_CLAVE", "true") || comprobar("PALABRA_CLAVE", "false")) {
        Token t = avanzar();
        base = {TipoDato::BOOLEAN, t.valor};
    }
    else if (esTipoDato()) {
        Token tokenTipo = tokenActual();
        avanzar(); 
        
        if (comprobar("IDENTIFICADOR")) {
            Token tokenVar = tokenActual();
            avanzar();
            
            TipoDato nuevoTipo = tipoDatoDesdeString(tokenTipo.valor);
            tablaSimbolos.insertarSimbolo(tokenVar.valor, nuevoTipo, tokenVar.linea, tokenVar.columna);
            base = {nuevoTipo, tokenVar.valor};

            if (coincidir("OPERADOR_ASIGNACION", "=")) {
                ResultadoExpresion asignado = expresion();
                tablaSimbolos.verificarAsignacion(tokenVar.valor, asignado.tipo, tokenVar.linea, tokenVar.columna);
                generador.emitir(tokenVar.valor + " = " + asignado.lugar, "Inicializacion de " + tokenVar.valor);
            }
        }
        return base;
    } 
    else {
        reportarError("Expresion o componente sintactico invalido ('" + tokenActual().valor + "').");
        return {TipoDato::DESCONOCIDO, ""};
    }

    while (comprobar("OPERADOR_ARITMETICO")) {
        Token tokenOp = tokenActual();
        avanzar();

        if (tokenOp.valor == "/" && (tokenActual().valor == "0" || tokenActual().valor == "0.0")) {
            reportarErrorSemantico("Division por cero detectada.", tokenOp.linea, tokenOp.columna);
        }

        ResultadoExpresion der = {TipoDato::DESCONOCIDO, ""};
        
        if (comprobar("IDENTIFICADOR")) {
            Token idDer = tokenActual();
            avanzar();
            if (comprobar("SIMBOLO", "(")) {
                der = llamadaFuncion(idDer.valor, "", idDer.linea, idDer.columna);
            } else {
                Simbolo* sim = tablaSimbolos.buscarSimbolo(idDer.valor, idDer.linea, idDer.columna);
                if (sim) der.tipo = sim->tipo;
                der.lugar = idDer.valor;
            }
        } else if (comprobar("ENTERO")) {
            Token t = avanzar();
            der = {TipoDato::INT, t.valor};
        } else if (comprobar("DECIMAL")) {
            Token t = avanzar();
            der = {TipoDato::FLOAT, t.valor};
        } else if (comprobar("CADENA")) {
            Token t = avanzar();
            der = {TipoDato::STRING, t.valor};
        } else {
            reportarError("Se esperaba un numero o variable despues del operador aritmetico.");
        }

        bool izqNum = (base.tipo == TipoDato::INT || base.tipo == TipoDato::FLOAT);
        bool derNum = (der.tipo == TipoDato::INT || der.tipo == TipoDato::FLOAT);

        if (!izqNum || !derNum) {
            reportarErrorSemantico("No se puede aplicar el operador aritmetico '" + tokenOp.valor + 
                                   "' a tipos incompatibles.", tokenOp.linea, tokenOp.columna);
            base.tipo = TipoDato::DESCONOCIDO;
        } else {
            if (base.tipo == TipoDato::FLOAT || der.tipo == TipoDato::FLOAT) {
                base.tipo = TipoDato::FLOAT;
            }
        }

        string temp = generador.nuevoTemporal();
        string exprArit = base.lugar + " " + tokenOp.valor + " " + der.lugar;
        generador.emitir(temp + " = " + exprArit, 
                         temp + " es una etiqueta temporal para la operacion " + exprArit);
        base.lugar = temp;
    }

    if (comprobar("OPERADOR_INCREMENTO")) {
        Token tokenInc = avanzar();
        string op = (tokenInc.valor == "++") ? "+" : "-";
        string accion = (tokenInc.valor == "++") ? "Incrementa " : "Decrementa ";
        generador.emitir(base.lugar + " = " + base.lugar + " " + op + " 1", accion + base.lugar);
    }

    return base;
}

ResultadoExpresion AnalizadorSintactico::llamadaFuncion(const string& nombre, const string& objetoReceptor, int linea, int columna) {
    coincidir("SIMBOLO", "(");

    SimboloFuncion* funcion = tablaSimbolos.buscarFuncion(nombre, linea, columna);
    vector<ResultadoExpresion> args;

    if (!comprobar("SIMBOLO", ")")) {
        do {
            args.push_back(expresion());
        } while (coincidir("SIMBOLO", ","));
    }

    if (!coincidir("SIMBOLO", ")")) {
        reportarError("Se esperaba ')' tras los argumentos del metodo.");
    }

    for (size_t i = 0; i < args.size(); ++i) {
        string tempParam = generador.nuevoTemporal();
        generador.emitir(tempParam + " = " + args[i].lugar, 
                         tempParam + " es una etiqueta temporal para el parametro " + to_string(i + 1));
    }

    if (funcion == nullptr) {
        return {TipoDato::DESCONOCIDO, "return_value"};
    }

    string etiqDestino = funcion->etiqueta.empty() ? nombre : funcion->etiqueta;
    if (!objetoReceptor.empty()) {
        generador.emitir("call " + etiqDestino, 
                         "Llama al metodo " + nombre + " del objeto " + objetoReceptor);
    } else {
        generador.emitir("call " + etiqDestino, 
                         "Llama al metodo " + nombre + " definido en la etiqueta " + etiqDestino);
    }

    if (args.size() != funcion->tiposParametros.size()) {
        reportarErrorSemantico("Llamada al metodo '" + nombre + "' con " + 
                               to_string(args.size()) + " argumentos en lugar de " + 
                               to_string(funcion->tiposParametros.size()) + ".", linea, columna);
        return {funcion->tipoRetorno, "return_value"};
    }

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i].tipo != funcion->tiposParametros[i] && args[i].tipo != TipoDato::DESCONOCIDO) {
            reportarErrorSemantico("Argumento " + to_string(i + 1) + " en llamada a '" + nombre + 
                                   "' es de tipo '" + tipoDatoToString(args[i].tipo) + 
                                   "', se esperaba '" + tipoDatoToString(funcion->tiposParametros[i]) + "'.", 
                                   linea, columna);
        }
    }

    return {funcion->tipoRetorno, "return_value"};
}
```

---

### **3.7. `AnalizadorLexicoCPP.cpp` (`main`)**
```cpp
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
```

---

## **4. Salida de Ejecución del Programa**

```text
======================================================================
  INTEGRANTES DEL EQUIPO (INICIO DE EJECUCION):
  1. Anguiano Garcia Angel Yahir Guadalupe
  2. Figueroa Robles Axel Israel
  3. Molina Alvarado Alvaro Moises
  4. Torres Martinez Miguel Angel
======================================================================

======================================================================
  ACTIVIDAD 11.- CODIGO INTERMEDIO PARA CONTROL DE FLUJO
======================================================================

**********************************************************************
  PRUEBA 1 (VALIDA): CONDICIONALES (if-else y switch)
**********************************************************************

[1] CODIGO FUENTE DE ENTRADA:
----------------------------------------------------------------------

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
    
----------------------------------------------------------------------

[2] FASE LEXICA:
  [+] Analisis lexico completado sin errores (74 tokens generados).

[3] FASE SINTACTICA Y SEMANTICA:
  [+] Validacion sintactica y semantica exitosa (0 errores).

[4] CODIGO INTERMEDIO GENERADO (REPRESENTACION DE 3 DIRECCIONES):
----------------------------------------------------------------------
  x = 10                  # Asigna el valor 10 a x
  y = 5                   # Asigna el valor 5 a y
  z = 0                   # Asigna el valor 0 a z
  t1 = x > y              # t1 es una etiqueta temporal para la condicion x > y
  if t1 goto L1           # Si t1 es verdadero, salta a la etiqueta L1
  z = y                   # Asigna el valor de y a z
  goto L2                 # Salta a la etiqueta L2 despues del bloque else
  L1:                     # Etiqueta L1 para el bloque if
  z = x                   # Asigna el valor de x a z
  L2:                     # Etiqueta L2 marca el final de la estructura condicional
  n = 1                   # Asigna el valor 1 a n
  result = ""             # Asigna el valor "" a result
  t2 = n                  # t2 es una etiqueta temporal para el valor de n
  if t2 == 1 goto L3      # Si t2 es 1, salta a la etiqueta L3
  if t2 == 2 goto L4      # Si t2 es 2, salta a la etiqueta L4
  goto L5                 # Si t2 no coincide con ningun caso, salta a la etiqueta L5
  L3:                     # Etiqueta L3 para el caso 1
  result = "Uno"          # Asigna el valor de "Uno" a result
  goto L6                 # Salta a la etiqueta L6 despues de manejar el caso 1
  L4:                     # Etiqueta L4 para el caso 2
  result = "Dos"          # Asigna el valor de "Dos" a result
  goto L6                 # Salta a la etiqueta L6 despues de manejar el caso 2
  L5:                     # Etiqueta L5 para el caso default
  result = "Otro"         # Asigna el valor de "Otro" a result
  L6:                     # Etiqueta L6 marca el final de la estructura switch
----------------------------------------------------------------------


**********************************************************************
  PRUEBA 2 (VALIDA): BUCLES (while, for y do-while)
**********************************************************************

[1] CODIGO FUENTE DE ENTRADA:
----------------------------------------------------------------------

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
    
----------------------------------------------------------------------

[2] FASE LEXICA:
  [+] Analisis lexico completado sin errores (67 tokens generados).

[3] FASE SINTACTICA Y SEMANTICA:
  [+] Validacion sintactica y semantica exitosa (0 errores).

[4] CODIGO INTERMEDIO GENERADO (REPRESENTACION DE 3 DIRECCIONES):
----------------------------------------------------------------------
  i = 0                   # Asigna el valor 0 a i
  L1:                     # Etiqueta L1 marca el inicio del bucle
  t1 = i < 10             # t1 es una etiqueta temporal para la condicion i < 10
  if not t1 goto L2       # Si t1 es falso, salta a la etiqueta L2 (fin del bucle)
  i = i + 1               # Incrementa i
  goto L1                 # Salta de vuelta al inicio del bucle
  L2:                     # Etiqueta L2 marca el final del bucle
  sum = 0                 # Asigna el valor 0 a sum
  k = 0                   # Inicializacion de k
  L3:                     # Etiqueta L3 marca el inicio del bucle
  t2 = k < 5              # t2 es una etiqueta temporal para la condicion k < 5
  if not t2 goto L4       # Si t2 es falso, salta a la etiqueta L4 (fin del bucle)
  t3 = sum + k            # t3 es una etiqueta temporal para la operacion sum + k
  sum = t3                # Asigna el valor de t3 a sum
  k = k + 1               # Incrementa k
  goto L3                 # Salta de vuelta al inicio del bucle
  L4:                     # Etiqueta L4 marca el final del bucle
  count = 0               # Asigna el valor 0 a count
  L5:                     # Etiqueta L5 marca el inicio del bucle
  count = count + 1       # Incrementa count
  t4 = count < 10         # t4 es una etiqueta temporal para la condicion count < 10
  if t4 goto L5           # Si t4 es verdadero, salta de vuelta al inicio del bucle
----------------------------------------------------------------------


**********************************************************************
  PRUEBA 3 (VALIDA): METODOS, CLASES Y OBJETOS
**********************************************************************

[1] CODIGO FUENTE DE ENTRADA:
----------------------------------------------------------------------

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
    
----------------------------------------------------------------------

[2] FASE LEXICA:
  [+] Analisis lexico completado sin errores (63 tokens generados).

[3] FASE SINTACTICA Y SEMANTICA:
  [+] Validacion sintactica y semantica exitosa (0 errores).

[4] CODIGO INTERMEDIO GENERADO (REPRESENTACION DE 3 DIRECCIONES):
----------------------------------------------------------------------
  # Definicion del metodo suma
  L1:                     # Etiqueta L1 marca el inicio del metodo suma
  t1 = a + b              # t1 es una etiqueta temporal para la operacion a + b
  return t1               # Retorna el valor de t1
  result = 0              # Asigna el valor 0 a result
  t2 = 5                  # t2 es una etiqueta temporal para el parametro 1
  t3 = 3                  # t3 es una etiqueta temporal para el parametro 2
  call L1                 # Llama al metodo suma definido en la etiqueta L1
  result = return_value   # Asigna el valor retornado por el metodo a result
  # Definicion de la clase Persona
  L2:                     # Etiqueta L2 marca el inicio del metodo celebrarCumpleaños
  t4 = edad + 1           # t4 es una etiqueta temporal para la operacion edad + 1
  edad = t4               # Asigna el valor de t4 a edad
  # Creacion del objeto Persona
  obj = create Persona    # Representa la creacion del objeto p
  call L2                 # Llama al metodo celebrarCumpleaños del objeto p
----------------------------------------------------------------------


**********************************************************************
  PRUEBA 4 (INVALIDA): DETECCION DE ERRORES EN TODAS LAS REGLAS
**********************************************************************

[1] CODIGO FUENTE DE ENTRADA:
----------------------------------------------------------------------

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
    
----------------------------------------------------------------------

[2] FASE LEXICA:
  [+] Analisis lexico completado sin errores (161 tokens generados).

[3] FASE SINTACTICA Y SEMANTICA:
  [!] Se detectaron 11 error(es):

      >> [Semantico] Linea 4, Columna 17: El metodo 'obtenerEdad' debe retornar 'int' pero retorno 'string'.
      >> [Semantico] Linea 15, Columna 17: La condicion del 'if' debe ser de tipo boolean (recibido: string).
      >> [Semantico] Linea 21, Columna 26: El tipo del 'case' (string) es incompatible con la expresion del 'switch' (int).
      >> [Semantico] Linea 27, Columna 26: Valor de 'case' duplicado (1) en 'switch'.
      >> [Semantico] Linea 33, Columna 17: La sentencia 'break' solo puede usarse dentro de un bucle o estructura 'switch'.
      >> [Semantico] Linea 36, Columna 17: La condicion del 'while' debe ser de tipo boolean (recibido: int).
      >> [Semantico] Linea 41, Columna 33: La condicion del 'for' debe ser de tipo boolean.
      >> [Semantico] Linea 48, Columna 19: La condicion del 'do-while' debe ser de tipo boolean (recibido: int).
      >> [Semantico] Linea 51, Columna 29: Argumento 2 en llamada a 'multiplicar' es de tipo 'string', se esperaba 'int'.
      >> [Semantico] Linea 52, Columna 29: Llamada al metodo 'multiplicar' con 3 argumentos en lugar de 2.
      >> [Semantico] Linea 55, Columna 18: La clase 'Vehiculo' no ha sido declarada.

[4] GENERACION DE CODIGO INTERMEDIO (TRES DIRECCIONES):
----------------------------------------------------------------------
  [X] Generacion abortada: Se requiere corregir los errores previos.
----------------------------------------------------------------------

======================================================================
  INTEGRANTES DEL EQUIPO (FINAL DE EJECUCION):
  1. Anguiano Garcia Angel Yahir Guadalupe
  2. Figueroa Robles Axel Israel
  3. Molina Alvarado Alvaro Moises
  4. Torres Martinez Miguel Angel
======================================================================
```

---

## **5. Referencias Bibliográficas**
- Aho, A. V., Lam, M. S., Sethi, R., & Ullman, J. D. (2006). *Compilers: Principles, Techniques, and Tools* (2nd ed.). Pearson/Addison Wesley.
- Appel, A. W., & Palsberg, J. (2004). *Modern Compiler Implementation in Java* (2nd ed.). Cambridge University Press.
- Cooper, K. D., & Torczon, L. (2011). *Engineering a Compiler* (2nd ed.). Morgan Kaufmann.
