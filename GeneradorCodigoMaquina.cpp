// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel

#include "GeneradorCodigoMaquina.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_set>

using namespace std;

namespace {

// Nombres de integrantes:
// 1. Anguiano Garcia Angel Yahir Guadalupe
// 2. Figueroa Robles Axel Israel
// 3. Molina Alvarado Alvaro Moises
// 4. Torres Martinez Miguel Angel
struct TuplaAnalizadaMaquina {
    bool esSeccion = false;
    bool esSoloEtiqueta = false;
    bool esGoto = false;
    bool esIfGoto = false;
    bool esIfNotGoto = false;
    bool esIfRelacionalGoto = false;
    bool esReturn = false;
    bool esCall = false;
    bool esCreate = false;
    bool esCopia = false;
    bool esBinaria = false;
    bool esInvalida = false;

    string etiquetaLinea;     // Etiqueta prefijada (ej. L1 en "L1: t1 = a + b") o declarada ("L1:")
    string destinoSalto;      // Etiqueta destino en goto / if / call
    string lhs;               // Variable destino
    string op1;               // Operando 1
    string operador;          // Operador (+, -, *, /, ==, !=, >, <, >=, <=, &&, ||)
    string op2;               // Operando 2
    string mensajeError;      // Detalle si la tupla es invalida
};

vector<string> tokenizarLineaTAC(const string& codigo) {
    vector<string> tokens;
    size_t i = 0;
    size_t n = codigo.size();

    while (i < n) {
        if (isspace(static_cast<unsigned char>(codigo[i]))) {
            i++;
            continue;
        }
        if (codigo[i] == '"' || codigo[i] == '\'') {
            char q = codigo[i];
            size_t ini = i++;
            while (i < n && codigo[i] != q) {
                if (codigo[i] == '\\' && i + 1 < n) i++;
                i++;
            }
            if (i < n) i++;
            tokens.push_back(codigo.substr(ini, i - ini));
        } else {
            size_t ini = i;
            while (i < n && !isspace(static_cast<unsigned char>(codigo[i]))) {
                i++;
            }
            tokens.push_back(codigo.substr(ini, i - ini));
        }
    }
    return tokens;
}

bool esNumeroLiteral(const string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[0] == '-' || s[0] == '+') {
        if (s.size() == 1) return false;
        i = 1;
    }
    bool vioDigito = false;
    bool vioPunto = false;
    for (; i < s.size(); ++i) {
        if (isdigit(static_cast<unsigned char>(s[i]))) {
            vioDigito = true;
        } else if (s[i] == '.' && !vioPunto) {
            vioPunto = true;
        } else {
            return false;
        }
    }
    return vioDigito;
}

bool esTemporalTAC(const string& s) {
    if (s.size() < 2 || s[0] != 't') return false;
    for (size_t i = 1; i < s.size(); ++i) {
        if (!isdigit(static_cast<unsigned char>(s[i]))) return false;
    }
    return true;
}

bool esSimboloResultado(const string& s) {
    static const unordered_set<string> resultados = {
        "result", "resultado", "res", "total", "salida", "final", "ans", "obj"
    };
    return resultados.count(s) > 0;
}

bool esRegistroCPU(const string& s) {
    return s == "R1" || s == "R2" || s == "R3" || s == "R4" || s == "PC" || s == "SP";
}

bool esIdentificadorValido(const string& s) {
    if (s.empty() || esNumeroLiteral(s) || esRegistroCPU(s)) return false;
    if (s == "true" || s == "false" || s == "return_value") return false;
    unsigned char c0 = static_cast<unsigned char>(s[0]);
    if (!(isalpha(c0) || c0 == '_' || c0 >= 128)) return false;
    for (size_t i = 1; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (!(isalnum(c) || c == '_' || c == '.' || c >= 128)) return false;
    }
    return true;
}

bool esOperadorSoportado(const string& op) {
    static const unordered_set<string> ops = {
        "+", "-", "*", "/", "==", "!=", ">", "<", ">=", "<=", "&&", "||"
    };
    return ops.count(op) > 0;
}

TuplaAnalizadaMaquina parsearTuplaMaquina(const InstruccionTAC& inst) {
    TuplaAnalizadaMaquina info;
    if (inst.esComentarioSeccion || (!inst.codigo.empty() && inst.codigo[0] == '#')) {
        info.esSeccion = true;
        return info;
    }

    vector<string> t = tokenizarLineaTAC(inst.codigo);
    if (t.empty()) {
        info.esSeccion = true;
        return info;
    }

    // Verificar si inicia con una etiqueta (ej. "L1:" sola o "L1: t1 = a + b")
    if (t[0].size() > 1 && t[0].back() == ':') {
        info.etiquetaLinea = t[0].substr(0, t[0].size() - 1);
        t.erase(t.begin());
        if (t.empty()) {
            info.esSoloEtiqueta = true;
            return info;
        }
    }

    // 1. Salto incondicional: goto L_X
    if (t[0] == "goto") {
        if (t.size() == 2 && esIdentificadorValido(t[1])) {
            info.esGoto = true;
            info.destinoSalto = t[1];
        } else {
            info.esInvalida = true;
            info.mensajeError = "Sintaxis invalida en salto incondicional 'goto' (falta etiqueta destino valida).";
        }
        return info;
    }

    // 2. Salto condicional: if ... goto L_X
    if (t[0] == "if") {
        if (t.size() == 4 && t[2] == "goto") {
            info.esIfGoto = true;
            info.op1 = t[1];
            info.destinoSalto = t[3];
            return info;
        }
        if (t.size() == 5 && t[1] == "not" && t[3] == "goto") {
            info.esIfNotGoto = true;
            info.op1 = t[2];
            info.destinoSalto = t[4];
            return info;
        }
        if (t.size() == 6 && t[4] == "goto" && esOperadorSoportado(t[2])) {
            info.esIfRelacionalGoto = true;
            info.op1 = t[1];
            info.operador = t[2];
            info.op2 = t[3];
            info.destinoSalto = t[5];
            return info;
        }
        info.esInvalida = true;
        info.mensajeError = "Estructura condicional 'if ... goto' mal formada en codigo intermedio.";
        return info;
    }

    // 3. Sentencia return
    if (t[0] == "return") {
        if (t.size() == 1) {
            info.esReturn = true;
            return info;
        }
        if (t.size() == 2) {
            info.esReturn = true;
            info.op1 = t[1];
            return info;
        }
        info.esInvalida = true;
        info.mensajeError = "Instruccion 'return' con exceso de operandos.";
        return info;
    }

    // 4. Llamada a subrutina: call L_X
    if (t[0] == "call") {
        if (t.size() == 2 && esIdentificadorValido(t[1])) {
            info.esCall = true;
            info.destinoSalto = t[1];
        } else {
            info.esInvalida = true;
            info.mensajeError = "Instruccion 'call' requiere una etiqueta o metodo destino valido.";
        }
        return info;
    }

    // 5. Asignaciones: lhs = ...
    if (t.size() >= 3 && t[1] == "=") {
        info.lhs = t[0];
        if (!esIdentificadorValido(info.lhs)) {
            info.esInvalida = true;
            info.mensajeError = "Identificador de destino invalido '" + info.lhs + "' en asignacion.";
            return info;
        }

        if (t.size() == 4 && t[2] == "create") {
            info.esCreate = true;
            info.op1 = t[3];
            return info;
        }

        if (t.size() == 3) {
            info.esCopia = true;
            info.op1 = t[2];
            return info;
        }

        if (t.size() == 5) {
            if (!esOperadorSoportado(t[3])) {
                info.esInvalida = true;
                info.mensajeError = "Operador intermedio desconocido o no soportado por la ALU: '" + t[3] + "'.";
                return info;
            }
            if (t[3] == "/" && (t[4] == "0" || t[4] == "0.0")) {
                info.esInvalida = true;
                info.mensajeError = "Division por cero detectada al traducir a instruccion DIV del procesador.";
                return info;
            }
            info.esBinaria = true;
            info.op1 = t[2];
            info.operador = t[3];
            info.op2 = t[4];
            return info;
        }

        info.esInvalida = true;
        info.mensajeError = "Tupla de asignacion no cumple el formato de Codigo de 3 Direcciones (TAC).";
        return info;
    }

    info.esInvalida = true;
    info.mensajeError = "Instruccion de codigo intermedio no reconocida: '" + inst.codigo + "'.";
    return info;
}

