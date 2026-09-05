cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(AnalizadorLexicoCPP)

# Especificar el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Agregar los archivos fuente para crear el ejecutable
add_executable(AnalizadorLexicoCPP
    AnalizadorLexicoCPP/AnalizadorLexico.cpp
    AnalizadorLexicoCPP/AnalizadorLexico.h
    AnalizadorLexicoCPP/AnalizadorLexicoCPP.cpp
)
