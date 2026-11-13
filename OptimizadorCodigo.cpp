// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#include "OptimizadorCodigo.hpp"
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace {

// Estructura auxiliar para analizar sintácticamente cada tupla de código intermedio (TAC)
struct InfoTupla {
    bool esSeccion = false;
    bool esEtiqueta = false;
    bool esGotoIncondicional = false;
    bool esSaltoCondicional = false;
    bool esReturn = false;
    bool esCall = false;
    bool esCreacionObjeto = false;
    bool esAsignacion = false;
    bool esCopiaSimple = false;
    bool esOperacionBinaria = false;

    string etiqueta;          // Nombre de etiqueta en declaracion (L1) o destino de salto
    string lhs;               // Variable destino en asignacion (ej. t1, temp, result)
    string op1;               // Primer operando en asignacion
    string operador;          // Operador en calculo binario (+, -, *, /, >, <, ==, etc.)
    string op2;               // Segundo operando en calculo binario
    vector<string> leidos;    // Variables leídas como operandos en esta instrucción
    vector<string> tokens;    // Tokens separados de la instrucción
};

vector<string> tokenizarTAC(const string& codigo) {
    vector<string> tokens;
    size_t i = 0;
    size_t n = codigo.size();

    while (i < n) {
        if (isspace(static_cast<unsigned char>(codigo[i]))) {
            i++;
            continue;
        }
        if (codigo[i] == '"' || codigo[i] == '\'') {
            char quote = codigo[i];
            size_t start = i++;
            while (i < n && codigo[i] != quote) {
                if (codigo[i] == '\\' && i + 1 < n) i++;
                i++;
            }
            if (i < n) i++;
            tokens.push_back(codigo.substr(start, i - start));
        } else {
            size_t start = i;
            while (i < n && !isspace(static_cast<unsigned char>(codigo[i]))) {
                i++;
            }
            tokens.push_back(codigo.substr(start, i - start));
        }
    }
    return tokens;
}

bool esPalabraReservadaTAC(const string& s) {
    static const unordered_set<string> reservadas = {
        "if", "not", "goto", "return", "call", "create",
        "true", "false", "return_value"
    };
    return reservadas.count(s) > 0;
}

bool esIdentificadorVariable(const string& s) {
    if (s.empty() || esPalabraReservadaTAC(s)) return false;
    unsigned char c0 = static_cast<unsigned char>(s[0]);
    if (!(isalpha(c0) || c0 == '_' || c0 >= 128)) return false;
    if (s.back() == ':') return false;
    return true;
}

bool esVariableSalidaFinal(const string& s) {
    static const unordered_set<string> salidas = {
        "result", "resultado", "res", "total", "salida", "final", "ans", "obj"
    };
    return salidas.count(s) > 0;
}

InfoTupla analizarTupla(const InstruccionTAC& inst) {
    InfoTupla info;
    if (inst.esComentarioSeccion || (!inst.codigo.empty() && inst.codigo[0] == '#')) {
        info.esSeccion = true;
        return info;
    }

    info.tokens = tokenizarTAC(inst.codigo);
    const auto& t = info.tokens;
    if (t.empty()) {
        info.esSeccion = true;
        return info;
    }

    // 1. Declaración de etiqueta: L_X:
    if (t.size() == 1 && t[0].size() > 1 && t[0].back() == ':') {
        info.esEtiqueta = true;
        info.etiqueta = t[0].substr(0, t[0].size() - 1);
        return info;
    }

    // 2. Salto incondicional: goto L_X
    if (t.size() == 2 && t[0] == "goto") {
        info.esGotoIncondicional = true;
        info.etiqueta = t[1];
        return info;
    }

    // 3. Salto condicional: if ... goto L_X
    if (t.size() >= 4 && t[0] == "if" && t[t.size() - 2] == "goto") {
        info.esSaltoCondicional = true;
        info.etiqueta = t.back();
        for (size_t i = 1; i + 2 < t.size(); ++i) {
            if (esIdentificadorVariable(t[i])) {
                info.leidos.push_back(t[i]);
            }
        }
        return info;
    }

    // 4. Sentencia return
    if (t[0] == "return") {
        info.esReturn = true;
        if (t.size() >= 2 && esIdentificadorVariable(t[1])) {
            info.op1 = t[1];
            info.leidos.push_back(t[1]);
        }
        return info;
    }

    // 5. Llamada a método: call L_X
    if (t.size() >= 2 && t[0] == "call") {
        info.esCall = true;
        info.etiqueta = t[1];
        return info;
    }

    // 6. Instrucción de asignación: lhs = ...
    if (t.size() >= 3 && t[1] == "=") {
        info.esAsignacion = true;
        info.lhs = t[0];

        if (t.size() == 4 && t[2] == "create") {
            info.esCreacionObjeto = true;
            info.op1 = t[3];
            return info;
        }

        if (t.size() == 3) {
            info.esCopiaSimple = true;
            info.op1 = t[2];
            if (esIdentificadorVariable(info.op1)) {
                info.leidos.push_back(info.op1);
            }
            return info;
        }

        if (t.size() == 5) {
            info.esOperacionBinaria = true;
            info.op1 = t[2];
            info.operador = t[3];
            info.op2 = t[4];
            if (esIdentificadorVariable(info.op1)) {
                info.leidos.push_back(info.op1);
            }
            if (esIdentificadorVariable(info.op2)) {
                info.leidos.push_back(info.op2);
            }
            return info;
        }

        // Asignación general de más tokens
        for (size_t i = 2; i < t.size(); ++i) {
            if (esIdentificadorVariable(t[i])) {
                info.leidos.push_back(t[i]);
            }
        }
    }

    return info;
}

// Reemplaza lecturas de 'varAntigua' por 'varNueva' en una instrucción
bool reemplazarOperandoLeido(InstruccionTAC& inst, const string& varAntigua, const string& varNueva) {
    InfoTupla info = analizarTupla(inst);
    if (info.esSeccion || info.esEtiqueta || info.esGotoIncondicional || info.esCall) {
        return false;
    }

    bool cambiado = false;
    if (info.esAsignacion) {
        for (size_t i = 2; i < info.tokens.size(); ++i) {
            if (info.tokens[i] == varAntigua) {
                info.tokens[i] = varNueva;
                cambiado = true;
            }
        }
    } else {
        for (size_t i = 0; i < info.tokens.size(); ++i) {
            if (info.esSaltoCondicional && i >= info.tokens.size() - 2) break;
            if (info.tokens[i] == varAntigua) {
                info.tokens[i] = varNueva;
                cambiado = true;
            }
        }
    }

    if (cambiado) {
        string nuevoCodigo;
        for (size_t i = 0; i < info.tokens.size(); ++i) {
            if (i > 0) nuevoCodigo += " ";
            nuevoCodigo += info.tokens[i];
        }
        inst.codigo = nuevoCodigo;
    }
    return cambiado;
}

} // namespace

