# Juego de Palabras en C++ (estilo Scrabble)

Este es un juego de consola para **dos jugadores**, hecho en C++. Se juega en un tablero de 15 x 15 casillas. El jugador mueve un cursor con las flechas, escribe letras y, al final, el programa revisa si la palabra formada es válida.

> **Importante:** el programa usa la librería `conio.h`, así que solo funciona en **Windows**.

---

## 1. ¿Qué hace el programa?

1. Pide los apodos (nicknames) de los dos jugadores la primera vez que se abre.
2. Guarda esos apodos en un archivo para no volver a pedirlos.
3. Muestra un menú con cuatro opciones.
4. Si se elige jugar, muestra el tablero, deja escribir una palabra y la valida.

---

## 2. Archivos que usa

- **main.cpp**: todo el código del juego.
- **userdata.txt**: lo crea el programa solo. Guarda los dos apodos, uno por línea.

---

## 3. Cómo compilar y ejecutar

En Windows, con el compilador g++ instalado, escribe en la terminal:

```
g++ main.cpp -o juego
juego.exe
```

---

## 4. El menú

Al iniciar, el programa muestra el logo y estas opciones:

| Número | Opción | ¿Qué hace? |
| --- | --- | --- |
| 1 | Iniciar Juego | Abre el tablero y empieza la partida |
| 2 | Cambiar nombre | Permite escribir apodos nuevos |
| 3 | Mostrar Registro de resultados | Todavía **no está hecha** (dice "FUNCION EN DESARROLLO") |
| 4 | Salir del juego | Muestra "SALIENDO DEL PROGRAMA..." y termina |

Si escribes un número que no está en la lista, el programa avisa que la opción es inválida.

---

## 5. Cómo se juega

1. Elige la opción **1** en el menú.
2. Aparece un tablero de 15 x 15. El cursor se ve como un símbolo especial y empieza en el centro (casilla fila 7, columna 7).
3. **Muévete** con las flechas del teclado. El cursor no se sale del tablero.
4. **Escribe una letra** y se coloca en la casilla donde está el cursor.
5. Muévete a la siguiente casilla y escribe la siguiente letra, hasta formar la palabra.
6. Presiona **Enter** para que el programa revise la palabra.
7. Presiona la tecla **punto (.)** si quieres detener el programa.

---

## 6. Reglas para que una palabra sea válida

Cuando presionas Enter, el programa revisa estas reglas en orden:

1. La palabra **no puede estar vacía**.
2. La palabra **no puede tener espacios**.
3. Cada letra debe estar en su propia casilla (no puede haber casillas vacías en medio).
4. **No se puede repetir una casilla**: no vale escribir dos letras en el mismo lugar.
5. La palabra debe ir **en línea recta**: horizontal o vertical. **No se permiten diagonales**.
6. Las letras deben estar en **casillas consecutivas**, pegadas una a la otra, sin saltos.
7. La **primera palabra del juego** debe pasar por la casilla central **\[7\]\[7\]**.
8. La palabra debe existir en el **diccionario** del programa.

Si falla alguna regla, el programa muestra un mensaje explicando el error. Si todo está bien y la palabra está en el diccionario, muestra **"PALABRA VALIDA, FELICIDADES"**. Si no está en el diccionario, muestra **"PALABRA INVALIDA"**.

---

## 7. Explicación del código, parte por parte

### Librerías del inicio

- `iostream`: para mostrar texto y leer lo que escribe el usuario.
- `fstream`: para leer y escribir archivos (se usa para guardar los apodos).
- `string`: para trabajar con texto.
- `conio.h`: para leer teclas al instante, sin esperar Enter (solo Windows).
- `cctype`: herramientas para letras y caracteres.
- `vector`: listas que pueden crecer (se usa para guardar las posiciones de las letras).
- `algorithm`: para ordenar listas.

### Variables globales (las que sirven en todo el programa)

