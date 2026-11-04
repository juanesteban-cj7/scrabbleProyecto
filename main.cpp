#include <iostream>
#include <fstream> 
#include <string>
#include <conio.h>
#include <cctype>
#include <vector>
#include <algorithm>
#include <windows.h>

using namespace std;

char table[15][15]; 
unsigned char validTable[15][15];

int turn = 1; 
int pts1 = 0;
int pts2 = 0; // Puntaje local
string playerName1 = "Jugador 1";
string playerName2 = "Jugador 2";
string lastValidationMessage = "";

string words[] = {"A", "E", "I", "L", "N", "O", "R", "S", "T", "U", "D", "G", "B", "C", "M", "P", "F", "H", "V", "Y", "CH", "Q", "J", "LL", "Ñ", "RR", "X", "Z"}; 
int player1Cantidad[] = {12, 12, 6, 4, 5, 9, 5, 6, 4, 5, 5, 2, 2, 4, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 
int player2Cantidad[] = {12, 12, 6, 4, 5, 9, 5, 6, 4, 5, 5, 2, 2, 4, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; 

//MENU FUNCTIONS

int terminalWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80;
}

void saveNickname(const string& nickname1, const string& nickname2);
void dataVerification(string& nickname1, string& nickname2);
int menu();
void start();
string changeName();
void log();
void exit();
void interactivePanel(int x);

// IN-GAME FUNCTIONS
void dashboard();
void dashboardPrint(int cursorY, int cursorX);
bool wValidation(string word, const vector<pair<int, int>>& letterPositions, bool& firstWord);
bool wordCrossesValidBoard(const vector<pair<int, int>>& letterPositions);
bool existingLetterMatchesWord(const string& word, const vector<pair<int, int>>& letterPositions);

int main() {
    string nickname1, nickname2;
    dataVerification(nickname1, nickname2);
	menu();
	return 0;
}

void saveNickname(const string& nickname1, const string& nickname2){
    ofstream userFile("userdata.txt");

    if(userFile.is_open()){
        userFile << nickname1 << '\n' << nickname2 << '\n';
        userFile.close();
    } else{
        cout << "No se pudo guardar el nickname \n";
    }
}

void dataVerification(string& nickname1, string& nickname2){
    ifstream userFile("userdata.txt");

    if(userFile >> nickname1 && userFile >> nickname2) {
        cout << "Bienvenidos nuevamente, " << nickname1 << " y " << nickname2 << "\n";
    } else{
        cout << "Primera vez que ejecutas el programa. \n";
        cout << "Ingresa tu nickname (User 1): ";
        cin >> nickname1;
        cout << "Ingresa tu nickname (User 2): ";
        cin >> nickname2;

        saveNickname(nickname1, nickname2);

        cout << "Bienvenidos, " << nickname1 << " y " << nickname2 << "\n";
    }

    playerName1 = nickname1;
    playerName2 = nickname2;
    userFile.close();
}

int menu() {
    int option;

    const string letters[4][5] = {
        {"10001", "11011", "10101", "10001", "10001"}, // M
        {"11111", "10000", "11110", "10000", "11111"}, // E
        {"10001", "11001", "10101", "10011", "10001"}, // N
        {"10001", "10001", "10001", "10001", "01110"}  // U
    };
    const char symbols[4] = {'#', '@', '%', '&'};   // un simbolo por letra

    const int width = terminalWidth() - 1;          // evita salto de linea extra
    const int totalCols = 4 * 5 + 3;                // 4 letras + 3 espacios = 23
    int rowRepeat = (width / totalCols) / 2;        // altura proporcional al ancho
    if (rowRepeat < 1) rowRepeat = 1;

    cout << string(width, '=') << "\n";

    for (int row = 0; row < 5; row++) {
        string line;
        for (int x = 0; x < width; x++) {
            int src = x * totalCols / width;        // columna original (0 a 22)
            int letter = src / 6;                   // cada letra: 5 columnas + 1 espacio
            int col = src % 6;

            if (letter < 4 && col < 5 && letters[letter][row][col] == '1') {
                line += symbols[letter];
            } else {
                line += ' ';
            }
        }
        for (int r = 0; r < rowRepeat; r++) {
            cout << line << "\n";
        }
    }

    cout << string(width, '=') << "\n";

    cout << "Seleccione la opcion que desea\n- Iniciar Juego (1)\n- Cambiar nombre (2)\n- Mostrar Registro de resultados (3)\n- Salir del juego (4)\n = ";
    cin >> option;
    interactivePanel(option);
    return option;
}

void start(){
    const int KEY_UP = 72;
    const int KEY_DOWN = 80;
    const int KEY_LEFT = 75;
    const int KEY_RIGHT = 77;

    string word = ""; 
    vector<pair<int, int>> letterPositions;
    bool firstWord = true;
    firstWord = true;

    dashboard();

    int coordX = 7;
    int coordY = 7;

    dashboardPrint(coordY, coordX);

    while(true){
        int key = _getch();

        if (key == 0 || key == 224) { // 0 y 224 inician teclas especiales
            int newY = coordY;
            int newX = coordX;

            switch (_getch()) { // Segunda verificación dentro de las teclas especiales
                case KEY_UP:
                    newY--;
                    break;
                case KEY_DOWN:
                    newY++;
                    break;
                case KEY_LEFT:
                    newX--;
                    break;
                case KEY_RIGHT:
                    newX++;
                    break;
            }
            if(newY >= 0 && newY < 15 && newX >= 0 && newX < 15){ 
                coordY = newY;
                coordX = newX;
            }
            cout << "\033[2J\033[1;1H"; // Hace un clear en la terminal
            dashboardPrint(coordY, coordX);
        }
        else if(key == 46){ // Punto (.)
            cout << "Programa detenido.";
            break;
        }else{
            if(key == 13){
                lastValidationMessage = "";
                for(char& c : word) {
                    if (c >= 'A' && c <= 'Z') {
                        c = static_cast<char>(c - 'A' + 'a');
                    }
                }
                bool wasFirstWord = firstWord;
                bool validPoint = wValidation(word, letterPositions, firstWord);

                if (validPoint && !wasFirstWord) {
                    if (!wordCrossesValidBoard(letterPositions)) {
                        lastValidationMessage = "La palabra debe empezar o cruzarse con una palabra valida.";
                        validPoint = false;
                    } else if (!existingLetterMatchesWord(word, letterPositions)) {
                        lastValidationMessage = "La letra nueva no coincide con una letra ya validada.";
                        validPoint = false;
                    }
                }

                if(validPoint){
                    if(turn == 1){
                        pts1++; // EL VALOR DEBE CAMBIAR ES SOLO UN EJEMPLO PARA TESTING... 
                        turn = 2; 
                    }else{
                        pts2++; 
                        turn = 1;
                    }
                    for (const auto& position : letterPositions) {
                        validTable[position.first][position.second] =
                            table[position.first][position.second];
                    }
                } else {
                    if(turn == 1){
                        turn = 2; 
                    }else{ 
                        turn = 1;
                    }
                    for (const auto& position : letterPositions) {
                        table[position.first][position.second] =
                            validTable[position.first][position.second] == 219
                                ? 219
                                : validTable[position.first][position.second];
                    }
                }
                word = "";
                letterPositions.clear();
                cout << "\033[2J\033[1;1H";
                dashboardPrint(coordY, coordX);
            }
            else{
                table[coordY][coordX] = static_cast<char>(key);
                word += static_cast<char>(key);
                letterPositions.push_back({coordY, coordX});
                cout << "\033[2J\033[1;1H";
                dashboardPrint(coordY, coordX);
            } 
        }
    }

}

string changeName(){
    string newNickname1, newNickname2;
    cout << "Ingresa tu nuevo nickname (User 1): ";
    cin >> newNickname1;
    cout << "Ingresa tu nuevo nickname (User 2): ";
    cin >> newNickname2;

    saveNickname(newNickname1, newNickname2);
    playerName1 = newNickname1;
    playerName2 = newNickname2;

    return newNickname1;
}

void log(){ // FALTA HACER ESTA VAINA !!!!!
    cout << "FUNCION EN DESARROLLO\n";
}

void exit(){
    cout << "SALIENDO DEL PROGRAMA...\n";
}

void interactivePanel(int x){
switch(x){
        case 1:
            start();
            break;
        case 2:
            changeName();
            menu();
            break;
        case 3:
            log();
            break;
        case 4:
            exit();
            break;
        default:
            cout << "ESA OPCIÓN ES INVÁLIDA" << "(" << x << ")\n";
    }
}

void dashboard(){
    for(int i = 0; i < 15; i++){
        for(int j = 0; j < 15; j++){
           table[i][j] = 219;
           validTable[i][j] = 219;
        }
    }
}

void dashboardPrint(int cursorY, int cursorX){
    cout << "Bienvenidos, " << playerName1 << " y " << playerName2 << "\n";
    cout << "Turno: J" << turn << " | Puntos J1: " << pts1 << " | Puntos J2: " << pts2 << "\n";
    if (!lastValidationMessage.empty()) {
        cout << lastValidationMessage << "\n";
    }
    cout << "\n";

    for(int i = 0; i < 15; i++){
        for(int j = 0; j < 15; j++){
           if(i == cursorY && j == cursorX){
               cout << " " << static_cast<char>(206) << " ";
           } else {
               cout << " " << table[i][j] << " ";
           }
        }
        cout << "\n\n";
    }
}

bool wordCrossesValidBoard(const vector<pair<int, int>>& letterPositions) {
    for (const auto& position : letterPositions) {
        int y = position.first;
        int x = position.second;

        if (validTable[y][x] != 219) {
            return true;
        }

        const int dy[] = {-1, 1, 0, 0};
        const int dx[] = {0, 0, -1, 1};
        for (int i = 0; i < 4; i++) {
            int adjacentY = y + dy[i];
            int adjacentX = x + dx[i];
            if (adjacentY >= 0 && adjacentY < 15 &&
                adjacentX >= 0 && adjacentX < 15 &&
                validTable[adjacentY][adjacentX] != 219) {
                return true;
            }
        }
    }
    return false;
}

bool existingLetterMatchesWord(const string& word, const vector<pair<int, int>>& letterPositions) {
    for (size_t i = 0; i < letterPositions.size(); i++) {
        int y = letterPositions[i].first;
        int x = letterPositions[i].second;
        if (validTable[y][x] != 219) {
            char candidate = static_cast<char>(tolower(static_cast<unsigned char>(table[y][x])));
            char validated = static_cast<char>(tolower(static_cast<unsigned char>(validTable[y][x])));
            char expected = static_cast<char>(tolower(static_cast<unsigned char>(word[i])));
            if (candidate != validated || validated != expected) {
                return false;
            }
        }
    }
    return true;
}

bool wValidation(string word, const vector<pair<int, int>>& letterPositions, bool& firstWord){
    string dictionary[520] = {
        // A (0-19)
        "avion", "avestruz", "animal", "agua", "abeja", "arbol", "anillo", "araña", "aguja", "ajo",
        "amarillo", "amigo", "arte", "azucar", "auto", "ala", "aire", "arena", "actor", "abuela",
        // B (20-39)
        "basura", "barco", "beso", "bota", "ballena", "bosque", "botella", "boton", "brazo", "bruja",
        "banco", "burro", "blanco", "bufanda", "balcon", "barril", "bebe", "bronce", "bicicleta", "bigote",
        // C (40-59)
        "casa", "cama", "carro", "cielo", "cinco", "color", "comida", "corazon", "caballo", "cabeza",
        "cebolla", "cerdo", "cereza", "circo", "clavo", "conejo", "cueva", "cuchillo", "copa", "cable",
        // D (60-79)
        "dedo", "dado", "diente", "dia", "dulce", "dinosaurio", "delfin", "disco", "diamante", "duende",
        "dragon", "ducha", "dinero", "diez", "dolor", "doctor", "dardo", "desierto", "dama", "domino",
        // E (80-99)
        "elefante", "estrella", "escoba", "espejo", "erizo", "enano", "escudo", "espada", "escuela", "escarabajo",
        "escaleras", "espina", "estatua", "estufa", "eclipse", "edificio", "examen", "equipo", "error", "espacio",
        // F (100-119)
        "fuego", "flor", "fantasma", "faro", "fiesta", "flecha", "foca", "foto", "fruta", "fuente",
        "fresa", "falda", "flauta", "foco", "familia", "fosforo", "frio", "frente", "fabrica", "farol",
        // G (120-139)
        "gato", "gallina", "globo", "gota", "guitarra", "gusano", "gorila", "galleta", "gigante", "girasol",
        "goma", "genio", "golpe", "grasa", "guante", "gaviota", "grifo", "ganso", "gorra", "gemelo",
        // H (140-159)
        "hielo", "hoja", "hormiga", "hueso", "huevo", "hada", "helado", "hilo", "humo", "hacha",
        "hamaca", "halcon", "harina", "hermano", "hospital", "hotel", "huella", "huerto", "hiena", "hipopotamo",
        // I (160-179)
        "isla", "iguana", "iman", "iglesia", "idea", "idioma", "insecto", "invierno", "imagen", "invento",
        "igual", "idolo", "indio", "icono", "incendio", "incienso", "ilusion", "imperio", "instinto", "iris",
        // J (180-199)
        "jabon", "jarra", "jefe", "jirafa", "joya", "juego", "jugo", "juguete", "jabali", "jardin",
        "jilguero", "jinete", "joven", "joyero", "judia", "jueves", "juez", "jungla", "junta", "justicia",
        // K (200-219)
        "koala", "kilo", "karate", "kayak", "kiwi", "karma", "kiosco", "kimono", "kermes", "ketchup",
        "kilt", "kurdo", "koine", "kendo", "kart", "kefir", "krill", "kung-fu", "kaiser", "karma",
        // L (220-239)
        "luna", "lobo", "leon", "lago", "lana", "lapiz", "leche", "letra", "libro", "limon",
        "llama", "llave", "lluvia", "loro", "luz", "labio", "lagarto", "lata", "lazo", "lente",
        // M (240-259)
        "mano", "mono", "mesa", "mapa", "manzana", "mariposa", "mosca", "musica", "madre", "mar",
        "miel", "miedo", "milagro", "minuto", "moneda", "montaña", "motor", "mundo", "muñeca", "muro",
        // N (260-279)
        "nube", "nariz", "naranja", "niño", "noche", "nieve", "nido", "nota", "nuez", "nave",
        "nombre", "norte", "nudo", "nueve", "numero", "nutria", "novia", "nuca", "noticia", "novela",
        // O (280-299)
        "ojo", "oreja", "oso", "oro", "ola", "oveja", "oceano", "ocho", "octubre", "oficina",
        "oido", "olla", "olor", "ombligo", "opera", "orden", "organo", "oruga", "ostra", "otoño",
        // P (300-319)
        "perro", "pato", "pez", "piedra", "puerta", "pan", "papel", "parque", "pelota", "pelo",
        "pera", "piano", "pie", "piel", "pintura", "piña", "planta", "plato", "playa", "puente",
        // Q (320-339)
        "queso", "quimica", "quince", "quinto", "queja", "quemadura", "querubin", "quijada", "quilla", "quimera",
        "quiosco", "quirurgico", "quiste", "quita", "quorum", "quesadilla", "quehacer", "querencia", "quirofano", "quiebre",
        // R (340-359)
        "raton", "rana", "rama", "rey", "rio", "rosa", "rueda", "reloj", "radio", "raiz",
        "racimo", "red", "regla", "reina", "risa", "roca", "ropa", "ruiseñor", "rubi", "ruido",
        // S (360-379)
        "sol", "sapo", "sal", "sangre", "santo", "silla", "sombrero", "sopa", "sueño", "suelo",
        "sabado", "sabana", "sabor", "saco", "salida", "salto", "saludo", "semilla", "serpiente", "silencio",
        // T (380-399)
        "tigre", "taza", "tambor", "tapa", "taxi", "teatro", "techo", "tela", "telefono", "television",
        "tenedor", "tiempo", "tienda", "tierra", "tijeras", "tiza", "toalla", "tomate", "torre", "tren",
        // U (400-419)
        "uva", "uña", "uno", "universo", "unicornio", "universidad", "urraca", "uso", "urna", "util",
        "utopia", "urgente", "urbano", "unguento", "union", "unidad", "uniforme", "ultrasonido", "ultimatum", "umbral",
        // V (420-439)
        "vaca", "vaso", "vela", "viento", "vino", "violin", "ventana", "verano", "verde", "vestido",
        "viaje", "vida", "vidrio", "viejo", "vagon", "volcan", "vuelo", "voz", "voto", "vuelta",
        // W (440-459)
        "waffle", "web", "wok", "wifi", "whisky", "walkman", "waterpolo", "wolframio", "wapiti", "webcam",
        "windsurf", "wagneriano", "western", "watt", "wombat", "water", "wasabi", "wushu", "weber", "wincha",
        // X (460-479)
        "xilofono", "xenofobia", "xerocopia", "xilografia", "xilofago", "xenon", "xilema", "xilorgano", "xifoides", "xilogeno",
        "xantoma", "xerofilo", "xilidina", "xeroftalmia", "xilol", "xilotila", "xantodermo", "xenobiotico", "xero", "xilofonista",
        // Y (480-499)
        "yate", "yegua", "yema", "yeso", "yunque", "yuca", "yogur", "yoga", "yodo", "yelmo",
        "yeti", "yarda", "yak", "yacimiento", "yerno", "yute", "yuyo", "yo-yo", "yin", "yanqui",
        // Z (500-519)
        "zapato", "zorro", "zanahoria", "zoologico", "zancudo", "zafiro", "zanja", "zarza", "zepelin", "zeta",
        "zigzag", "zinc", "zocalo", "zona", "zombi", "zumo", "zurdo", "zumbido", "zafarrancho", "zueco"
    };
    
    if (word.empty()) {
        lastValidationMessage = "La palabra no puede estar vacia.";
        return false;
    }

    if (word.find(' ') != string::npos || word.find('\t') != string::npos) {
        lastValidationMessage = "La palabra no puede tener espacios.";
        return false;
    }

    if (letterPositions.size() != word.length()) {
        lastValidationMessage = "La palabra no puede contener espacios ni posiciones vacias.";
        return false;
    }

    bool horizontal = true;
    bool vertical = true;

    for (int i = 1; i < static_cast<int>(letterPositions.size()); i++) {
        if (letterPositions[i].first != letterPositions[0].first) {
            horizontal = false;
        }
        if (letterPositions[i].second != letterPositions[0].second) {
            vertical = false;
        }
    }

    for (int i = 0; i < static_cast<int>(letterPositions.size()); i++) {
        for (int j = i + 1; j < static_cast<int>(letterPositions.size()); j++) {
            if (letterPositions[i] == letterPositions[j]) {
                lastValidationMessage = "La palabra no puede repetir una casilla.";
                return false;
            }
        }
    }

    if (!horizontal && !vertical) {
        lastValidationMessage = "La palabra no puede estar escrita en diagonal.";
        return false;
    }

    vector<int> coordinates;
    if (horizontal) {
        for (const auto& position : letterPositions) {
            coordinates.push_back(position.second);
        }
    } else {
        for (const auto& position : letterPositions) {
            coordinates.push_back(position.first);
        }
    }

    sort(coordinates.begin(), coordinates.end());
    for (int i = 1; i < static_cast<int>(coordinates.size()); i++) {
        if (coordinates[i] != coordinates[i - 1] + 1) {
            lastValidationMessage = "La palabra debe estar escrita en casillas consecutivas.";
            return false;
        }
    }

    if (firstWord) {
        bool coversCenter = false;
        for (const auto& position : letterPositions) {
            if (position.first == 7 && position.second == 7) {
                coversCenter = true;
                break;
            }
        }
        if (!coversCenter) {
            lastValidationMessage = "La primera palabra debe pasar por la casilla [7][7].";
            return false;
        }
    }
  

    bool encontrada = false;

    for (int i = 0; i < 520; i++) {
        if (word == dictionary[i]) {
            encontrada = true;
            break;
        }
    }

    if (encontrada) {
        lastValidationMessage = "PALABRA VALIDA, FELICIDADES";
        firstWord = false;
    } else {
        lastValidationMessage = "PALABRA INVALIDA";
    }

    return encontrada;
}