// ============================================================================
// REGLA 1: Identificación de Código Muerto (Saltos Incondicionales goto)
// ============================================================================
void OptimizadorCodigo::eliminarCodigoMuertoSaltos(vector<InstruccionTAC>& instrucciones) {
    // Contabilizar referencias a etiquetas en saltos condicionales y llamadas
    unordered_set<string> etiquetasReferenciadasCondicionalmente;
    for (const auto& inst : instrucciones) {
        InfoTupla info = analizarTupla(inst);
        if (info.esSaltoCondicional || info.esCall) {
            etiquetasReferenciadasCondicionalmente.insert(info.etiqueta);
        }
    }

    vector<InstruccionTAC> resultado;
    size_t n = instrucciones.size();
    size_t i = 0;

    while (i < n) {
        resultado.push_back(instrucciones[i]);
        InfoTupla info = analizarTupla(instrucciones[i]);

        if (info.esGotoIncondicional) {
            const string& etiqDestino = info.etiqueta;

            // Buscar si la declaración de la etiqueta destino (ej. L_X:) está más adelante
            size_t posEtiqueta = n;
            for (size_t k = i + 1; k < n; ++k) {
                InfoTupla infoK = analizarTupla(instrucciones[k]);
                if (infoK.esEtiqueta && infoK.etiqueta == etiqDestino) {
                    posEtiqueta = k;
                    break;
                }
            }

            if (posEtiqueta < n) {
                // Eliminar instrucciones siguientes hasta encontrar la etiqueta correspondiente L_X:
                size_t j = i + 1;
                while (j < posEtiqueta) {
                    InfoTupla infoJ = analizarTupla(instrucciones[j]);
                    // Si aparece otra etiqueta activa (ej. rama de if-else o case), detener el corte
                    if (infoJ.esEtiqueta && etiquetasReferenciadasCondicionalmente.count(infoJ.etiqueta)) {
                        break;
                    }
                    if (!infoJ.esSeccion) {
                        bitacora.push_back({
                            "Regla 1: Codigo Muerto (Saltos)",
                            instrucciones[j].codigo,
                            "Eliminada tras 'goto " + etiqDestino + "' (inaccesible antes de '" + etiqDestino + ":')"
                        });
                    }
                    j++;
                }
                i = j;
                continue;
            }
        }
        i++;
    }

    instrucciones = std::move(resultado);
}