string mnemonicoDesdeOperador(const string& op) {
    if (op == "+") return "ADD";
    if (op == "-") return "SUB";
    if (op == "*") return "MUL";
    if (op == "/") return "DIV";
    if (op == "&&") return "AND";
    if (op == "||") return "OR";
    return "CMP";
}

string comentarioOperacion(const string& mnemonico, const string& op2) {
    if (mnemonico == "ADD") return "Sumen el valor de " + op2 + " al registro R1";
    if (mnemonico == "SUB") return "Resten el valor de " + op2 + " al registro R1";
    if (mnemonico == "MUL") return "Multipliquen el valor en R1 por " + op2;
    if (mnemonico == "DIV") return "Dividan el valor en R1 entre " + op2;
    if (mnemonico == "AND") return "Apliquen AND logico de " + op2 + " con R1";
    if (mnemonico == "OR")  return "Apliquen OR logico de " + op2 + " con R1";
    return "Comparen el valor en R1 con " + op2;
}

} // namespace

string GeneradorCodigoMaquina::formatearHex(uint32_t valor, int digitos) {
    ostringstream oss;
    oss << "0x" << uppercase << hex << setw(digitos) << setfill('0') << valor;
    return oss.str();
}

bool GeneradorCodigoMaquina::validarInstruccionesTAC(const vector<InstruccionTAC>& instrucciones) {
    errores.clear();
    unordered_set<string> etiquetasDeclaradas;

    for (size_t i = 0; i < instrucciones.size(); ++i) {
        TuplaAnalizadaMaquina info = parsearTuplaMaquina(instrucciones[i]);
        if (!info.etiquetaLinea.empty()) {
            etiquetasDeclaradas.insert(info.etiquetaLinea);
        }
        if (info.esInvalida) {
            errores.push_back({
                static_cast<int>(i + 1),
                instrucciones[i].codigo,
                "Error de Traduccion ISA",
                info.mensajeError
            });
        }
    }

    // Verificar que todos los saltos apunten a etiquetas existentes
    for (size_t i = 0; i < instrucciones.size(); ++i) {
        TuplaAnalizadaMaquina info = parsearTuplaMaquina(instrucciones[i]);
        if (info.esGoto || info.esIfGoto || info.esIfNotGoto || info.esIfRelacionalGoto || info.esCall) {
            if (!etiquetasDeclaradas.count(info.destinoSalto)) {
                errores.push_back({
                    static_cast<int>(i + 1),
                    instrucciones[i].codigo,
                    "Error de Direccionamiento",
                    "La etiqueta destino '" + info.destinoSalto + "' no existe en el segmento de codigo."
                });
            }
        }
    }

    return errores.empty();
}

