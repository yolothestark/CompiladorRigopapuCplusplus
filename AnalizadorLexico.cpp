// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Torres Martinez Miguel Angel
// 4. Molina Alvarado Alvaro Moises

#include "AnalizadorLexico.h"
#include <regex>
#include <unordered_set>
#include <cctype>

const vector<ErrorLexico>& AnalizadorLexico::obtenerErrores() const {
    return errores;
}

bool AnalizadorLexico::tieneErrores() const {
    return !errores.empty();
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
        "nullptr", "private", "protected", "public", "return",
        "short", "signed", "sizeof", "static", "struct",
        "switch", "this", "throw", "true", "try",
        "typedef", "using", "virtual", "void", "volatile",
        "while"
    };

    // Regex originales
    regex identificador("[a-zA-Z_][a-zA-Z0-9_]*");
    regex decimal("[0-9]+\\.[0-9]+");
    regex entero("[0-9]+");
    regex cadena("\"([^\"\\\\]|\\\\.)*\"");
    regex caracter("'([^'\\\\]|\\\\.)'");

    // Regex para errores léxicos específicos
    // Identificador mal formado: empieza con un número y tiene letras (ej. 123invalido)
    regex err_identificador("[0-9]+[a-zA-Z_][a-zA-Z0-9_]*");
    // Número con múltiples puntos (ej. 12.34.56)
    regex err_decimal_multiple("[0-9]+\\.[0-9]+(\\.[0-9]+)+");
    
    size_t posicion = 0;
    int linea_actual = 1;
    int columna_actual = 1;

    // Función lambda para avanzar la posición y actualizar línea y columna
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
        if (isspace(codigo[posicion]))
        {
            string espacio = string(1, codigo[posicion]);
            avanzar(1, espacio);
            continue;
        }

        if (posicion + 1 < codigo.length() &&
            codigo[posicion] == '/' &&
            codigo[posicion + 1] == '/')
        {
            int start = posicion;
            while (posicion < codigo.length() && codigo[posicion] != '\n') {
                posicion++;
            }
            string consumido = codigo.substr(start, posicion - start);
            
            // Revertir y usar avanzar
            posicion = start;
            avanzar(consumido.length(), consumido);
            continue;
        }

        if (posicion + 1 < codigo.length() &&
            codigo[posicion] == '/' &&
            codigo[posicion + 1] == '*')
        {
            int start = posicion;
            posicion += 2;
            while (posicion + 1 < codigo.length() &&
                !(codigo[posicion] == '*' && codigo[posicion + 1] == '/'))
            {
                posicion++;
            }

            if (posicion + 1 < codigo.length())
            {
                posicion += 2;
            }
            
            string consumido = codigo.substr(start, posicion - start);
            posicion = start;
            avanzar(consumido.length(), consumido);
            continue;
        }

        smatch coincidencia;
        string restante = codigo.substr(posicion);

        // Detección de cadena mal formada (manual para atrapar falta de cierre antes de salto de línea)
        if (codigo[posicion] == '"') {
            size_t len = 1;
            bool cerrado = false;
            while (posicion + len < codigo.length() && codigo[posicion + len] != '\n') {
                if (codigo[posicion + len] == '\\') {
                    len += 2; // saltar caracter escapado
                    continue;
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
                errores.push_back({linea_actual, columna_actual, "Error Léxico", "Cadena mal formada (falta cierre de comillas)", valor_err});
                avanzar(len, valor_err);
                continue;
            }
        }

        // Primero verificar errores con expresiones regulares
        if (regex_search(restante, coincidencia, err_decimal_multiple, regex_constants::match_continuous))
        {
            errores.push_back({linea_actual, columna_actual, "Error Léxico", "Literal numérico incorrecto (múltiples puntos decimales)", coincidencia.str()});
            avanzar(coincidencia.length(), coincidencia.str());
            continue;
        }

        if (regex_search(restante, coincidencia, err_identificador, regex_constants::match_continuous))
        {
            errores.push_back({linea_actual, columna_actual, "Error Léxico", "Identificador mal formado o literal numérico incorrecto", coincidencia.str()});
            avanzar(coincidencia.length(), coincidencia.str());
            continue;
        }

        // Si no es error, procesar tokens válidos
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

        if (regex_search(restante, coincidencia, identificador, regex_constants::match_continuous))
        {
            string palabra = coincidencia.str();
            if (palabrasClave.count(palabra))
            {
                resultado.push_back({"PALABRA_CLAVE", palabra, linea_actual, columna_actual});
            }
            else
            {
                resultado.push_back({"IDENTIFICADOR", palabra, linea_actual, columna_actual});
            }
            avanzar(coincidencia.length(), coincidencia.str());
            continue;
        }

        // Operadores de más de un caracter
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

        // Símbolos de 1 caracter
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

        // Si llega aquí, es un caracter no reconocido
        errores.push_back({linea_actual, columna_actual, "Error Léxico", "Caracter no reconocido", str_simbolo});
        avanzar(1, str_simbolo);
    }

    return resultado;
}