// ============================================================================
// REGLA 3: Eliminación de Cálculos Repetidos (Redundantes)
// ============================================================================
void OptimizadorCodigo::eliminarCalculosRepetidos(vector<InstruccionTAC>& instrucciones) {
    struct CalculoRegistrado {
        string op1;
        string operador;
        string op2;
        string varResultado;
    };

    vector<CalculoRegistrado> calculosActivos;
    unordered_map<string, string> mapaReemplazo; // ej. t2 -> t1
    vector<bool> eliminar(instrucciones.size(), false);

    auto esConmutativo = [](const string& op) {
        return op == "+" || op == "*" || op == "==" || op == "!=";
    };

    auto mismoCalculo = [&](const CalculoRegistrado& c, const string& a, const string& op, const string& b) {
        if (c.operador != op) return false;
        if (c.op1 == a && c.op2 == b) return true;
        if (esConmutativo(op) && c.op1 == b && c.op2 == a) return true;
        return false;
    };

    for (size_t i = 0; i < instrucciones.size(); ++i) {
        // Primero propagar reemplazos de temporales redundantes ya detectados
        for (const auto& par : mapaReemplazo) {
            reemplazarOperandoLeido(instrucciones[i], par.first, par.second);
        }

        InfoTupla info = analizarTupla(instrucciones[i]);
        if (info.esSeccion) continue;

        // En etiquetas o llamadas a funciones se reinicia el contexto local de expresiones
        if (info.esEtiqueta || info.esCall) {
            calculosActivos.clear();
            mapaReemplazo.clear();
            continue;
        }

        if (info.esAsignacion) {
            // Si la variable destino se redefine, limpiar reemplazos asociados a ella
            mapaReemplazo.erase(info.lhs);
            for (auto it = mapaReemplazo.begin(); it != mapaReemplazo.end(); ) {
                if (it->second == info.lhs) {
                    it = mapaReemplazo.erase(it);
                } else {
                    ++it;
                }
            }

            // Verificar si es un cálculo binario elegible (ej. t2 = a + b)
            bool autoModificacion = (info.lhs == info.op1 || info.lhs == info.op2);
            if (info.esOperacionBinaria && !autoModificacion) {
                string temporalExistente = "";
                for (const auto& calc : calculosActivos) {
                    if (mismoCalculo(calc, info.op1, info.operador, info.op2)) {
                        temporalExistente = calc.varResultado;
                        break;
                    }
                }

                if (!temporalExistente.empty()) {
                    string instruccionOriginal = instrucciones[i].codigo;
                    mapaReemplazo[info.lhs] = temporalExistente;

                    // Reemplazar en adelante cualquier uso de info.lhs (ej. t2) por temporalExistente (ej. t1)
                    for (size_t k = i + 1; k < instrucciones.size(); ++k) {
                        InfoTupla infoK = analizarTupla(instrucciones[k]);
                        if (infoK.esAsignacion && (infoK.lhs == info.lhs || infoK.lhs == temporalExistente)) {
                            // Si la propia instrucción k lee antes de sobrescribir, reemplazar en su RHS
                            reemplazarOperandoLeido(instrucciones[k], info.lhs, temporalExistente);
                            break;
                        }
                        reemplazarOperandoLeido(instrucciones[k], info.lhs, temporalExistente);
                    }

                    eliminar[i] = true;
                    bitacora.push_back({
                        "Regla 3: Calculo Repetido",
                        instruccionOriginal,
                        "Reutiliza '" + temporalExistente + "' para '" + info.op1 + " " + info.operador + " " + info.op2 + "' y elimina '" + info.lhs + "'"
                    });
                    continue;
                }
            }

            // Invalidar cálculos previos cuyos operandos o resultado hayan cambiado
            calculosActivos.erase(
                remove_if(calculosActivos.begin(), calculosActivos.end(),
                          [&](const CalculoRegistrado& c) {
                              return c.varResultado == info.lhs ||
                                     c.op1 == info.lhs ||
                                     c.op2 == info.lhs;
                          }),
                calculosActivos.end());

            // Registrar nuevo cálculo binario
            if (info.esOperacionBinaria && !autoModificacion) {
                calculosActivos.push_back({info.op1, info.operador, info.op2, info.lhs});
            }
        }
    }

    vector<InstruccionTAC> resultado;
    for (size_t i = 0; i < instrucciones.size(); ++i) {
        if (!eliminar[i]) {
            resultado.push_back(instrucciones[i]);
        }
    }
    instrucciones = std::move(resultado);
}