void GeneradorCodigoMaquina::generarMapeoInstrucciones(const vector<InstruccionTAC>& instrucciones) {
    codigoMaquinaBase.clear();
    codigoMaquinaMapeado.clear();

    // Analizar todas las tuplas primero para saber cuántas veces se lee cada temporal tX
    vector<TuplaAnalizadaMaquina> tuplas;
    unordered_map<string, int> conteoLecturas;
    for (const auto& inst : instrucciones) {
        TuplaAnalizadaMaquina t = parsearTuplaMaquina(inst);
        tuplas.push_back(t);
        if (!t.op1.empty()) conteoLecturas[t.op1]++;
        if (!t.op2.empty()) conteoLecturas[t.op2]++;
    }

    // 1) Generación de Código Máquina Base (sin retención en registros: cada tupla hace LOAD/OP/STORE)
    {
        string etiquetaPendiente = "";
        for (size_t i = 0; i < tuplas.size(); ++i) {
            const auto& t = tuplas[i];
            if (t.esSeccion) continue;
            if (!t.etiquetaLinea.empty()) {
                if (!etiquetaPendiente.empty()) etiquetaPendiente += ",";
                etiquetaPendiente += t.etiquetaLinea;
            }
            if (t.esSoloEtiqueta) continue;

            auto emitirBase = [&](const string& mnem, const string& o1, const string& o2, const string& com) {
                codigoMaquinaBase.push_back({etiquetaPendiente, mnem, o1, o2, com});
                etiquetaPendiente.clear();
            };

            if (t.esBinaria) {
                emitirBase("LOAD", "R1", t.op1, "Carguen el valor de " + t.op1 + " en el registro R1");
                string mnem = mnemonicoDesdeOperador(t.operador);
                emitirBase(mnem, "R1", t.op2, comentarioOperacion(mnem, t.op2));
                emitirBase("STORE", t.lhs, "R1", "Almacenen el resultado temporal en " + t.lhs);
            } else if (t.esCopia) {
                emitirBase("LOAD", "R1", t.op1, "Carguen el valor de " + t.op1 + " en el registro R1");
                emitirBase("STORE", t.lhs, "R1", "Almacenen el resultado en la variable " + t.lhs);
            } else if (t.esCreate) {
                emitirBase("ALLOC", "R1", t.op1, "Reserven memoria para instancia de " + t.op1);
                emitirBase("STORE", t.lhs, "R1", "Almacenen referencia del objeto en " + t.lhs);
            } else if (t.esGoto) {
                emitirBase("JMP", t.destinoSalto, "", "Salto incondicional a " + t.destinoSalto);
            } else if (t.esIfGoto) {
                emitirBase("LOAD", "R1", t.op1, "Carguen condicion " + t.op1 + " en R1");
                emitirBase("JNZ", t.destinoSalto, "", "Si R1 es verdadero, salten a " + t.destinoSalto);
            } else if (t.esIfNotGoto) {
                emitirBase("LOAD", "R1", t.op1, "Carguen condicion " + t.op1 + " en R1");
                emitirBase("JZ", t.destinoSalto, "", "Si R1 es falso, salten a " + t.destinoSalto);
            } else if (t.esIfRelacionalGoto) {
                emitirBase("LOAD", "R1", t.op1, "Carguen " + t.op1 + " en R1");
                emitirBase("CMP", "R1", t.op2, "Comparen R1 con " + t.op2);
                emitirBase("JE", t.destinoSalto, "", "Si son iguales, salten a " + t.destinoSalto);
            } else if (t.esCall) {
                emitirBase("CALL", t.destinoSalto, "", "Llamen a la subrutina en " + t.destinoSalto);
            } else if (t.esReturn) {
                if (!t.op1.empty()) {
                    emitirBase("LOAD", "R1", t.op1, "Carguen valor de retorno " + t.op1 + " en R1");
                }
                emitirBase("RET", "", "", "Retornen de la subrutina actual");
            }
        }
        if (!etiquetaPendiente.empty()) {
            codigoMaquinaBase.push_back({etiquetaPendiente, "NOP", "", "", "Fin de bloque etiquetado"});
        }
    }

    // 2) Generación de Código Máquina Mapeado (Mapeo directo con seguimiento del acumulador R1)
    // Ejemplo: t1 = a + b; t2 = t1 * c; result = t2 -> LOAD R1, a; ADD R1, b; MUL R1, c; STORE result, R1
    {
        string enR1 = "";
        string etiquetaPendiente = "";

        auto siguienteTuplaEjecutable = [&](size_t actual) -> const TuplaAnalizadaMaquina* {
            for (size_t k = actual + 1; k < tuplas.size(); ++k) {
                if (!tuplas[k].esSeccion && !tuplas[k].esSoloEtiqueta) {
                    return &tuplas[k];
                }
            }
            return nullptr;
        };

        for (size_t i = 0; i < tuplas.size(); ++i) {
            const auto& t = tuplas[i];
            if (t.esSeccion) continue;

            if (!t.etiquetaLinea.empty()) {
                if (!etiquetaPendiente.empty()) etiquetaPendiente += ",";
                etiquetaPendiente += t.etiquetaLinea;
            }
            if (t.esSoloEtiqueta) continue;

            auto emitirMapeado = [&](const string& mnem, const string& o1, const string& o2, const string& com) {
                codigoMaquinaMapeado.push_back({etiquetaPendiente, mnem, o1, o2, com});
                etiquetaPendiente.clear();
            };

            const TuplaAnalizadaMaquina* sig = siguienteTuplaEjecutable(i);

            if (t.esBinaria) {
                string opIzq = t.op1;
                string opDer = t.op2;
                // Si el operando derecho ya está en R1 y la operación es conmutativa (+, *), intercambiar
                if (enR1 != opIzq && enR1 == opDer && (t.operador == "+" || t.operador == "*")) {
                    swap(opIzq, opDer);
                }

                if (enR1 != opIzq) {
                    emitirMapeado("LOAD", "R1", opIzq, "Carguen el valor de " + opIzq + " en el registro R1");
                }
                string mnem = mnemonicoDesdeOperador(t.operador);
                emitirMapeado(mnem, "R1", opDer, comentarioOperacion(mnem, opDer));
                enR1 = t.lhs;

                // Determinar si t.lhs es un temporal efímero consumido inmediatamente en la siguiente tupla
                bool consumidoInmediatamente = false;
                if (esTemporalTAC(t.lhs) && conteoLecturas[t.lhs] == 1 && sig != nullptr) {
                    if ((sig->esBinaria && (sig->op1 == t.lhs || sig->op2 == t.lhs)) ||
                        (sig->esCopia && sig->op1 == t.lhs) ||
                        ((sig->esIfGoto || sig->esIfNotGoto || sig->esReturn) && sig->op1 == t.lhs)) {
                        consumidoInmediatamente = true;
                    }
                }

                if (!consumidoInmediatamente) {
                    emitirMapeado("STORE", t.lhs, "R1", "Almacenen el resultado en la variable " + t.lhs);
                }
            } else if (t.esCopia) {
                if (enR1 != t.op1) {
                    emitirMapeado("LOAD", "R1", t.op1, "Carguen el valor de " + t.op1 + " en el registro R1");
                }
                emitirMapeado("STORE", t.lhs, "R1", "Almacenen el resultado en la variable " + t.lhs);
                enR1 = t.lhs;
            } else if (t.esCreate) {
                emitirMapeado("ALLOC", "R1", t.op1, "Reserven memoria para instancia de " + t.op1);
                emitirMapeado("STORE", t.lhs, "R1", "Almacenen referencia del objeto en " + t.lhs);
                enR1 = t.lhs;
            } else if (t.esGoto) {
                emitirMapeado("JMP", t.destinoSalto, "", "Salto incondicional a " + t.destinoSalto);
                enR1.clear();
            } else if (t.esIfGoto) {
                if (enR1 != t.op1) {
                    emitirMapeado("LOAD", "R1", t.op1, "Carguen condicion " + t.op1 + " en R1");
                }
                emitirMapeado("JNZ", t.destinoSalto, "", "Si R1 es verdadero, salten a " + t.destinoSalto);
                enR1.clear();
            } else if (t.esIfNotGoto) {
                if (enR1 != t.op1) {
                    emitirMapeado("LOAD", "R1", t.op1, "Carguen condicion " + t.op1 + " en R1");
                }
                emitirMapeado("JZ", t.destinoSalto, "", "Si R1 es falso, salten a " + t.destinoSalto);
                enR1.clear();
            } else if (t.esIfRelacionalGoto) {
                if (enR1 != t.op1) {
                    emitirMapeado("LOAD", "R1", t.op1, "Carguen " + t.op1 + " en R1");
                }
                emitirMapeado("CMP", "R1", t.op2, "Comparen R1 con " + t.op2);
                emitirMapeado("JE", t.destinoSalto, "", "Si coincide, salten a " + t.destinoSalto);
                enR1.clear();
            } else if (t.esCall) {
                emitirMapeado("CALL", t.destinoSalto, "", "Llamen a la subrutina en " + t.destinoSalto);
                enR1 = "return_value";
            } else if (t.esReturn) {
                if (!t.op1.empty() && enR1 != t.op1) {
                    emitirMapeado("LOAD", "R1", t.op1, "Carguen valor de retorno " + t.op1 + " en R1");
                }
                emitirMapeado("RET", "", "", "Retornen de la subrutina actual");
                enR1.clear();
            }
        }

        if (!etiquetaPendiente.empty()) {
            codigoMaquinaMapeado.push_back({etiquetaPendiente, "NOP", "", "", "Fin de bloque etiquetado"});
        }
    }
}