- `table`: el tablero. Es una cuadrícula de 15 x 15 caracteres.
- `words`: la lista de letras del juego (A, E, I, L, N, O, R, S, T, U, etc., incluyendo Ch, LL, Ñ y RR).
- `player1Cantidad` y `player2Cantidad`: cuántas fichas de cada letra tendría cada jugador. **Por ahora están definidas pero no se usan** en el juego.

### `main()`: el punto de partida

Hace dos cosas: primero llama a `dataVerification` para revisar los apodos, y luego llama a `menu` para mostrar el menú.

### `saveNickname`: guardar los apodos

Abre el archivo `userdata.txt` y escribe ahí los dos apodos, uno por línea. Si no puede abrir el archivo, avisa con un mensaje.

### `dataVerification`: revisar si ya hay apodos

Intenta leer `userdata.txt`.

- **Si encuentra los dos apodos**: da la bienvenida con los nombres guardados.
- **Si no los encuentra** (primera vez): pide los dos apodos, los guarda con `saveNickname` y da la bienvenida.

### `menu`: el menú principal

Dibuja el logo con letras, muestra las cuatro opciones, lee el número que escribe el usuario y se lo pasa a `interactivePanel`.

### `interactivePanel`: decide qué hacer según la opción

Funciona como un semáforo que dirige al jugador:

- Opción 1: llama a `start` (jugar).
- Opción 2: llama a `changeName` y luego vuelve a mostrar el menú.
- Opción 3: llama a `log` (en desarrollo).
- Opción 4: llama a `exit` (salir).
- Cualquier otra: avisa que la opción es inválida.

### `changeName`: cambiar los apodos

Pide dos apodos nuevos y los guarda con `saveNickname`, reemplazando los anteriores.

### `log` y `exit`

- `log`: solo muestra "FUNCION EN DESARROLLO". Falta programarla.
- `exit`: solo muestra "SALIENDO DEL PROGRAMA...".

### `dashboard`: preparar el tablero

Llena las 225 casillas del tablero con un bloque sólido (el símbolo número 219). Así se ve un tablero "vacío".

### `dashboardPrint`: dibujar el tablero

Recorre todas las casillas y las muestra en pantalla. En la casilla donde está el cursor dibuja un símbolo especial (el número 206), así sabes dónde estás.

### `start`: el juego en sí

Es la parte más importante. Funciona así:

1. Prepara el tablero y pone el cursor en el centro (7, 7).
2. Entra a un ciclo que se repite esperando teclas.
3. **Si es una flecha**: mueve el cursor (solo si no se sale del tablero) y vuelve a dibujar todo.
4. **Si es el punto (.)**: dice "Programa detenido." y termina.
5. **Si es Enter**: pasa la palabra a minúsculas y llama a `wValidation` para revisarla. Luego termina el ciclo.
6. **Cualquier otra tecla**: la guarda como letra en la casilla del cursor, la agrega a la palabra, anota su posición y vuelve a dibujar el tablero.

Para que la limpieza de pantalla funcione, el programa usa un código especial que borra la terminal cada vez que se redibuja.

### `wValidation`: revisar la palabra

Recibe la palabra, las posiciones de cada letra y si es la primera palabra del juego. Contiene un **diccionario de 520 palabras** en español, organizado por letra (de la A a la Z, 20 palabras por letra). Revisa las reglas de la sección 6 y, al final, busca la palabra en el diccionario. Devuelve verdadero si es válida y falso si no.

---

## 8. Cosas que faltan o que se pueden mejorar

- La opción 3 (registro de resultados) está sin programar.
- No hay puntaje: las variables de puntos existen, pero no se usan.
- Las cantidades de fichas por jugador (`player1Cantidad` y `player2Cantidad`) todavía no se usan.
- No hay turnos entre los dos jugadores: el juego termina después de validar una sola palabra.
- El diccionario solo tiene 520 palabras, así que muchas palabras reales serán marcadas como inválidas.
- La opción "Ch", "LL" y "RR" del listado de letras no se usa en el tablero, que guarda un solo carácter por casilla.
- Solo funciona en Windows por usar `conio.h`.