// ============================================================================
// REGLA 4: Eliminación de Asignaciones Innecesarias
// ============================================================================
void OptimizadorCodigo::eliminarAsignacionesInnecesarias(vector<InstruccionTAC>& instrucciones) {
    unordered_set<string> variablesSobrescritas;

    // 4.1. Eliminar asignaciones cuyo valor es sobrescrito inmediatamente antes de ser leído
    // Ejemplo: temp = a; temp = b; -> se elimina temp = a
    {
        vector<bool> eliminar(instrucciones.size(), false);
        for (size_t i = 0; i < instrucciones.size(); ++i) {
            InfoTupla infoI = analizarTupla(instrucciones[i]);
            if (!infoI.esAsignacion || infoI.esCreacionObjeto) continue;

            for (size_t j = i + 1; j < instrucciones.size(); ++j) {
                InfoTupla infoJ = analizarTupla(instrucciones[j]);
                if (infoJ.esSeccion) continue;

                // Detener análisis si hay cambio de flujo de control
                if (infoJ.esEtiqueta || infoJ.esGotoIncondicional ||
                    infoJ.esSaltoCondicional || infoJ.esCall || infoJ.esReturn) {
                    break;
                }

                // Si la instrucción j lee la variable de i, entonces su valor SÍ se utilizó
                bool leidaEnJ = find(infoJ.leidos.begin(), infoJ.leidos.end(), infoI.lhs) != infoJ.leidos.end();
                if (leidaEnJ) {
                    break;
                }

                // Si la instrucción j vuelve a asignar a la misma variable sin haberla leído
                if (infoJ.esAsignacion && infoJ.lhs == infoI.lhs) {
                    eliminar[i] = true;
                    variablesSobrescritas.insert(infoI.lhs);
                    bitacora.push_back({
                        "Regla 4: Asignacion Innecesaria",
                        instrucciones[i].codigo,
                        "Eliminada porque '" + infoI.lhs + "' es sobrescrita en '" + instrucciones[j].codigo + "' sin leerse"
                    });
                    break;
                }
            }
        }

        vector<InstruccionTAC> filtradas;
        for (size_t i = 0; i < instrucciones.size(); ++i) {
            if (!eliminar[i]) filtradas.push_back(instrucciones[i]);
        }
        instrucciones = std::move(filtradas);
    }

    // 4.2. Propagar copias intermedias innecesarias (repetir hasta convergencia para cadenas de copias):
    // Ejemplo: temp = b; result = temp; -> result = b; (eliminando temp si ya no tiene uso posterior)
    bool cambioCopia = true;
    while (cambioCopia) {
        cambioCopia = false;
        vector<bool> eliminar(instrucciones.size(), false);
        for (size_t i = 0; i < instrucciones.size(); ++i) {
            if (eliminar[i]) continue;
            InfoTupla infoI = analizarTupla(instrucciones[i]);

            if (!infoI.esAsignacion || !infoI.esCopiaSimple) continue;
            if (infoI.op1 == "return_value") continue;

            // Aplicamos a copias entre variables (temp = b) o variables temporales sobrescritas
            bool elegibleCopia = esIdentificadorVariable(infoI.op1) ||
                                 variablesSobrescritas.count(infoI.lhs) > 0;
            if (!elegibleCopia) continue;

            // Buscar uso inmediato en copia directa (ej. result = temp) dentro del mismo bloque básico
            for (size_t j = i + 1; j < instrucciones.size(); ++j) {
                InfoTupla infoJ = analizarTupla(instrucciones[j]);
                if (infoJ.esSeccion) continue;

                if (infoJ.esEtiqueta || infoJ.esGotoIncondicional ||
                    infoJ.esSaltoCondicional || infoJ.esCall || infoJ.esReturn) {
                    break;
                }

                // Si se modifica el operando origen antes de usarse, no se puede propagar
                if (infoJ.esAsignacion && infoJ.lhs == infoI.op1) {
                    break;
                }

                // Si encontramos una copia como result = temp
                if (infoJ.esAsignacion && infoJ.esCopiaSimple && infoJ.op1 == infoI.lhs) {
                    // Verificar si infoI.lhs (temp) tiene algún uso posterior después de j
                    bool usadoDespuesDeJ = false;
                    for (size_t k = j + 1; k < instrucciones.size(); ++k) {
                        InfoTupla infoK = analizarTupla(instrucciones[k]);
                        if (find(infoK.leidos.begin(), infoK.leidos.end(), infoI.lhs) != infoK.leidos.end()) {
                            usadoDespuesDeJ = true;
                            break;
                        }
                        if (infoK.esAsignacion && infoK.lhs == infoI.lhs) {
                            break;
                        }
                    }

                    string instAntes = instrucciones[j].codigo;
                    instrucciones[j].codigo = infoJ.lhs + " = " + infoI.op1;
                    if (!instrucciones[j].comentario.empty()) {
                        instrucciones[j].comentario = "Asigna directamente " + infoI.op1 + " a " + infoJ.lhs + " (optimizado)";
                    }

                    if (!usadoDespuesDeJ) {
                        eliminar[i] = true;
                        cambioCopia = true;
                        bitacora.push_back({
                            "Regla 4: Asignacion Innecesaria",
                            instrucciones[i].codigo + " ; " + instAntes,
                            "Simplificada a '" + instrucciones[j].codigo + "' (elimina '" + infoI.lhs + "')"
                        });
                    }
                    break;
                }

                // Si temp se lee en otro tipo de instrucción o se redefine, detenemos
                if (find(infoJ.leidos.begin(), infoJ.leidos.end(), infoI.lhs) != infoJ.leidos.end() ||
                    (infoJ.esAsignacion && infoJ.lhs == infoI.lhs)) {
                    break;
                }
            }
        }

        if (cambioCopia) {
            vector<InstruccionTAC> filtradas;
            for (size_t i = 0; i < instrucciones.size(); ++i) {
                if (!eliminar[i]) filtradas.push_back(instrucciones[i]);
            }
            instrucciones = std::move(filtradas);
        }
    }
}