void GeneradorCodigoMaquina::aplicarOptimizacionInstrucciones(bool modoAcumulacionAditiva) {
    bitacoraOptimizacion.clear();
    codigoMaquinaOptimizado = codigoMaquinaBase;

    // ------------------------------------------------------------------------
    // Regla M1: Eliminación de pares STORE tX, R1 seguidos de LOAD R1, tX
    // ------------------------------------------------------------------------
    {
        unordered_map<string, int> lecturasSimbolo;
        for (const auto& inst : codigoMaquinaOptimizado) {
            if (inst.mnemonico != "STORE" && !inst.op2.empty()) {
                lecturasSimbolo[inst.op2]++;
            }
        }

        vector<InstruccionMaquina> resultado;
        size_t i = 0;
        while (i < codigoMaquinaOptimizado.size()) {
            if (i + 1 < codigoMaquinaOptimizado.size()) {
                const auto& act = codigoMaquinaOptimizado[i];
                const auto& sig = codigoMaquinaOptimizado[i + 1];
                if (act.mnemonico == "STORE" && act.op2 == "R1" &&
                    sig.mnemonico == "LOAD" && sig.op1 == "R1" &&
                    act.op1 == sig.op2 && esTemporalTAC(act.op1) &&
                    lecturasSimbolo[act.op1] == 1) {
                    bitacoraOptimizacion.push_back({
                        "Regla M1: Retencion en Registro R1",
                        "STORE " + act.op1 + ", R1  +  LOAD R1, " + sig.op2,
                        "Eliminados STORE/LOAD redundantes del temporal '" + act.op1 + "' (valor permanece en R1)"
                    });
                    // Preservar cualquier etiqueta que estuviera en act o sig en la siguiente instrucción
                    string etiqCombinada = act.etiqueta;
                    if (!sig.etiqueta.empty()) {
                        if (!etiqCombinada.empty()) etiqCombinada += ",";
                        etiqCombinada += sig.etiqueta;
                    }
                    if (!etiqCombinada.empty() && i + 2 < codigoMaquinaOptimizado.size()) {
                        if (codigoMaquinaOptimizado[i + 2].etiqueta.empty()) {
                            codigoMaquinaOptimizado[i + 2].etiqueta = etiqCombinada;
                        } else {
                            codigoMaquinaOptimizado[i + 2].etiqueta =
                                etiqCombinada + "," + codigoMaquinaOptimizado[i + 2].etiqueta;
                        }
                    }
                    i += 2;
                    continue;
                }
            }
            resultado.push_back(codigoMaquinaOptimizado[i]);
            i++;
        }
        codigoMaquinaOptimizado = std::move(resultado);
    }

    // ------------------------------------------------------------------------
    // Regla M2: Acumulación Aditiva Directa en R1 (Ejemplo exacto de la práctica)
    // t1 = a + b; t2 = t1 + c; result = t2 -> ADD R1, a; ADD R1, b; ADD R1, c; STORE result, R1
    // ------------------------------------------------------------------------
    if (modoAcumulacionAditiva && !codigoMaquinaOptimizado.empty()) {
        bool soloAdiciones = true;
        for (size_t i = 0; i < codigoMaquinaOptimizado.size(); ++i) {
            const auto& inst = codigoMaquinaOptimizado[i];
            if (i == 0 && inst.mnemonico == "LOAD" && inst.op1 == "R1") continue;
            if (i + 1 == codigoMaquinaOptimizado.size() && inst.mnemonico == "STORE") continue;
            if (inst.mnemonico != "ADD") {
                soloAdiciones = false;
                break;
            }
        }

        if (soloAdiciones && codigoMaquinaOptimizado[0].mnemonico == "LOAD") {
            string opInicial = codigoMaquinaOptimizado[0].op2;
            codigoMaquinaOptimizado[0].mnemonico = "ADD";
            codigoMaquinaOptimizado[0].comentario = "Sumen a R1";
            if (codigoMaquinaOptimizado.size() > 1 && codigoMaquinaOptimizado[1].mnemonico == "ADD") {
                codigoMaquinaOptimizado[1].comentario = "Sumen a R1";
            }
            for (size_t k = 2; k < codigoMaquinaOptimizado.size(); ++k) {
                if (codigoMaquinaOptimizado[k].mnemonico == "ADD") {
                    codigoMaquinaOptimizado[k].comentario = "Sumen " + codigoMaquinaOptimizado[k].op2 + " a R1";
                } else if (codigoMaquinaOptimizado[k].mnemonico == "STORE") {
                    codigoMaquinaOptimizado[k].comentario =
                        "Almacenen el resultado en " + codigoMaquinaOptimizado[k].op1;
                }
            }
            bitacoraOptimizacion.push_back({
                "Regla M2: Acumulacion Aditiva en R1",
                "LOAD R1, " + opInicial + " -> ADD R1, " + opInicial,
                "Unifica cadena de sumas acumulando directamente sobre el registro R1"
            });
        }
    }

    // ------------------------------------------------------------------------
    // Regla M3: Eliminación Peephole de Identidades Aritméticas y Cargas Redundantes
    // (ADD R1, 0 | SUB R1, 0 | MUL R1, 1 | DIV R1, 1 | LOAD R1, x tras STORE x, R1)
    // ------------------------------------------------------------------------
    {
        vector<InstruccionMaquina> resultado;
        string ultimoSimboloEnR1 = "";

        for (size_t i = 0; i < codigoMaquinaOptimizado.size(); ++i) {
            auto& inst = codigoMaquinaOptimizado[i];
            if (!inst.etiqueta.empty()) {
                ultimoSimboloEnR1.clear();
            }

            bool esIdentidad = ((inst.mnemonico == "ADD" || inst.mnemonico == "SUB") && inst.op2 == "0") ||
                               ((inst.mnemonico == "MUL" || inst.mnemonico == "DIV") && inst.op2 == "1");
            if (esIdentidad) {
                bitacoraOptimizacion.push_back({
                    "Regla M3: Identidad Aritmetica Peephole",
                    inst.mnemonico + " " + inst.op1 + ", " + inst.op2,
                    "Eliminada operacion aritmetica neutra que no altera el registro R1"
                });
                continue;
            }

            if (inst.mnemonico == "LOAD" && inst.op1 == "R1" && !ultimoSimboloEnR1.empty() &&
                inst.op2 == ultimoSimboloEnR1 && inst.etiqueta.empty()) {
                bitacoraOptimizacion.push_back({
                    "Regla M3: Carga Redundante Peephole",
                    "LOAD R1, " + inst.op2,
                    "Eliminada porque el registro R1 ya contiene el valor de '" + inst.op2 + "'"
                });
                continue;
            }

            if (inst.mnemonico == "STORE" && inst.op2 == "R1") {
                ultimoSimboloEnR1 = inst.op1;
            } else if (inst.mnemonico == "LOAD" && inst.op1 == "R1") {
                ultimoSimboloEnR1 = inst.op2;
            } else {
                ultimoSimboloEnR1.clear();
            }

            resultado.push_back(inst);
        }
        codigoMaquinaOptimizado = std::move(resultado);
    }

    // ------------------------------------------------------------------------
    // Regla M4: Eliminación de Saltos Redundantes a la Siguiente Instrucción
    // (JMP L_X seguido inmediatamente por L_X:)
    // ------------------------------------------------------------------------
    {
        vector<InstruccionMaquina> resultado;
        for (size_t i = 0; i < codigoMaquinaOptimizado.size(); ++i) {
            const auto& inst = codigoMaquinaOptimizado[i];
            if (inst.mnemonico == "JMP" && i + 1 < codigoMaquinaOptimizado.size()) {
                const string& etiqSig = codigoMaquinaOptimizado[i + 1].etiqueta;
                if (etiqSig == inst.op1) {
                    bitacoraOptimizacion.push_back({
                        "Regla M4: Salto Contiguo Innecesario",
                        "JMP " + inst.op1,
                        "Eliminado salto hacia '" + inst.op1 + "' por ser la siguiente instruccion inmediata"
                    });
                    continue;
                }
            }
            resultado.push_back(inst);
        }
        codigoMaquinaOptimizado = std::move(resultado);
    }

    // Eliminar NOP final si la etiqueta que tenía ya no es referenciada por ningún salto
    if (!codigoMaquinaOptimizado.empty() && codigoMaquinaOptimizado.back().mnemonico == "NOP") {
        string etiqNop = codigoMaquinaOptimizado.back().etiqueta;
        bool referenciada = false;
        for (const auto& inst : codigoMaquinaOptimizado) {
            if ((inst.mnemonico == "JMP" || inst.mnemonico == "JZ" || inst.mnemonico == "JNZ" ||
                 inst.mnemonico == "JE" || inst.mnemonico == "CALL") && inst.op1 == etiqNop) {
                referenciada = true;
                break;
            }
        }
        if (!referenciada) {
            codigoMaquinaOptimizado.pop_back();
        }
    }
}

