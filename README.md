# Juego de Palabras en C++ (estilo Scrabble) — Documentación técnica

Juego de consola para **dos jugadores** escrito en C++ sobre un tablero de **15 x 15** casillas. Los jugadores se turnan para escribir palabras con el teclado, moviendo un cursor con las flechas, usando solo las fichas que tienen en su **portafichas** (un atril de 7 fichas, como en el Scrabble real). El programa valida la forma de la palabra, la conexión con palabras anteriores y las fichas disponibles, pide una **confirmación manual** (`y`/`n`) de que la palabra existe, calcula el puntaje y lleva un registro persistente de victorias y puntos acumulados de cada jugador que ha usado el programa alguna vez.

> **Plataforma:** solo Windows. El código usa `conio.h` (`_getch`, `getch`) y `windows.h` (`GetConsoleScreenBufferInfo`, `GetFileAttributesA`, `MoveFileExA`).

> **Nota sobre esta versión del README:** se escribió leyendo el `main.cpp` real, línea por línea (1140 líneas / 1006 "loc" según el contador de GitHub, 38.1 KB). La revisión anterior de este documento se basó por error en una copia desactualizada del archivo (874 líneas) que no tenía el sistema de portafichas ni el registro de usuarios; ver la [sección 13](#13-qué-cambió-en-esta-revisión-del-readme).

---

## Tabla de contenido

1. [Resumen rápido](#1-resumen-rápido)
2. [Compilación y ejecución](#2-compilación-y-ejecución)
3. [Estructura del proyecto](#3-estructura-del-proyecto)
4. [Cómo se juega](#4-cómo-se-juega)
5. [Arquitectura general](#5-arquitectura-general)
6. [Librerías incluidas](#6-librerías-incluidas)
7. [Variables y estructuras globales](#7-variables-y-estructuras-globales)
8. [Referencia de funciones](#8-referencia-de-funciones)
9. [Modelo de datos: tablero, fichas y portafichas](#9-modelo-de-datos-tablero-fichas-y-portafichas)
10. [Reglas de validación en orden](#10-reglas-de-validación-en-orden)
11. [Limitaciones y errores conocidos](#11-limitaciones-y-errores-conocidos)
12. [Mejoras sugeridas](#12-mejoras-sugeridas)
13. [Qué cambió en esta revisión del README](#13-qué-cambió-en-esta-revisión-del-readme)

---

## 1. Resumen rápido

| Aspecto | Detalle |
|---|---|
| Lenguaje | C++ (un solo archivo: `main.cpp`, **1140 líneas en total / 1006 líneas de código**, según GitHub) |
| Jugadores | 2 por partida, con apodos **distintos entre sí**; el registro de resultados guarda a **todos** los nicknames que hayan jugado alguna vez |
| Tablero | `15 x 15`, cursor inicial en la casilla central `[7][7]` |
| Fichas | **98 por jugador** en total (igual que antes), pero ahora solo se puede jugar una letra si además está en el **portafichas** (atril de 7 fichas al azar) |
| Entrada | Flechas (mover), letras (escribir), **`1`** (cambiar el portafichas, una vez por turno), `Enter` (validar), `y`/`n` (confirmar palabra), `.` (terminar la partida) |
| Turnos | Sí, alternan J1 / J2 después de cada `Enter` |
| Puntaje | Por valor de letra (1 a 10 puntos), contando solo las letras **nuevas** de la palabra |
| Fin de partida | Cuando se presiona `.` o cuando a algún jugador le quedan menos de 10 fichas en total |
| Validación de existencia | **Manual**: el programa pregunta `y/n` (no hay diccionario interno) |
| Persistencia | `userdata.txt`: apodos activos + una lista de **todos** los nicknames históricos, cada uno con sus victorias y puntos totales acumulados |
| Guardado | **Atómico**: se escribe primero un archivo `.tmp` y luego se reemplaza `userdata.txt` con la API de Windows, para no dejar el archivo a medias si algo falla |
| Menú | 5 opciones: Iniciar, Cambiar nombre, Registro de resultados, **Instrucciones** (nueva), Salir |

---

## 2. Compilación y ejecución

Con `g++` (MinGW) en Windows:

```
g++ main.cpp -o juego
juego.exe
```

Notas del entorno (verificadas en el código):

- El juego limpia la pantalla con la secuencia ANSI `\033[2J\033[1;1H`. En **Windows Terminal** funciona directamente; en consolas antiguas (`cmd.exe` sin soporte ANSI) puede imprimir caracteres extraños.
- Las casillas vacías del tablero usan el carácter de código `219` (`█`) y el cursor el `206` (`╬`) de la página de códigos 437/850 de la consola.
- La `Ñ` se reconoce tanto en UTF-8 como en varias codificaciones de un solo byte de la consola (CP437/CP850/CP1252).
- El generador de números aleatorios (`rand`) se inicializa en `main` con la hora actual (`chrono::system_clock`), así que el portafichas sale distinto en cada ejecución.

---

## 3. Estructura del proyecto

```
scrabbleProyecto/
├── main.cpp        # Todo el código del juego
├── README.md       # Esta documentación
├── LICENSE
├── .gitignore
└── userdata.txt    # Se crea en la primera ejecución; formato nuevo (ver abajo)
```

`main.cpp` está dividido internamente por tres comentarios de sección: `// MENU FUNCTIONS`, `// IN-GAME FUNCTIONS` y `// FINAL FUNCTIONS`.

**Formato actual de `userdata.txt`** (versión `SCRABBLE_USERS_V2`):

```
SCRABBLE_USERS_V2
<apodo activo del jugador 1>
<apodo activo del jugador 2>
<cantidad de registros>
<nickname_1> <victorias_1> <puntos_totales_1>
<nickname_2> <victorias_2> <puntos_totales_2>
...
```

El archivo guarda **un registro por cada nickname distinto que haya usado el programa alguna vez**, no solo los dos jugadores activos. Si el programa encuentra un `userdata.txt` de un formato más viejo (con la etiqueta `SCRABBLE_USERS_V1`, o incluso el formato antiguo de solo 2 o 4 líneas sin ninguna etiqueta), lo **migra automáticamente** al formato nuevo la primera vez que lo lee.

---

## 4. Cómo se juega

1. Al abrir el programa por primera vez se piden los apodos de los dos jugadores; **deben ser distintos entre sí** (si se repite, se vuelve a pedir el segundo). Quedan guardados en `userdata.txt`.
2. En el menú se escribe `1` para jugar.
3. Al iniciar la partida, cada jugador recibe un **portafichas** de 7 fichas sacadas al azar de las que le quedan (ver [sección 9](#9-modelo-de-datos-tablero-fichas-y-portafichas)). El panel lateral del tablero muestra el portafichas del jugador en turno, por ejemplo `[A] [E] [CH] [S] [O] [R] [Z]`.
4. El jugador en turno **mueve el cursor** con las flechas y **escribe una letra por casilla** hasta formar su palabra.
5. En cualquier momento en que no haya una palabra a medio escribir, puede presionar **`1`** para **cambiar su portafichas** por uno nuevo al azar — solo **una vez por turno**.
6. Con **Enter** se valida la palabra:
   - Forma de la palabra, que no sea de una sola letra, que el jugador tenga fichas suficientes **y** que esas fichas estén en su portafichas actual.
   - El programa pregunta `Palabra: <palabra> ¿Desea aceptar esta palabra? y/n:`. Se responde `y` (existe) o `n` (se rechaza).
   - Si es aceptada y cumple las reglas de conexión, se suman los puntos, se descuentan las fichas y las letras quedan fijas en el tablero. Mensaje: `PALABRA VALIDA, FELICIDADES (+N puntos)`.
   - Sea válida o no, al terminar la jugada el portafichas del jugador que acaba de jugar se **vuelve a llenar al azar** automáticamente.
7. La partida termina cuando:
   - se presiona la tecla **punto (`.`)**, o
   - después de una palabra válida, la suma de fichas de algún jugador (no del portafichas, sino del total que le queda) cae por debajo de **10**.
8. Al terminar se anuncia al ganador (o empate); se suma una victoria y los puntos de la partida al registro histórico de cada jugador en `userdata.txt`.

El encabezado del tablero muestra siempre los apodos, de quién es el turno, los puntos de cada jugador y el último mensaje de validación.

**Valor de las letras** (sin cambios respecto a versiones anteriores)

| Puntos | Letras |
|---|---|
| 1 | A, E, I, L, N, O, R, S, T, U |
| 2 | D, G |
| 3 | B, C, M, P |
| 4 | F, H, V, Y |
| 5 | CH, Q |
| 8 | J, LL, Ñ, RR, X |
| 10 | Z |

---

## 5. Arquitectura general

```
main
 ├── dataVerification (ahora devuelve bool; si falla, main termina con return 1)
 │     ├── loadUserData ── findUserRecord, ensureUserRecord, saveUserData
 │     └── saveNickname  ── ensureUserRecord, saveUserData
 └── menu
      └── interactivePanel
           ├── (1) start ── dashboard, createBag, dashboardPrint, wValidation,
           │                 wordCrossesValidBoard, existingLetterMatchesWord,
           │                 hasNewLetter, wordScore, subtractingLetters, finalCout
           ├── (2) changeName ── saveNickname  → vuelve a menu()
           ├── (3) log                         → vuelve a menu()
           ├── (4) instructions                → vuelve a menu()
           ├── (5) exit
           └── otra → mensaje de opción inválida

wValidation ── bytesEnye, findLetterIndex
wordScore ── bytesEnye, singleLetterPoints
subtractingLetters ── bytesEnye, findLetterIndex, consumeTile
dashboardPrint ── bagLine
finalCout ── findUserRecord, saveUserData
```

Puntos clave de diseño:

- **No existe un ciclo de menú propiamente dicho.** Después de la opción 1, 5 o una opción inválida, las funciones retornan en cadena y el programa termina. Las opciones 2, 3 y 4 vuelven a mostrar el menú llamando a `menu()` de nuevo (recursión).
- **Dos tableros.** `table` (de trabajo) y `validTable` (confirmado), igual que en versiones anteriores — ver [sección 9](#9-modelo-de-datos-tablero-fichas-y-portafichas).
- **Tres niveles de "fichas".** `player1Cantidad`/`player2Cantidad` (98 fichas totales por jugador, se descuentan al jugar), `player1Bag`/`player2Bag` (el portafichas visible de 7 fichas, una muestra al azar de las que aún quedan) y lo que ya está fijo en el tablero. Jugar una letra exige que esté disponible en **ambos** niveles: en el total y en el portafichas actual.
- **Registro de usuarios persistente.** `userRecords` es un `vector<UserRecord>` en memoria con el historial de **todos** los nicknames usados alguna vez (victorias y puntos totales), cargado de `userdata.txt` al inicio y reescrito con `saveUserData` cada vez que cambia.
- **Guardado atómico.** `saveUserData` escribe primero en `userdata.txt.tmp` y solo si eso tiene éxito reemplaza el archivo real con `MoveFileExA`, para no corromperlo si el programa se interrumpe a la mitad de guardar.
- **Validación en dos capas**, igual que antes: `wValidation` revisa forma, fichas (ahora fichas totales *y* portafichas) y confirmación manual; después `start()` aplica las reglas de conexión con el tablero.

---

## 6. Librerías incluidas

| Librería | Para qué se usa en el código |
|---|---|
| `<iostream>` | `cout` / `cin` |
| `<fstream>` | `ifstream` / `ofstream` para leer y escribir `userdata.txt` |
| `<string>` | `string`, `to_string` |
| `<conio.h>` | `_getch()`, `getch()` (solo Windows) |
| `<cctype>` | `toupper()` / `tolower()` |
| `<vector>` | `vector<pair<int,int>>`, `vector<string>`, y el nuevo `vector<UserRecord>` |
| `<windows.h>` | `GetConsoleScreenBufferInfo` (ancho de consola), `GetFileAttributesA` y `MoveFileExA` (guardado atómico de `userdata.txt`) |
| `<cstdlib>` | `rand()` para elegir las fichas del portafichas al azar |
| `<chrono>` | `chrono::system_clock` para sembrar (`srand`) el generador aleatorio con la hora actual |

**Cambio respecto a versiones anteriores:** `<algorithm>` ya **no** se incluye; `sort()` fue reemplazado por una función propia, `sortedCoordinates` (inserción simple), así que el código ya no depende de esa cabecera.

---

## 7. Variables y estructuras globales

| Nombre | Tipo | Descripción |
|---|---|---|
| `table` | `char[15][15]` | Tablero de trabajo. Casilla vacía = `219`. |
| `validTable` | `unsigned char[15][15]` | Tablero confirmado. |
| `turn` | `int` (inicia en `1`) | Jugador en turno. |
| `pts1`, `pts2` | `int` (inician en `0`) | Puntos de cada jugador en la partida actual. |
| `playerName1`, `playerName2` | `string` | Apodos activos de los jugadores 1 y 2. |
| `lastValidationMessage` | `string` | Último mensaje de validación mostrado en el encabezado. |
| `UserRecord` | `struct` | `{ string nickname; int wins; long long totalPoints; }` — un registro histórico por jugador. |
| `userRecords` | `vector<UserRecord>` | **Nuevo.** Lista en memoria de todos los nicknames que han jugado, con sus victorias y puntos acumulados. Reemplaza a las antiguas variables `wins1`/`wins2`. |
| `USER_DATA_FILE` | `const string` | `"userdata.txt"`. |
| `LEGACY_USER_DATA_VERSION` | `const string` | `"SCRABBLE_USERS_V1"` — etiqueta del formato de archivo anterior (sin puntos totales), para poder seguir leyéndolo. |
| `USER_DATA_VERSION` | `const string` | `"SCRABBLE_USERS_V2"` — etiqueta del formato actual. |
| `words` | `string[28]` | Alfabeto de fichas (A, E, I, L, N, O, R, S, T, U, D, G, B, C, M, P, F, H, V, Y, CH, Q, J, LL, Ñ, RR, X, Z). Sin cambios; no incluye K ni W. |
| `WORD_COUNT` | `const int` | `28`. |
| `player1Cantidad`, `player2Cantidad` | `int[28]` | Fichas totales restantes por letra y por jugador. Mismos valores que antes; **suma verificada: 98 por jugador**. |
| `BAG_SIZE` | `const int` | `7` — tamaño del portafichas, como el atril del Scrabble real. |
| `player1Bag`, `player2Bag` | `int[7]` | **Nuevo.** El portafichas de cada jugador: cada posición guarda el índice (en `words`) de una ficha, o `-1` si la casilla está vacía (ya no quedan fichas de ningún tipo para esa posición). |

---

## 8. Referencia de funciones

Funciones en el orden en que aparecen en el archivo. No se asignan números de línea exactos salvo cuando se citan directamente del código.

### Manejo de usuarios y persistencia (nuevo bloque)

- **`int findUserRecord(const string& nickname)`** — busca un nickname en `userRecords` y devuelve su índice, o `-1` si no existe.
- **`void ensureUserRecord(const string& nickname)`** — si el nickname no está en `userRecords`, crea un registro nuevo para él con `wins = 0` y `totalPoints = 0`.
- **`bool saveUserData()`** — escribe todo `userRecords` (con la etiqueta `SCRABBLE_USERS_V2`, los dos apodos activos y la cantidad de registros) en `userdata.txt.tmp`, y si se escribió sin errores, reemplaza `userdata.txt` con `MoveFileExA`. Devuelve `false` si no pudo escribir o no pudo reemplazar el archivo.
- **`bool loadUserData(string& nickname1, string& nickname2)`** — lee `userdata.txt`. Según la primera palabra del archivo:
  - `SCRABBLE_USERS_V2` o `SCRABBLE_USERS_V1`: lee los dos apodos activos, la cantidad de registros, y luego cada registro (`nickname wins [totalPoints]`; los puntos totales solo si es V2). Por cada uno llama a `ensureUserRecord` y le suma lo leído.
  - Cualquier otro valor: lo interpreta como el formato antiguo sin etiqueta (2 apodos y, si los hay, dos números de victorias sueltos) y crea los registros correspondientes.
  - Al final llama a `saveUserData()` para reescribir el archivo ya en el formato nuevo (migración automática). Si esa escritura falla, devuelve `false`.
- **`bool saveNickname(const string& nickname1, const string& nickname2)`** — da de alta (si no existían) los dos nicknames con `ensureUserRecord`, actualiza `playerName1`/`playerName2` y llama a `saveUserData()`.
- **`bool dataVerification()`** — ya no recibe parámetros ni es `void`:
  1. Si `userdata.txt` se puede abrir: llama a `loadUserData`; si falla, imprime un error y **devuelve `false`** (el archivo no se toca). Si funciona, saluda con `Bienvenidos nuevamente` y devuelve `true`.
  2. Si no se pudo abrir pero `GetFileAttributesA` dice que el archivo **sí existe** (por ejemplo, sin permisos): imprime `No se pudo abrir userdata.txt.` y devuelve `false`.
  3. Si el archivo realmente no existe: pide los dos apodos (exigiendo que sean distintos), los guarda con `saveNickname` y devuelve `true`.
  - **`main` ahora puede terminar con código de salida `1`** si `dataVerification()` devuelve `false`.

### Menú y flujo

- **`int terminalWidth()`** — igual que antes.
- **`int main()`** — siembra `rand` con la hora actual, llama a `dataVerification()` (si falla, `return 1`), luego `menu()`, `return 0`.
- **`void instructions()`** — **nueva**. Imprime un resumen de las reglas básicas (tablero 15×15, que la primera letra va en `[7][7]`, que `1` cambia el portafichas una vez por turno, que `.` termina la partida), espera una tecla y vuelve a `menu()`.
- **`int menu()`** — igual que antes (logo "MENU" escalado a la consola), pero con **5 opciones** en vez de 4: `1` Iniciar, `2` Cambiar nombre, `3` Registro de resultados, `4` **Instrucciones**, `5` Salir.
- **`void interactivePanel(int x)`** — despachador: `1`→`start`, `2`→`changeName` + `menu`, `3`→`log`, `4`→`instructions`, `5`→`exit`, otro → opción inválida.
- **`string changeName()`** — pide dos apodos nuevos, exige que sean distintos entre sí, los guarda con `saveNickname` (que conserva el historial de victorias/puntos de ambos) y confirma si pudo guardar.
- **`void log()`** — **reescrita por completo**: ya no lee solo los dos jugadores activos, sino que recorre **todo** `userRecords` y muestra, para cada nickname que haya jugado alguna vez, sus victorias y puntos totales acumulados, marcando cuáles son "Jugador 1" / "Jugador 2" de la sesión actual.
- **`void exit()`** — solo imprime un mensaje; sigue sin terminar el proceso por sí misma.

### Portafichas (bloque nuevo)

- **`void createBag(int player)`** — llena el portafichas del jugador con hasta `BAG_SIZE` (7) fichas elegidas al azar, con una probabilidad proporcional a cuántas le quedan de cada letra (`player1Cantidad`/`player2Cantidad`), sin repetir más copias de las que realmente tiene. Si ya no le quedan suficientes fichas en total, deja algunas posiciones del portafichas en `-1` (vacías).
- **`string bagLine(int player)`** — arma el texto del portafichas para el panel, por ejemplo `[A] [E] [CH] [S] [O] [R] [Z]`, omitiendo las posiciones vacías.

### Tablero y consola

- **`void dashboard()`** — igual que antes (llena ambos tableros con `219`).
- **`void dashboardPrint(int cursorY, int cursorX)`** — igual que antes en la lógica del encabezado y el tablero (incluye los números de fila/columna), pero el panel lateral ahora tiene **más líneas**: a las 10 de siempre (título, jugador en turno, separador, 7 grupos de puntaje) se agregan un separador, el título `PORTAFICHAS (Jugador N)` y la línea con `bagLine(turn)` — 13 líneas en total.

### Validación, puntaje y fichas

- **`int findLetterIndex(string letter)`** — igual que antes (ya no toma el parámetro por referencia constante, pero el comportamiento es el mismo).
- **`void consumeTile(int player, int letterIndex)`** — **nueva**: centraliza el descuento de una ficha (antes se hacía con una referencia `int& remaining` dentro de cada función). Solo descuenta si `letterIndex >= 0` y todavía queda más de `0`.
- **`void subtractingLetters(string word, vector<pair<int,int>> letterPositions, int player)`** — misma lógica de tokens (Ñ, doble, simple) que antes, pero ahora delega el descuento en `consumeTile` en vez de manipular el arreglo directamente.
- **`int singleLetterPoints(char letter)`** — sin cambios.
- **`size_t bytesEnye(string word, size_t i)`** — reemplaza a la antigua `isEnye`. En vez de devolver `bool` y un parámetro de salida `bytes`, devuelve directamente la cantidad de bytes que ocupa la Ñ en la posición `i` (`2` en UTF-8, `1` en codificación de un byte), o `0` si no hay una Ñ ahí.
- **`vector<int> sortedCoordinates(vector<int> values)`** — **nueva**: ordena de menor a mayor con una inserción simple, para no depender de `<algorithm>`.
- **`bool hasNewLetter(vector<pair<int,int>> letterPositions)`** — sin cambios de lógica.
- **`int wordScore(string word, vector<pair<int,int>> letterPositions)`** — misma lógica de puntaje que antes.
- **`bool wordCrossesValidBoard(vector<pair<int,int>> letterPositions)`** — sin cambios de lógica.
- **`bool existingLetterMatchesWord(string word, vector<pair<int,int>> letterPositions)`** — sin cambios de lógica.
- **`bool wValidation(string word, vector<pair<int,int>> letterPositions, bool firstWord)`** — ver [sección 10](#10-reglas-de-validación-en-orden) para las reglas. **Cambio importante de firma:** `letterPositions` y `firstWord` ahora se reciben **por valor**, no por referencia. Como consecuencia, `wValidation` **ya no puede marcar `firstWord = false`** ella misma: eso ahora lo hace `start()` justo después de llamarla (`if (validPoint) firstWord = false;`). El resultado final es el mismo, pero la responsabilidad se movió de la función a quien la llama.

### Fin de partida

- **`void finalCout()`** — en vez de solo sumar un contador `wins1`/`wins2`, ahora: busca (o usa) el registro de cada jugador en `userRecords` con `findUserRecord`, les suma `pts1`/`pts2` a su `totalPoints` histórico, incrementa `wins` del ganador (o ninguno si hay empate), y guarda todo con `saveUserData()` (la versión atómica).

---

## 9. Modelo de datos: tablero, fichas y portafichas

Hay tres niveles de datos que gobiernan qué puede jugar un jugador, además del tablero:

| Nivel | Variable | Qué representa |
|---|---|---|
| Tablero de trabajo | `table` | Lo que se está escribiendo ahora mismo. |
| Tablero confirmado | `validTable` | Las letras ya aceptadas; `219` = libre. |
| Fichas totales | `player1Cantidad` / `player2Cantidad` | Cuántas fichas de cada letra le quedan al jugador en toda la partida (empiezan en 98 en total). Se descuentan solo cuando se juega una letra **nueva**. |
| Portafichas | `player1Bag` / `player2Bag` | Una muestra de 7 fichas sacadas al azar de las que aún le quedan (índices de `words`, o `-1` si está vacío). Es lo único que el jugador puede jugar **en este momento**. |

Para que una letra nueva se pueda jugar, `wValidation` exige que exista **tanto** en las fichas totales (`availableLetterCantidad`, copiado de `player1Cantidad`/`player2Cantidad`) **como** en el portafichas actual (`bagLetterCantidad`, contado de `player1Bag`/`player2Bag`). Si falta en el portafichas pero sí hay en el total, el mensaje es `La ficha X no esta en el portafichas del Jugador N.`; si ya no queda en el total, es `Ya no quedan fichas de la letra X para el Jugador N.`

El portafichas se vuelve a llenar al azar automáticamente **después de cada jugada** (válida o no) y, además, el jugador puede forzar un cambio con la tecla **`1`** una vez por turno (solo si no tiene una palabra a medio escribir). Rellenar el portafichas **no** descuenta nada de `player1Cantidad`/`player2Cantidad`: esos arreglos solo bajan cuando una letra realmente se juega y se confirma.

La relación entre `table` y `validTable` (qué se restaura cuando una palabra falla, qué significa que una letra sea "nueva") es idéntica a la de versiones anteriores del juego.

---

## 10. Reglas de validación en orden

`wValidation` revisa estas reglas **en este orden** y se detiene en la primera que falla:

| # | Regla | Mensaje |
|---|---|---|
| 1 | La palabra no está vacía | `La palabra no puede estar vacia.` |
| 2 | **Nueva.** La palabra no es de una sola letra (y una Ñ sola tampoco cuenta, aunque ocupe 2 bytes en UTF-8) | `Las palabras de una sola letra no son validas.` |
| 3 | No contiene espacio ni tabulación | `La palabra no puede tener espacios.` |
| 4 | Número de posiciones = longitud de la palabra | `La palabra no puede contener espacios ni posiciones vacias.` |
| 5 | Ninguna casilla repetida | `La palabra no puede repetir una casilla.` |
| 6 | Horizontal o vertical (no diagonal) | `La palabra no puede estar escrita en diagonal.` |
| 7 | Casillas consecutivas (ordenadas con `sortedCoordinates`) | `La palabra debe estar escrita en casillas consecutivas.` |
| 8 | Si es la primera palabra, pasa por `[7][7]` | `La primera palabra debe pasar por la casilla [7][7].` |
| 9 | Hay fichas totales de cada letra nueva | `Ya no quedan fichas de la letra X para el Jugador N.` |
| 10 | **Nueva.** Esa letra está, además, en el portafichas actual | `La ficha X no esta en el portafichas del Jugador N.` |
| 11 | El usuario confirma la palabra (`y`) | `PALABRA INVALIDA` (si `n`) / `PALABRA VALIDA, FELICIDADES` (si `y`) |

Después, **solo para palabras que no son la primera**, `start()` agrega, en este orden:

| # | Regla | Mensaje |
|---|---|---|
| 12 | Toca o cruza una letra ya confirmada (`wordCrossesValidBoard`) | `La palabra debe empezar o cruzarse con una palabra valida.` |
| 13 | Las letras reutilizadas coinciden (`existingLetterMatchesWord`) | `La letra nueva no coincide con una letra ya validada.` |
| 14 | Incluye al menos una letra nueva (`hasNewLetter`) | `La palabra debe incluir al menos una letra nueva.` |

---

## 11. Limitaciones y errores conocidos

**Flujo del programa**
- No hay un ciclo de menú: tras la opción 1, 5 o una opción inválida el programa termina. Las opciones 2, 3 y 4 usan recursión para volver a mostrar el menú.
- `exit()` no cierra el programa por sí misma, solo imprime un mensaje; sigue teniendo el mismo nombre que `std::exit`.
- La tecla **`1` ya no se puede escribir como carácter en el tablero**: siempre se interpreta como "cambiar portafichas", incluso si el jugador quisiera (sin sentido en Scrabble, pero posible antes) escribir el dígito `1` en una casilla.
- El turno cambia aunque la palabra sea inválida.
- Cualquier tecla que no sea flecha, `1`, Enter ni punto se trata como letra: espacio, Esc, Tab y Backspace (que no borra) se escriben en el tablero.
- La palabra que dispara el fin de partida (menos de 10 fichas totales) sí se puntúa y descuenta, pero sus letras no se copian a `validTable` porque el ciclo termina antes de llegar a ese paso.

**Reglas del juego**
- La existencia de la palabra la decide quien esté frente al teclado con `y`/`n`; no hay diccionario.
- La confirmación se pide **antes** de las reglas de conexión, por lo que puede preguntarse por una palabra que luego será rechazada igualmente.
- El orden en que se escribieron las letras no tiene que coincidir con el orden en el tablero (las coordenadas solo se ordenan para comprobar que sean consecutivas).
- No se revisan ni puntúan palabras secundarias formadas por adyacencia.
- `CH`, `LL` y `RR` se tratan como una sola ficha; si no hay fichas o no está esa combinación en el portafichas, no se puede jugar aunque sobren las letras sueltas.
- El portafichas se puede cambiar manualmente solo si `word` está vacío; si el jugador ya empezó a escribir una letra, debe terminar o fallar esa palabra antes de poder cambiarlo.
- Cambiar el portafichas con `1` no tiene límite de "fichas que sobran" como en el Scrabble real (no se valida contra una bolsa común): siempre se vuelve a muestrear de lo que al jugador le queda en total.
- No hay casillas de bonificación (doble/triple letra o palabra).

**Datos**
- `words` sigue sin incluir `K` ni `W`.
- El registro de usuarios (`userRecords`) crece indefinidamente: cualquier nickname que se haya usado alguna vez queda para siempre en `userdata.txt`, no hay forma de borrar un registro desde el programa.
- Si `userdata.txt` existe pero no se puede leer en un formato reconocible, `dataVerification` hace que el programa **no inicie** (en vez de sobrescribirlo como antes) — esto protege el archivo, pero también significa que un archivo corrupto bloquea el juego hasta que se corrija o se borre a mano.
- La confirmación `y`/`n` sigue leyendo con `cin >> char`, así que escribir varias letras dejará texto en el búfer para la siguiente pregunta.

**Interfaz**
- Los apodos no pueden contener espacios ni repetirse entre los dos jugadores activos.
- El cursor (`╬`) oculta la letra de la casilla donde está.
- El texto de `instructions()` tiene un paréntesis sin cerrar en el mensaje sobre el portafichas (detalle cosmético, no afecta la ejecución).

**Portabilidad**
- Solo Windows (`conio.h`, `windows.h`, y ahora también `GetFileAttributesA`/`MoveFileExA` de la misma API).

---

## 12. Mejoras sugeridas

1. **Bucle de menú** con `while` en lugar de recursión, y salida real con `return` o `std::exit(0)`.
2. **Permitir borrar o fusionar registros** de `userRecords` desde el menú (por ejemplo, si alguien se equivocó al escribir su nickname).
3. **Mostrar cuántas fichas le quedan en total** junto al portafichas, para que el jugador sepa si se está quedando sin una letra.
4. **Dejar cambiar el portafichas aunque haya una letra a medio escribir**, descartando esa letra en vez de bloquear el cambio.
5. **Diccionario automático** para la validación de existencia, dejando `y`/`n` solo como respaldo.
6. **Casillas de bonificación** (doble/triple letra y palabra).
7. **Palabras secundarias** formadas por adyacencia.
8. **Renombrar** `log` y `exit` para evitar el choque de nombres con la librería estándar.
9. **Dividir el código** en varios archivos (persistencia, portafichas, validación, puntaje, interfaz); la lógica de tokens (Ñ, CH, LL, RR) sigue repetida en varias funciones.
10. **Incluir K y W** en `words`, o impedir explícitamente su uso.
11. **Manejar mejor la corrupción de `userdata.txt`**: ofrecer reiniciar el archivo en vez de bloquear el inicio del programa.

---

## 13. Qué cambió en esta revisión del README

La revisión anterior de este documento (la que decía "874 líneas") se basó en una copia de `main.cpp` que **no tenía** el sistema de portafichas ni el registro histórico de usuarios — evidentemente una versión anterior a los cambios que ya tenías en tu copia local. Esta versión se rehízo leyendo directamente el archivo real que subiste (1140 líneas), y documenta como nuevo todo lo que no existía antes:

- El **portafichas** (`player1Bag`/`player2Bag`, `createBag`, `bagLine`, la tecla `1`, las reglas 9 y 10 de validación).
- El **registro de usuarios persistente** (`UserRecord`, `userRecords`, `findUserRecord`, `ensureUserRecord`) con puntos totales acumulados, en lugar de los antiguos `wins1`/`wins2`.
- El **guardado atómico** de `userdata.txt` (`saveUserData` con archivo `.tmp` y `MoveFileExA`).
- La **migración automática** de formatos antiguos de `userdata.txt` (`loadUserData`).
- La opción de menú **Instrucciones** y la función `instructions()`.
- La exigencia de **apodos distintos** entre los dos jugadores.
- El rechazo de **palabras de una sola letra**.
- El reemplazo de `isEnye` por `bytesEnye` y de `sort()` (de `<algorithm>`, ya no incluida) por `sortedCoordinates`.
- El cambio de firma de `wValidation` (parámetros por valor en vez de por referencia) y el traslado de `firstWord = false` a `start()`.

Todo lo que no se menciona aquí (reglas de forma de la palabra, valor de las letras, el modelo `table`/`validTable`, el ciclo general del menú) se revisó contra el código y sigue siendo igual que antes.