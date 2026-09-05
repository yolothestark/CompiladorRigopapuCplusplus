# Compilador Clase Trductores de lenguajes en C++

Este proyecto es un Analizador Léxico desarrollado en C++ como parte de la materia **Traductores de Lenguaje**. El programa procesa código fuente de C++ simulado y lo descompone en tokens léxicos, validando la sintaxis básica e identificando errores.

## Integrantes del Equipo

 1. Anguiano Garcia Angel Yahir Guadalupe
 2. Figueroa Robles Axel Israel
 3. Torres Martinez Miguel Angel
 4. Molina Alvarado Alvaro Moises

*(Nota: Integrantes listados en orden alfabético por su primer apellido)*

## Características Principales

- **Reconocimiento de Tokens:** Identifica palabras clave, variables, literales numéricos, cadenas de texto, caracteres sencillos, símbolos de puntuación y todo tipo de operadores (aritméticos, lógicos, relacionales, incremento, etc).
- **Gestión Avanzada de Errores Léxicos:** 
  -  Detecta **caracteres no reconocidos** (`@`, `$`, etc.) en lugar de procesarlos incorrectamente.
  -  Identifica **identificadores mal formados** (por ejemplo, iniciar una variable con un número: `123invalido`).
  -  Atrapa **literales numéricos incorrectos** (por ejemplo, múltiples puntos decimales: `12.34.56`).
  -  Señala **cadenas mal formadas** (un string de texto que no tiene sus comillas de cierre `" ` antes del final de la línea).
- **Ubicación Exacta:** El analizador cuenta automáticamente los espacios y saltos de línea para indicarle al usuario la **Línea y Columna** precisa tanto de los tokens válidos como de todos los errores.

##  Cómo compilar y ejecutar

El proyecto mantiene total compatibilidad con **Visual Studio 2022** (abriendo el archivo `.sln`) y está configurado de manera nativa para ejecutarse mediante **Visual Studio Code**.

### En Visual Studio Code:
El proyecto viene preconfigurado (`.vscode/`) para el compilador **g++ (MinGW)** y el depurador **GDB**.

1. Abre la carpeta del proyecto en Visual Studio Code.
2. Abre el archivo principal `AnalizadorLexicoCPP/AnalizadorLexicoCPP/AnalizadorLexicoCPP.cpp`.
3. Presiona la tecla **F5** (o ve al menú superior `Ejecutar > Iniciar depuración`).
4. Visualiza los resultados de la tabla y los errores abriendo la pestaña **Terminal** en la parte inferior de tu pantalla.

---
*Prácticas: Definición de tokens y expresiones regulares., gestión de errores léxicos.*