uint32_t GeneradorCodigoMaquina::codificarInstruccionBinaria(const InstruccionMaquina& inst) const {
    uint8_t opcode = 0x00;
    if (inst.mnemonico == "LOAD")       opcode = 0x01;
    else if (inst.mnemonico == "STORE") opcode = 0x02;
    else if (inst.mnemonico == "ADD")   opcode = 0x10;
    else if (inst.mnemonico == "SUB")   opcode = 0x11;
    else if (inst.mnemonico == "MUL")   opcode = 0x12;
    else if (inst.mnemonico == "DIV")   opcode = 0x13;
    else if (inst.mnemonico == "AND")   opcode = 0x14;
    else if (inst.mnemonico == "OR")    opcode = 0x15;
    else if (inst.mnemonico == "CMP")   opcode = 0x20;
    else if (inst.mnemonico == "JMP")   opcode = 0x30;
    else if (inst.mnemonico == "JE")    opcode = 0x31;
    else if (inst.mnemonico == "JNZ")   opcode = 0x32;
    else if (inst.mnemonico == "JZ")    opcode = 0x33;
    else if (inst.mnemonico == "CALL")  opcode = 0x40;
    else if (inst.mnemonico == "RET")   opcode = 0x41;
    else if (inst.mnemonico == "ALLOC") opcode = 0x50;
    else if (inst.mnemonico == "NOP")   opcode = 0x90;

    uint8_t regField = 0x10; // R1 por defecto (nibble alto = 1)
    uint16_t addrField = 0x0000;

    string operandoDir = (inst.mnemonico == "STORE" || inst.mnemonico == "JMP" ||
                          inst.mnemonico == "JE" || inst.mnemonico == "JZ" ||
                          inst.mnemonico == "JNZ" || inst.mnemonico == "CALL")
                         ? inst.op1 : inst.op2;

    auto it = mapaDirecciones.find(operandoDir);
    if (it != mapaDirecciones.end()) {
        addrField = static_cast<uint16_t>(it->second & 0xFFFF);
    } else if (esNumeroLiteral(operandoDir)) {
        regField = 0x11; // Modo inmediato
        addrField = static_cast<uint16_t>(stoi(operandoDir) & 0xFFFF);
    }

    return (static_cast<uint32_t>(opcode) << 24) |
           (static_cast<uint32_t>(regField) << 16) |
           static_cast<uint32_t>(addrField);
}

void GeneradorCodigoMaquina::resolverDireccionesYTablaSimbolos(
    vector<InstruccionMaquina>& instrucciones,
    const unordered_map<string, int>& valoresIniciales) {

    tablaSimbolosMemoria.clear();
    mapaDirecciones.clear();

    // 1. Asignar direcciones de memoria al segmento de código (.text: 0x1000, 0x1004, ...)
    uint32_t dirCodigoActual = 0x1000;
    vector<pair<string, uint32_t>> etiquetasCodigo;

    for (auto& inst : instrucciones) {
        inst.direccionMemoria = dirCodigoActual;
        if (!inst.etiqueta.empty()) {
            stringstream ss(inst.etiqueta);
            string etiq;
            while (getline(ss, etiq, ',')) {
                if (!etiq.empty() && !mapaDirecciones.count(etiq)) {
                    mapaDirecciones[etiq] = dirCodigoActual;
                    etiquetasCodigo.push_back({etiq, dirCodigoActual});
                }
            }
        }
        dirCodigoActual += 0x0004;
    }

    // 2. Recolectar variables de datos (0x2000+) y variables de resultado (0x3000+)
    vector<string> variablesDatos;
    vector<string> variablesResultado;
    unordered_set<string> vistos;

    auto registrarOperandoVariable = [&](const string& op) {
        if (op.empty() || esRegistroCPU(op) || esNumeroLiteral(op)) return;
        if (mapaDirecciones.count(op)) return; // Es una etiqueta de código
        if (vistos.count(op)) return;
        vistos.insert(op);

        if (esSimboloResultado(op)) {
            variablesResultado.push_back(op);
        } else {
            variablesDatos.push_back(op);
        }
    };

    for (const auto& inst : instrucciones) {
        if (inst.mnemonico == "JMP" || inst.mnemonico == "JZ" || inst.mnemonico == "JNZ" ||
            inst.mnemonico == "JE" || inst.mnemonico == "CALL" || inst.mnemonico == "ALLOC") {
            continue;
        }
        registrarOperandoVariable(inst.op1);
        registrarOperandoVariable(inst.op2);
    }

    uint32_t dirDatosActual = 0x2000;
    for (const auto& var : variablesDatos) {
        int valIni = 0;
        auto it = valoresIniciales.find(var);
        if (it != valoresIniciales.end()) valIni = it->second;

        mapaDirecciones[var] = dirDatosActual;
        tablaSimbolosMemoria.push_back({var, dirDatosActual, ".data", "Variable", valIni});
        dirDatosActual += 0x0004;
    }

    uint32_t dirResultadoActual = 0x3000;
    for (const auto& var : variablesResultado) {
        int valIni = 0;
        auto it = valoresIniciales.find(var);
        if (it != valoresIniciales.end()) valIni = it->second;

        mapaDirecciones[var] = dirResultadoActual;
        tablaSimbolosMemoria.push_back({var, dirResultadoActual, ".result", "Resultado", valIni});
        dirResultadoActual += 0x0004;
    }

    // También registrar las etiquetas de código en la tabla de símbolos con su dirección 0x1000+
    for (const auto& parEtiq : etiquetasCodigo) {
        tablaSimbolosMemoria.push_back({parEtiq.first, parEtiq.second, ".text", "Etiqueta", 0});
    }

    // 3. Reemplazar los símbolos por sus direcciones hexadecimales en cada instrucción
    auto resolverOperando = [&](const string& op) -> string {
        if (op.empty() || esRegistroCPU(op)) return op;
        auto it = mapaDirecciones.find(op);
        if (it != mapaDirecciones.end()) {
            return formatearHex(it->second);
        }
        return op;
    };

    for (auto& inst : instrucciones) {
        inst.op1Direccion = resolverOperando(inst.op1);
        inst.op2Direccion = resolverOperando(inst.op2);
        inst.palabraBinaria = codificarInstruccionBinaria(inst);
    }
}

bool GeneradorCodigoMaquina::generarDesdeTAC(
    const vector<InstruccionTAC>& instrucciones,
    bool modoAcumulacionAditiva,
    const unordered_map<string, int>& valoresIniciales) {

    tacEntrada = instrucciones;
    if (!validarInstruccionesTAC(instrucciones)) {
        return false;
    }

    generarMapeoInstrucciones(instrucciones);
    aplicarOptimizacionInstrucciones(modoAcumulacionAditiva);
    resolverDireccionesYTablaSimbolos(codigoMaquinaOptimizado, valoresIniciales);
    resolverDireccionesYTablaSimbolos(codigoMaquinaMapeado, valoresIniciales);
    // Dejar la tabla final sincronizada con el código máquina optimizado
    resolverDireccionesYTablaSimbolos(codigoMaquinaOptimizado, valoresIniciales);
    return true;
}