// ============================================================================
// REGLA 2: Análisis de Variables No Utilizadas
// ============================================================================
void OptimizadorCodigo::eliminarVariablesNoUtilizadas(vector<InstruccionTAC>& instrucciones) {
    // Repetir hasta convergencia por si al eliminar una variable quedan operandos previos sin uso
    bool huboCambios = true;
    while (huboCambios) {
        huboCambios = false;
        size_t n = instrucciones.size();
        if (n == 0) break;

        // Mapa de posiciones de etiquetas para detectar ciclos (saltos hacia atrás)
        unordered_map<string, size_t> posEtiqueta;
        for (size_t i = 0; i < n; ++i) {
            InfoTupla info = analizarTupla(instrucciones[i]);
            if (info.esEtiqueta) {
                posEtiqueta[info.etiqueta] = i;
            }
        }

        // Identificar cuál es la variable de salida final si no hay una con nombre estándar
        string ultimaVarUsuario = "";
        for (size_t i = n; i > 0; --i) {
            InfoTupla info = analizarTupla(instrucciones[i - 1]);
            if (info.esAsignacion) {
                ultimaVarUsuario = info.lhs;
                break;
            }
        }

        vector<bool> eliminar(n, false);

        for (size_t i = 0; i < n; ++i) {
            InfoTupla infoI = analizarTupla(instrucciones[i]);
            if (!infoI.esAsignacion || infoI.esCreacionObjeto) continue;

            const string& var = infoI.lhs;

            // Conservar asignaciones a variables de resultado final (ej. result, resultado, obj)
            // o temporales de paso de parametros para llamadas a funciones (call)
            if (esVariableSalidaFinal(var) || var == ultimaVarUsuario ||
                instrucciones[i].comentario.find("parametro") != string::npos) {
                continue;
            }

            // Determinar el rango de instrucciones donde 'var' podría ser leída
            // Por defecto: cualquier instrucción posterior j > i.
            // Si 'i' está dentro de un bucle (existe un salto hacia atrás después de i hacia una etiqueta <= i),
            // también puede ser leída desde el inicio de ese bucle.
            size_t inicioBusqueda = i + 1;
            for (size_t k = i + 1; k < n; ++k) {
                InfoTupla infoK = analizarTupla(instrucciones[k]);
                if (infoK.esGotoIncondicional || infoK.esSaltoCondicional) {
                    auto it = posEtiqueta.find(infoK.etiqueta);
                    if (it != posEtiqueta.end() && it->second <= i) {
                        inicioBusqueda = min(inicioBusqueda, it->second);
                    }
                }
            }

            bool esUtilizada = false;
            for (size_t j = inicioBusqueda; j < n; ++j) {
                if (j == i) continue;
                InfoTupla infoJ = analizarTupla(instrucciones[j]);
                if (find(infoJ.leidos.begin(), infoJ.leidos.end(), var) != infoJ.leidos.end()) {
                    esUtilizada = true;
                    break;
                }
            }

            if (!esUtilizada) {
                eliminar[i] = true;
                huboCambios = true;
                bitacora.push_back({
                    "Regla 2: Variable No Utilizada",
                    instrucciones[i].codigo,
                    "Eliminada porque la variable '" + var + "' nunca es leida en calculos ni en el resultado final"
                });
            }
        }

        if (huboCambios) {
            vector<InstruccionTAC> filtradas;
            for (size_t i = 0; i < n; ++i) {
                if (!eliminar[i]) filtradas.push_back(instrucciones[i]);
            }
            instrucciones = std::move(filtradas);
        }
    }
}

