# Guion para el Video Descriptivo y Demostrativo (YouTube)
## **Actividad 11.- Código Intermedio para Control de Flujo**

> [!IMPORTANT]
> **Checklist obligatorio antes de iniciar la grabación (OBS Studio / Xbox Game Bar / Camtasia):**
> - **Duración máxima:** Menos de 5 minutos (este guion está diseñado para durar **~4:00 a 4:20 minutos**).
> - **Resolución mínima:** 720p (recomendado 1080p).
> - **Pantalla completa:** Grabar **toda la pantalla** asegurando que la **barra de tareas de Windows con la fecha y hora del equipo sea visible** en todo momento.
> - **Legibilidad del código:** Hacer zoom en VS Code (`Ctrl` + `+`) para que el código y la terminal se lean con total claridad.

---

## **1. Introducción y Objetivo de la Práctica (0:00 – 0:45)**

**En pantalla:**  
Mostrar VS Code abierto en [`AnalizadorLexicoCPP.cpp`](file:///C:/Users/Yahir/OneDrive/Documentos/GitHub/CompiladorRigopapuCplusplus/AnalizadorLexicoCPP.cpp) donde se vean los nombres de los integrantes en los comentarios iniciales y la función `imprimirIntegrantes`.

**Diálogo sugerido:**
> *"Hola, buen día profesor Rigoberto Cárdenas Larios. Somos el equipo conformado en orden alfabético por primer apellido por:*
> 1. *Anguiano Garcia Angel Yahir Guadalupe,*
> 2. *Figueroa Robles Axel Israel,*
> 3. *Molina Alvarado Alvaro Moises, y*
> 4. *Torres Martinez Miguel Angel.*
>
> *En este video presentaremos la **Actividad 11: Código Intermedio para Control de Flujo**.*  
> *El **objetivo** de esta práctica es transformar las estructuras condicionales (`if`, `else`, `switch`), los bucles (`while`, `for`, `do-while`), así como la definición y llamada de métodos y clases del código fuente en una **representación de código intermedio de tres direcciones (TAC)** que gestione las variables temporales, etiquetas y saltos de flujo de control."*

---

## **2. Explicación de la Arquitectura y Reglas de Traducción (0:45 – 2:30)**

### **A) Generador de Código Intermedio y Tabla de Símbolos (0:45 – 1:15)**
**En pantalla:**  
Cambiar a [`GeneradorCodigoIntermedio.hpp`](file:///C:/Users/Yahir/OneDrive/Documentos/GitHub/CompiladorRigopapuCplusplus/GeneradorCodigoIntermedio.hpp) y luego a [`TablaSimbolos.hpp`](file:///C:/Users/Yahir/OneDrive/Documentos/GitHub/CompiladorRigopapuCplusplus/TablaSimbolos.hpp).

**Diálogo sugerido:**
> *"Como se observa al inicio de cada clase, hemos incluido los nombres completos del equipo en orden alfabético. En `GeneradorCodigoIntermedio.hpp` implementamos la clase `GeneradorCodigoIntermedio`, la cual administra los contadores de variables temporales (`t1, t2...`), las etiquetas de salto (`L1, L2...`) y una pila de buffers (`pilaBuffers`) que nos permite reordenar bloques de instrucciones en estructuras como `if-else`, `switch` y `for`.*  
> *Además, en `TablaSimbolos.hpp` extendimos la tabla de símbolos para registrar clases, objetos instanciados y la etiqueta de inicio asignada a cada método."*

### **B) Reglas para Condicionales (`if-else` y `switch`) (1:15 – 1:45)**
**En pantalla:**  
Mostrar [`AnalizadorSintactico.cpp`](file:///C:/Users/Yahir/OneDrive/Documentos/GitHub/CompiladorRigopapuCplusplus/AnalizadorSintactico.cpp) en las funciones `sentenciaIf()` (línea 397) y `sentenciaSwitch()` (línea 458).

**Diálogo sugerido:**
> *"En `AnalizadorSintactico.cpp` implementamos las reglas de traducción dirigida por la sintaxis:*
> - *Para **`if-else`** (`sentenciaIf`), primero se evalúa la condición relacional en un temporal `t1 = x > y`. Luego emitimos el salto condicional `if t1 goto L1`; si es falso, se ejecuta directamente el bloque `else` seguido de `goto L2` para saltar al final, y en `L1:` se coloca el bloque `if`, cerrando en `L2:`.*
> - *Para **`switch`** (`sentenciaSwitch`), se guarda el valor evaluado en un temporal `t2 = n`, se emiten las comparaciones `if t2 == valor goto L_caso` para cada `case` y un `goto` hacia la etiqueta `default`. Después se emiten los cuerpos de cada caso convirtiendo cada `break` en un salto incondicional `goto` hacia la etiqueta final del `switch`."*

### **C) Reglas para Bucles (`while`, `for`, `do-while`), Métodos y Clases (1:45 – 2:30)**
**En pantalla:**  
Desplazar en [`AnalizadorSintactico.cpp`](file:///C:/Users/Yahir/OneDrive/Documentos/GitHub/CompiladorRigopapuCplusplus/AnalizadorSintactico.cpp) hacia `sentenciaWhile()`, `sentenciaFor()`, `sentenciaDoWhile()`, `declaracionMetodo()` y `declaracionObjetoOExpresion()`.

**Diálogo sugerido:**
> - *"En el bucle **`while`**, emitimos la etiqueta de inicio `L1:`, evaluamos la condición en un temporal y usamos `if not t goto L2` para salir si es falsa; al final del cuerpo emitimos `goto L1` y la etiqueta de salida `L2:`.*
> - *En el bucle **`for`**, primero emitimos la inicialización antes del ciclo, luego la etiqueta de inicio, la evaluación de condición con `if not t goto L_fin`, el cuerpo del ciclo, enseguida vaciamos el buffer con el incremento de la variable de control, saltamos al inicio y colocamos la etiqueta de salida.*
> - *En **`do-while`**, colocamos la etiqueta al inicio, ejecutamos el cuerpo y al final evaluamos la condición emitiendo `if t goto L_inicio`.*
> - *Para **métodos y clases**, cada método recibe una etiqueta de entrada (`L_metodo:`), evalúa su expresión de retorno y emite `return t`. Al llamarse un método, los argumentos se asignan a temporales, se emite `call L_metodo` y el resultado se recibe con `return_value`. Finalmente, al instanciar un objeto con `new`, se emite `obj = create NombreClase` y sus llamadas invocan la etiqueta del método de la clase."*

---

## **3. Demostración del Programa en Ejecución: Pruebas Válidas e Inválidas (2:30 – 4:10)**

**En pantalla:**  
Abrir la terminal integrada en VS Code y ejecutar `.\compilador.exe`. Recorrer la salida desde el inicio hasta el final.

**Diálogo sugerido:**
> *"Ahora ejecutamos nuestro compilador en la terminal. Como primera instrucción del `main`, se imprimen nuestros nombres completos en orden alfabético por primer apellido:*
> - *En la **Prueba 1 (Válida - Condicionales)**, vemos cómo el bloque `if (x > y)` genera `t1 = x > y`, `if t1 goto L1`, la asignación del `else`, `goto L2`, `L1: z = x` y `L2:`. Enseguida, el `switch (n)` genera `t2 = n`, los saltos `if t2 == 1 goto L3`, `if t2 == 2 goto L4`, `goto L5` para el `default`, y cada caso con su `goto L6` correspondiente al `break`.*
> - *En la **Prueba 2 (Válida - Bucles)**, observamos el código de tres direcciones para el `while` con las etiquetas `L1` y `L2`, el bucle `for` con su inicialización `k = 0`, condición `t2 = k < 5`, suma `t3 = sum + k`, incremento `k = k + 1` y etiquetas `L3` y `L4`, y el `do-while` con verificación al final en `L5`.*
> - *En la **Prueba 3 (Válida - Métodos y Clases)**, vemos la definición del método `suma` en `L1:` con `t1 = a + b` y `return t1`, su llamada asignando los parámetros a `t2 = 5` y `t3 = 3`, `call L1` y `result = return_value`. Asimismo, para la clase `Persona`, se define `celebrarCumpleaños` en `L2:`, se instancia el objeto con `obj = create Persona` y se invoca con `call L2`.*
> - *En la **Prueba 4 (Inválida)**, demostramos la validación semántica de todas las reglas: detecta retorno incompatible, condiciones no booleanas en `if`, `while`, `for` y `do-while`, `case` con tipo incompatible y duplicado en `switch`, `break` fuera de contexto, llamada a método con tipos y número de argumentos incorrectos, e instanciación de una clase no declarada (`Vehiculo`), abortando la generación de código intermedio.*
> - *Finalmente, como última instrucción del `main`, se vuelven a imprimir nuestros nombres completos en orden alfabético. ¡Muchas gracias por su atención!"*