bool GeneradorCodigoMaquina::configurarEntornoYGenerarEjecutable(
    const string& nombreArchivoEjecutable,
    const unordered_map<string, int>& valoresEntrada) {

    if (tieneErrores()) return false;

    // 1. Configurar cabecera y entorno de ejecución
    cabecera = CabeceraEjecutable();
    cabecera.tamanoCodigoBytes = static_cast<uint32_t>(codigoMaquinaOptimizado.size() * 4);
    uint32_t numVars = 0;
    for (const auto& s : tablaSimbolosMemoria) {
        if (s.categoria != "Etiqueta") numVars++;
    }
    cabecera.tamanoDatosBytes = numVars * 4;
    cabecera.totalSimbolos = static_cast<uint32_t>(tablaSimbolosMemoria.size());
    cabecera.libreriasCargadas = {
        "librigocore_runtime.rlib (Gestion de registros y ALU de 32 bits)",
        "libmem_Win32.rlib        (Asignacion de segmentos 0x1000, 0x2000, 0x3000)",
        "libio_console.rlib       (Entrada/Salida estandar del sistema objetivo)"
    };

    // 2. Inicializar memoria del procesador simulado con la tabla de símbolos y valoresEntrada
    memoriaEjecucion.clear();
    for (auto& sim : tablaSimbolosMemoria) {
        if (sim.categoria == "Etiqueta") continue;
        auto it = valoresEntrada.find(sim.simbolo);
        if (it != valoresEntrada.end()) {
            sim.valorInicial = it->second;
        }
        memoriaEjecucion[sim.direccion] = sim.valorInicial;
    }

    // 3. Ejecutar el código máquina en el procesador virtual (Ciclo Fetch - Decode - Execute)
    trazaEjecucion.clear();
    int R1 = 0;
    bool flagZero = false;
    int idx = 0;
    int maxCiclos = 250;
    int ciclo = 1;

    unordered_map<uint32_t, int> dirAIndice;
    for (size_t i = 0; i < codigoMaquinaOptimizado.size(); ++i) {
        dirAIndice[codigoMaquinaOptimizado[i].direccionMemoria] = static_cast<int>(i);
    }

    auto obtenerValorOperando = [&](const string& opSimbolico) -> int {
        if (esNumeroLiteral(opSimbolico)) {
            return static_cast<int>( stod(opSimbolico) );
        }
        auto itDir = mapaDirecciones.find(opSimbolico);
        if (itDir != mapaDirecciones.end()) {
            return memoriaEjecucion[itDir->second];
        }
        return 0;
    };

    while (idx >= 0 && idx < static_cast<int>(codigoMaquinaOptimizado.size()) && ciclo <= maxCiclos) {
        const auto& inst = codigoMaquinaOptimizado[idx];
        uint32_t pcActual = inst.direccionMemoria;
        string cambioMem = "--";
        int siguienteIdx = idx + 1;

        string textoInst = inst.mnemonico;
        if (!inst.op1Direccion.empty()) textoInst += " " + inst.op1Direccion;
        if (!inst.op2Direccion.empty()) textoInst += ", " + inst.op2Direccion;

        if (inst.mnemonico == "LOAD") {
            R1 = obtenerValorOperando(inst.op2);
            cambioMem = "R1 <- " + to_string(R1) + " (" + inst.op2 + ")";
        } else if (inst.mnemonico == "ADD") {
            int val = obtenerValorOperando(inst.op2);
            R1 += val;
            cambioMem = "R1 <- R1 + " + to_string(val) + " = " + to_string(R1);
        } else if (inst.mnemonico == "SUB") {
            int val = obtenerValorOperando(inst.op2);
            R1 -= val;
            cambioMem = "R1 <- R1 - " + to_string(val) + " = " + to_string(R1);
        } else if (inst.mnemonico == "MUL") {
            int val = obtenerValorOperando(inst.op2);
            R1 *= val;
            cambioMem = "R1 <- R1 * " + to_string(val) + " = " + to_string(R1);
        } else if (inst.mnemonico == "DIV") {
            int val = obtenerValorOperando(inst.op2);
            if (val != 0) R1 /= val;
            cambioMem = "R1 <- R1 / " + to_string(val) + " = " + to_string(R1);
        } else if (inst.mnemonico == "STORE") {
            uint32_t dirDest = mapaDirecciones[inst.op1];
            memoriaEjecucion[dirDest] = R1;
            cambioMem = "[" + formatearHex(dirDest) + " (" + inst.op1 + ")] <- " + to_string(R1);
        } else if (inst.mnemonico == "CMP") {
            int val = obtenerValorOperando(inst.op2);
            flagZero = (R1 == val);
            cambioMem = "ZF <- " + string(flagZero ? "1" : "0");
        } else if (inst.mnemonico == "JMP") {
            uint32_t dirSalto = mapaDirecciones[inst.op1];
            if (dirAIndice.count(dirSalto)) {
                siguienteIdx = dirAIndice[dirSalto];
                cambioMem = "PC <- " + formatearHex(dirSalto);
            }
        } else if (inst.mnemonico == "JZ") {
            if (R1 == 0) {
                uint32_t dirSalto = mapaDirecciones[inst.op1];
                if (dirAIndice.count(dirSalto)) siguienteIdx = dirAIndice[dirSalto];
                cambioMem = "Salto tomado -> " + formatearHex(dirSalto);
            }
        } else if (inst.mnemonico == "JNZ") {
            if (R1 != 0) {
                uint32_t dirSalto = mapaDirecciones[inst.op1];
                if (dirAIndice.count(dirSalto)) siguienteIdx = dirAIndice[dirSalto];
                cambioMem = "Salto tomado -> " + formatearHex(dirSalto);
            }
        } else if (inst.mnemonico == "JE") {
            if (flagZero) {
                uint32_t dirSalto = mapaDirecciones[inst.op1];
                if (dirAIndice.count(dirSalto)) siguienteIdx = dirAIndice[dirSalto];
                cambioMem = "Salto tomado -> " + formatearHex(dirSalto);
            }
        } else if (inst.mnemonico == "RET") {
            cambioMem = "Fin de subrutina (RET)";
            trazaEjecucion.push_back({ciclo++, pcActual, textoInst, R1, cambioMem});
            break;
        }

        trazaEjecucion.push_back({ciclo++, pcActual, textoInst, R1, cambioMem});
        idx = siguienteIdx;
    }

    // 4. Escribir el archivo ejecutable con cabecera + tabla de símbolos + código máquina
    ofstream archivo(nombreArchivoEjecutable);
    if (archivo.is_open()) {
        archivo << "================================================================================\n";
        archivo << "  ARCHIVO EJECUTABLE GENERADO - COMPILADOR RIGOPAPU C++\n";
        archivo << "================================================================================\n";
        archivo << "[CABECERA DEL EJECUTABLE (HEADER)]\n";
        archivo << "  Magic Number    : " << cabecera.magicNumber << "\n";
        archivo << "  Arquitectura    : " << cabecera.arquitectura << "\n";
        archivo << "  Entry Point (PC): " << formatearHex(cabecera.puntoEntrada) << "\n";
        archivo << "  Segmento .text  : " << formatearHex(cabecera.baseCodigo) << " (" << cabecera.tamanoCodigoBytes << " bytes) [" << cabecera.permisosCodigo << "]\n";
        archivo << "  Segmento .data  : " << formatearHex(cabecera.baseDatos) << " (" << cabecera.tamanoDatosBytes << " bytes) [" << cabecera.permisosDatos << "]\n";
        archivo << "  Segmento .result: " << formatearHex(cabecera.baseResultados) << " [" << cabecera.permisosDatos << "]\n\n";

        archivo << "[TABLA DE SIMBOLOS Y DIRECCIONES]\n";
        for (const auto& s : tablaSimbolosMemoria) {
            archivo << "  " << left << setw(14) << s.simbolo
                    << " -> " << formatearHex(s.direccion)
                    << " (" << s.segmento << ", " << s.categoria << ")\n";
        }

        archivo << "\n[SEGMENTO DE CODIGO MAQUINA (.text)]\n";
        for (const auto& inst : codigoMaquinaOptimizado) {
            string asmStr = inst.mnemonico + " " + inst.op1Direccion;
            if (!inst.op2Direccion.empty()) asmStr += ", " + inst.op2Direccion;
            archivo << "  " << formatearHex(inst.direccionMemoria) << " | "
                    << formatearHex(inst.palabraBinaria, 8) << " | "
                    << left << setw(24) << asmStr
                    << " # " << inst.comentario << "\n";
        }
        archivo.close();
    }

    return true;
}