// ============================================================================
// Método Principal de Optimización y Salida Tabulada
// ============================================================================
vector<InstruccionTAC> OptimizadorCodigo::optimizar(const vector<InstruccionTAC>& instruccionesEntrada) {
    codigoOriginal = instruccionesEntrada;
    codigoOptimizado = instruccionesEntrada;
    bitacora.clear();

    // Aplicar las 4 reglas de optimización
    eliminarCodigoMuertoSaltos(codigoOptimizado);
    eliminarCalculosRepetidos(codigoOptimizado);
    eliminarAsignacionesInnecesarias(codigoOptimizado);
    eliminarVariablesNoUtilizadas(codigoOptimizado);

    return codigoOptimizado;
}

const vector<InstruccionTAC>& OptimizadorCodigo::obtenerCodigoOriginal() const {
    return codigoOriginal;
}

const vector<InstruccionTAC>& OptimizadorCodigo::obtenerCodigoOptimizado() const {
    return codigoOptimizado;
}

const vector<RegistroOptimizacion>& OptimizadorCodigo::obtenerBitacora() const {
    return bitacora;
}

void OptimizadorCodigo::imprimirTablaCodigo(const string& titulo, const vector<InstruccionTAC>& instrucciones) {
    cout << "\n+------------------------------------------------------------------------------+\n";
    cout << "| " << left << setw(76) << titulo << " |\n";
    cout << "+------+------------------------------+----------------------------------------+\n";
    cout << "| No.  | Instruccion TAC (Tupla)      | Descripcion / Comentario               |\n";
    cout << "+------+------------------------------+----------------------------------------+\n";

    if (instrucciones.empty()) {
        cout << "|  --  | (Sin instrucciones)          | --                                     |\n";
    } else {
        int num = 1;
        for (const auto& inst : instrucciones) {
            if (inst.esComentarioSeccion) {
                cout << "|  --  | " << left << setw(28) << inst.codigo
                     << " | " << left << setw(38) << "Seccion de bloque" << " |\n";
            } else {
                string comentarioCorto = inst.comentario;
                if (comentarioCorto.size() > 38) {
                    comentarioCorto = comentarioCorto.substr(0, 35) + "...";
                }
                cout << "| " << right << setw(3) << num++ << "  | "
                     << left << setw(28) << inst.codigo << " | "
                     << left << setw(38) << comentarioCorto << " |\n";
            }
        }
    }
    cout << "+------+------------------------------+----------------------------------------+\n";
}

