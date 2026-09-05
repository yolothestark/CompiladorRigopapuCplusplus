#include "AnalizadorLexico.h"
#include <regex>
#include <unordered_set>
#include <cctype>

vector<Token> AnalizadorLexico::analizar(const string& codigo)
{
    vector<Token> resultado;

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

    regex identificador("[a-zA-Z_][a-zA-Z0-9_]*");
    regex decimal("[0-9]+\\.[0-9]+");
    regex entero("[0-9]+");
    regex cadena("\"([^\"\\\\]|\\\\.)*\"");
    regex caracter("'([^'\\\\]|\\\\.)'");

    size_t posicion = 0;

    while (posicion < codigo.length())
    {
        if (isspace(codigo[posicion]))
        {
            posicion++;
            continue;
        }

        if (posicion + 1 < codigo.length() &&
            codigo[posicion] == '/' &&
            codigo[posicion + 1] == '/')
        {
            posicion += 2;

            while (posicion < codigo.length() &&
                codigo[posicion] != '\n')
            {
                posicion++;
            }

            continue;
        }

        if (posicion + 1 < codigo.length() &&
            codigo[posicion] == '/' &&
            codigo[posicion + 1] == '*')
        {
            posicion += 2;

            while (posicion + 1 < codigo.length() &&
                !(codigo[posicion] == '*' &&
                    codigo[posicion + 1] == '/'))
            {
                posicion++;
            }

            if (posicion + 1 < codigo.length())
            {
                posicion += 2;
            }

            continue;
        }

        smatch coincidencia;

        string restante = codigo.substr(posicion);

        if (regex_search(restante, coincidencia, cadena,
            regex_constants::match_continuous))
        {
            resultado.push_back({
                "CADENA",
                coincidencia.str()
                });

            posicion += coincidencia.length();
            continue;
        }

        if (regex_search(restante, coincidencia, caracter,
            regex_constants::match_continuous))
        {
            resultado.push_back({
                "CARACTER",
                coincidencia.str()
                });

            posicion += coincidencia.length();
            continue;
        }

        if (regex_search(restante, coincidencia, decimal,
            regex_constants::match_continuous))
        {
            resultado.push_back({
                "DECIMAL",
                coincidencia.str()
                });

            posicion += coincidencia.length();
            continue;
        }

        if (regex_search(restante, coincidencia, entero,
            regex_constants::match_continuous))
        {
            resultado.push_back({
                "ENTERO",
                coincidencia.str()
                });

            posicion += coincidencia.length();
            continue;
        }

        if (regex_search(restante, coincidencia, identificador,
            regex_constants::match_continuous))
        {
            string palabra = coincidencia.str();

            if (palabrasClave.count(palabra))
            {
                resultado.push_back({
                    "PALABRA_CLAVE",
                    palabra
                    });
            }
            else
            {
                resultado.push_back({
                    "IDENTIFICADOR",
                    palabra
                    });
            }

            posicion += coincidencia.length();
            continue;
        }

        if (posicion + 1 < codigo.length())
        {
            string operador = codigo.substr(posicion, 2);

            if (operador == "==" ||
                operador == "!=" ||
                operador == "<=" ||
                operador == ">=")
            {
                resultado.push_back({
                    "OPERADOR_RELACIONAL",
                    operador
                    });

                posicion += 2;
                continue;
            }

            if (operador == "&&" ||
                operador == "||")
            {
                resultado.push_back({
                    "OPERADOR_LOGICO",
                    operador
                    });

                posicion += 2;
                continue;
            }

            if (operador == "++" ||
                operador == "--")
            {
                resultado.push_back({
                    "OPERADOR_INCREMENTO",
                    operador
                    });

                posicion += 2;
                continue;
            }

            if (operador == "+=" ||
                operador == "-=" ||
                operador == "*=" ||
                operador == "/=")
            {
                resultado.push_back({
                    "OPERADOR_ASIGNACION",
                    operador
                    });

                posicion += 2;
                continue;
            }

            if (operador == "::")
            {
                resultado.push_back({
                    "OPERADOR_RESOLUCION",
                    operador
                    });

                posicion += 2;
                continue;
            }
        }

        char simbolo = codigo[posicion];

        if (simbolo == '+' ||
            simbolo == '-' ||
            simbolo == '*' ||
            simbolo == '/' ||
            simbolo == '%')
        {
            resultado.push_back({
                "OPERADOR_ARITMETICO",
                string(1, simbolo)
                });

            posicion++;
            continue;
        }

        if (simbolo == '<' ||
            simbolo == '>')
        {
            resultado.push_back({
                "OPERADOR_RELACIONAL",
                string(1, simbolo)
                });

            posicion++;
            continue;
        }

        if (simbolo == '!')
        {
            resultado.push_back({
                "OPERADOR_LOGICO",
                string(1, simbolo)
                });

            posicion++;
            continue;
        }

        if (simbolo == '=')
        {
            resultado.push_back({
                "OPERADOR_ASIGNACION",
                string(1, simbolo)
                });

            posicion++;
            continue;
        }

        if (simbolo == '(' ||
            simbolo == ')' ||
            simbolo == '{' ||
            simbolo == '}' ||
            simbolo == '[' ||
            simbolo == ']' ||
            simbolo == ',' ||
            simbolo == ';' ||
            simbolo == '.' ||
            simbolo == ':')
        {
            resultado.push_back({
                "SIMBOLO",
                string(1, simbolo)
                });

            posicion++;
            continue;
        }

        resultado.push_back({
            "DESCONOCIDO",
            string(1, simbolo)
            });

        posicion++;
    }

    return resultado;
}