bool GeneradorCodigoMaquina::tieneErrores() const {
    return !errores.empty();
}

const vector<ErrorCodigoMaquina>& GeneradorCodigoMaquina::obtenerErrores() const {
    return errores;
}

const vector<InstruccionMaquina>& GeneradorCodigoMaquina::obtenerCodigoMapeado() const {
    return codigoMaquinaMapeado;
}

const vector<InstruccionMaquina>& GeneradorCodigoMaquina::obtenerCodigoOptimizado() const {
    return codigoMaquinaOptimizado;
}

const vector<SimboloDireccionMemoria>& GeneradorCodigoMaquina::obtenerTablaSimbolosMemoria() const {
    return tablaSimbolosMemoria;
}

void GeneradorCodigoMaquina::imprimirMapeoInstrucciones() const {
    cout << "\n+------------------------------------------------------------------------------+\n";
    cout << "| 1.1. CODIGO INTERMEDIO DE ENTRADA (3 DIRECCIONES - TAC)                      |\n";
    cout << "+------+------------------------------+----------------------------------------+\n";
    cout << "| No.  | Instruccion Intermedia       | Descripcion                            |\n";
    cout << "+------+------------------------------+----------------------------------------+\n";
    int n = 1;
    for (const auto& t : tacEntrada) {
        if (t.esComentarioSeccion) continue;
        string com = t.comentario;
        if (com.size() > 38) com = com.substr(0, 35) + "...";
        cout << "| " << right << setw(3) << n++ << "  | "
             << left << setw(28) << t.codigo << " | "
             << left << setw(38) << com << " |\n";
    }
    cout << "+------+------------------------------+----------------------------------------+\n";

    cout << "\n+------------------------------------------------------------------------------------+\n";
    cout << "| 1.1. TRADUCCION A CODIGO MAQUINA (MAPEO DE INSTRUCCIONES DEL PROCESADOR)           |\n";
    cout << "+------+------------------------------+----------------------------------------------+\n";
    cout << "| No.  | Instruccion de Maquina (ASM) | Comentario / Accion en Procesador            |\n";
    cout << "+------+------------------------------+----------------------------------------------+\n";
    int m = 1;
    for (const auto& inst : codigoMaquinaMapeado) {
        string asmTexto = inst.mnemonico;
        if (!inst.op1.empty()) asmTexto += " " + inst.op1;
        if (!inst.op2.empty()) asmTexto += ", " + inst.op2;

        string com = "# " + inst.comentario;
        if (com.size() > 44) com = com.substr(0, 41) + "...";

        cout << "| " << right << setw(3) << m++ << "  | "
             << left << setw(28) << asmTexto << " | "
             << left << setw(44) << com << " |\n";
    }
    cout << "+------+------------------------------+----------------------------------------------+\n";
}

void GeneradorCodigoMaquina::imprimirOptimizacionInstrucciones() const {
    cout << "\n+------------------------------------------------------------------------------------+\n";
    cout << "| 1.2. CODIGO MAQUINA BASE (ANTES DE OPTIMIZAR INSTRUCCIONES DE MAQUINA)             |\n";
    cout << "+------+------------------------------+----------------------------------------------+\n";
    int b = 1;
    for (const auto& inst : codigoMaquinaBase) {
        string asmTexto = inst.mnemonico;
        if (!inst.op1.empty()) asmTexto += " " + inst.op1;
        if (!inst.op2.empty()) asmTexto += ", " + inst.op2;
        string com = "# " + inst.comentario;
        if (com.size() > 44) com = com.substr(0, 41) + "...";
        cout << "| " << right << setw(3) << b++ << "  | "
             << left << setw(28) << asmTexto << " | "
             << left << setw(44) << com << " |\n";
    }
    cout << "+------+------------------------------+----------------------------------------------+\n";

    cout << "\n+----------------------------------------------------------------------------------------------------+\n";
    cout << "| BITACORA DE OPTIMIZACION DE INSTRUCCIONES DE MAQUINA                                               |\n";
    cout << "+-------------------------------------+--------------------------------------------------------------+\n";
    cout << "| Regla de Optimizacion               | Transformacion Aplicada                                      |\n";
    cout << "+-------------------------------------+--------------------------------------------------------------+\n";
    if (bitacoraOptimizacion.empty()) {
        cout << "| (Sin reducciones adicionales)       | El codigo maquina ya se encuentra en su forma optima         |\n";
        cout << "+-------------------------------------+--------------------------------------------------------------+\n";
    } else {
        for (const auto& reg : bitacoraOptimizacion) {
            string orig = "Instruccion(es): " + reg.instruccionOriginal;
            if (orig.size() > 60) orig = orig.substr(0, 57) + "...";
            string det = "-> " + reg.detalleTransformacion;
            if (det.size() > 60) det = det.substr(0, 57) + "...";
            cout << "| " << left << setw(35) << reg.regla << " | "
                 << left << setw(60) << orig << " |\n";
            cout << "| " << left << setw(35) << " " << " | "
                 << left << setw(60) << det << " |\n";
            cout << "+-------------------------------------+--------------------------------------------------------------+\n";
        }
    }

    cout << "\n+------------------------------------------------------------------------------------+\n";
    cout << "| 1.2. CODIGO MAQUINA OPTIMIZADO (DESPUES DE OPTIMIZAR INSTRUCCIONES)                |\n";
    cout << "+------+------------------------------+----------------------------------------------+\n";
    cout << "| No.  | Instruccion Optimizada       | Comentario / Accion en Procesador            |\n";
    cout << "+------+------------------------------+----------------------------------------------+\n";
    int o = 1;
    for (const auto& inst : codigoMaquinaOptimizado) {
        string asmTexto = inst.mnemonico;
        if (!inst.op1.empty()) asmTexto += " " + inst.op1;
        if (!inst.op2.empty()) asmTexto += ", " + inst.op2;
        string com = "# " + inst.comentario;
        if (com.size() > 44) com = com.substr(0, 41) + "...";
        cout << "| " << right << setw(3) << o++ << "  | "
             << left << setw(28) << asmTexto << " | "
             << left << setw(44) << com << " |\n";
    }
    cout << "+------+------------------------------+----------------------------------------------+\n";

    int totalBase = static_cast<int>(codigoMaquinaBase.size());
    int totalOpt = static_cast<int>(codigoMaquinaOptimizado.size());
    int ahorradas = totalBase - totalOpt;
    double pct = (totalBase > 0) ? (100.0 * ahorradas / totalBase) : 0.0;
    cout << "  [METRICAS DE CODIGO MAQUINA]: Instrucciones base: " << totalBase
         << " | Instrucciones optimizadas: " << totalOpt
         << " | Reduccion: " << ahorradas << " instruccion(es) ("
         << fixed << setprecision(1) << pct << "%)\n";
}