void OptimizadorCodigo::imprimirResumenOptimizacion() const {
    cout << "\n+----------------------------------------------------------------------------------------------------+\n";
    cout << "| BITACORA DE REGLAS DE OPTIMIZACION APLICADAS                                                       |\n";
    cout << "+-----------------------------------+----------------------------------------------------------------+\n";
    cout << "| Regla Aplicada                    | Detalle de la Transformacion                                   |\n";
    cout << "+-----------------------------------+----------------------------------------------------------------+\n";

    if (bitacora.empty()) {
        cout << "| (Ninguna regla activada)          | El codigo ya se encuentra optimizado                           |\n";
        cout << "+-----------------------------------+----------------------------------------------------------------+\n";
    } else {
        for (const auto& reg : bitacora) {
            string lineaTupla = "Tupla: " + reg.instruccionOriginal;
            if (lineaTupla.size() > 62) lineaTupla = lineaTupla.substr(0, 59) + "...";

            string lineaAccion = "-> " + reg.detalleAccion;
            if (lineaAccion.size() > 62) lineaAccion = lineaAccion.substr(0, 59) + "...";

            cout << "| " << left << setw(33) << reg.regla << " | "
                 << left << setw(62) << lineaTupla << " |\n";
            cout << "| " << left << setw(33) << " " << " | "
                 << left << setw(62) << lineaAccion << " |\n";
            cout << "+-----------------------------------+----------------------------------------------------------------+\n";
        }
    }
}

void OptimizadorCodigo::imprimirComparativa() const {
    imprimirTablaCodigo("CODIGO INTERMEDIO ORIGINAL (ANTES DE OPTIMIZAR)", codigoOriginal);
    imprimirResumenOptimizacion();
    imprimirTablaCodigo("CODIGO INTERMEDIO OPTIMIZADO (DESPUES DE OPTIMIZAR)", codigoOptimizado);

    int origCount = 0;
    for (const auto& i : codigoOriginal) if (!i.esComentarioSeccion) origCount++;
    int optCount = 0;
    for (const auto& i : codigoOptimizado) if (!i.esComentarioSeccion) optCount++;

    int eliminadas = origCount - optCount;
    double porcentaje = (origCount > 0) ? (100.0 * eliminadas / origCount) : 0.0;

    cout << "  [METRICAS DE OPTIMIZACION]: Instrucciones originales: " << origCount
         << " | Instrucciones optimizadas: " << optCount
         << " | Reduccion: " << eliminadas << " tupla(s) ("
         << fixed << setprecision(1) << porcentaje << "%)\n";
}