void GeneradorCodigoMaquina::imprimirAsignacionDireccionesYTablaSimbolos() const {
    cout << "\n+----------------------------------------------------------------------------------------------------+\n";
    cout << "| 2.1. ASIGNACION DE DIRECCIONES DE MEMORIA AL CODIGO MAQUINA                                        |\n";
    cout << "+------------+------------+----------------------------+---------------------------------------------+\n";
    cout << "| Direccion  | Codigo Hex | Instruccion Direccionada   | Comentario                                  |\n";
    cout << "+------------+------------+----------------------------+---------------------------------------------+\n";

    for (const auto& inst : codigoMaquinaOptimizado) {
        string asmDir = inst.mnemonico;
        if (!inst.op1Direccion.empty()) asmDir += " " + inst.op1Direccion;
        if (!inst.op2Direccion.empty()) asmDir += ", " + inst.op2Direccion;

        string com = "# " + inst.comentario;
        if (!inst.etiqueta.empty()) {
            com = "# [" + inst.etiqueta + "] " + inst.comentario;
        }
        if (com.size() > 43) com = com.substr(0, 40) + "...";

        cout << "| " << left << setw(10) << (formatearHex(inst.direccionMemoria) + ":") << " | "
             << left << setw(10) << formatearHex(inst.palabraBinaria, 8) << " | "
             << left << setw(26) << asmDir << " | "
             << left << setw(43) << com << " |\n";
    }
    cout << "+------------+------------+----------------------------+---------------------------------------------+\n";

    cout << "\n+--------------------------------------------------------------+\n";
    cout << "| 2.2. TABLA DE SIMBOLOS DE DIRECCIONES DE MEMORIA             |\n";
    cout << "+------------------+-------------+------------+----------------+\n";
    cout << "| Simbolo          | Direccion   | Segmento   | Categoria      |\n";
    cout << "+------------------+-------------+------------+----------------+\n";
    for (const auto& sim : tablaSimbolosMemoria) {
        cout << "| " << left << setw(16) << sim.simbolo << " | "
             << left << setw(11) << formatearHex(sim.direccion) << " | "
             << left << setw(10) << sim.segmento << " | "
             << left << setw(14) << sim.categoria << " |\n";
    }
    cout << "+------------------+-------------+------------+----------------+\n";
}

void GeneradorCodigoMaquina::imprimirPreparacionYEjecucion(const string& rutaEjecutable) const {
    cout << "\n+----------------------------------------------------------------------------------------------------+\n";
    cout << "| 3.1. CREACION DE ARCHIVO EJECUTABLE Y 3.2. CONFIGURACION DEL ENTORNO DE EJECUCION                  |\n";
    cout << "+----------------------------------------------------------------------------------------------------+\n";
    cout << "| [CABECERA DEL EJECUTABLE]                                                                          |\n";
    cout << "|   Archivo Generado : " << left << setw(77) << rutaEjecutable << " |\n";
    cout << "|   Magic Number     : " << left << setw(77) << cabecera.magicNumber << " |\n";
    cout << "|   Arquitectura     : " << left << setw(77) << cabecera.arquitectura << " |\n";
    string lineaSegmentos = "Entry Point: " + formatearHex(cabecera.puntoEntrada) +
                            " | .text: " + formatearHex(cabecera.baseCodigo) + " (" + to_string(cabecera.tamanoCodigoBytes) + " B)" +
                            " | .data: " + formatearHex(cabecera.baseDatos) + " (" + to_string(cabecera.tamanoDatosBytes) + " B)" +
                            " | .result: " + formatearHex(cabecera.baseResultados);
    cout << "|   Mapa de Memoria  : " << left << setw(77) << lineaSegmentos << " |\n";
    cout << "| [CONFIGURACION DEL ENTORNO DE EJECUCION]                                                           |\n";
    cout << "|   Permisos .text   : " << left << setw(77) << cabecera.permisosCodigo << " |\n";
    cout << "|   Permisos .data   : " << left << setw(77) << cabecera.permisosDatos << " |\n";
    for (const auto& lib : cabecera.libreriasCargadas) {
        cout << "|   Libreria Cargada : " << left << setw(77) << lib << " |\n";
    }
    cout << "+----------------------------------------------------------------------------------------------------+\n";

    cout << "\n+----------------------------------------------------------------------------------------------------+\n";
    cout << "| 3.3. TRAZA DE EJECUCION EN EL PROCESADOR OBJETIVO (CICLO FETCH - DECODE - EXECUTE)                 |\n";
    cout << "+-------+----------+----------------------------+-------------+--------------------------------------+\n";
    cout << "| Ciclo | PC       | Instruccion Ejecutada      | Registro R1 | Efecto en Registros / Memoria RAM    |\n";
    cout << "+-------+----------+----------------------------+-------------+--------------------------------------+\n";
    for (const auto& paso : trazaEjecucion) {
        string efecto = paso.cambioMemoria;
        if (efecto.size() > 36) efecto = efecto.substr(0, 33) + "...";
        cout << "| " << right << setw(4) << paso.ciclo << "  | "
             << left << setw(8) << formatearHex(paso.pc) << " | "
             << left << setw(26) << paso.instruccionEnsamblador << " | "
             << right << setw(11) << paso.registroR1 << " | "
             << left << setw(36) << efecto << " |\n";
    }
    cout << "+-------+----------+----------------------------+-------------+--------------------------------------+\n";

    cout << "  [ESTADO FINAL DE MEMORIA TRAS LA EJECUCION]:\n";
    for (const auto& sim : tablaSimbolosMemoria) {
        if (sim.categoria == "Etiqueta") continue;
        auto it = memoriaEjecucion.find(sim.direccion);
        int valFinal = (it != memoriaEjecucion.end()) ? it->second : sim.valorInicial;
        cout << "    -> [" << formatearHex(sim.direccion) << "] "
             << left << setw(12) << sim.simbolo << " = " << valFinal
             << " (" << sim.categoria << ")\n";
    }
}

void GeneradorCodigoMaquina::imprimirErrores() const {
    if (errores.empty()) {
        cout << "  [+] Validacion de generacion de codigo maquina exitosa (0 errores).\n";
        return;
    }
    cout << "\n+----------------------------------------------------------------------------------------------------+\n";
    cout << "| [!] ERRORES DETECTADOS EN LA GENERACION DE CODIGO MAQUINA (PRUEBA INVALIDA)                        |\n";
    cout << "+-------+--------------------------+-------------------------+---------------------------------------+\n";
    cout << "| Tupla | Instruccion TAC          | Tipo de Error           | Diagnostico                           |\n";
    cout << "+-------+--------------------------+-------------------------+---------------------------------------+\n";
    for (const auto& err : errores) {
        string tup = err.tupla;
        if (tup.size() > 24) tup = tup.substr(0, 21) + "...";
        string msg = err.mensaje;
        if (msg.size() > 37) msg = msg.substr(0, 34) + "...";
        cout << "| " << right << setw(4) << err.indiceTupla << "  | "
             << left << setw(24) << tup << " | "
             << left << setw(23) << err.tipo << " | "
             << left << setw(37) << msg << " |\n";
    }
    cout << "+-------+--------------------------+-------------------------+---------------------------------------+\n";
    for (const auto& err : errores) {
        cout << "  [!] Detalle Tupla #" << err.indiceTupla << " ('" << err.tupla << "'): " << err.mensaje << "\n";
    }
}
