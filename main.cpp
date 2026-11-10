#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <cctype>
#include <vector>
#include <windows.h>

using namespace std;

char table[15][15];
unsigned char validTable[15][15];

int turn = 1;
int pts1 = 0;
int pts2 = 0; // Puntaje local
int wins1 = 0;
int wins2 = 0;

string playerName1 = "Jugador 1";
string playerName2 = "Jugador 2";
string lastValidationMessage = "";

string words[] = {"A", "E", "I", "L", "N", "O", "R", "S", "T", "U", "D", "G", "B", "C", "M", "P", "F", "H", "V", "Y", "CH", "Q", "J", "LL", "Ñ", "RR", "X", "Z"};
const int WORD_COUNT = sizeof(words) / sizeof(words[0]);
int player1Cantidad[] = {12, 12, 6, 4, 5, 9, 5, 6, 4, 5, 5, 2, 2, 4, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int player2Cantidad[] = {12, 12, 6, 4, 5, 9, 5, 6, 4, 5, 5, 2, 2, 4, 2, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

//MENU FUNCTIONS
int terminalWidth() {
    // la API de Windows exige pasar la direccion de la estructura.
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return 80;
}
void saveNickname(string nickname1, string nickname2);
void dataVerification();
int menu();
void start();
string changeName();
void log();
void exit();
void interactivePanel(int x);

// IN-GAME FUNCTIONS
void dashboard();
void dashboardPrint(int cursorY, int cursorX);
bool wValidation(string word, vector<pair<int, int>> letterPositions, bool firstWord);
bool wordCrossesValidBoard(vector<pair<int, int>> letterPositions);
bool existingLetterMatchesWord(string word, vector<pair<int, int>> letterPositions);
int singleLetterPoints(char letter);
size_t bytesEnye(string word, size_t i);
void consumeTile(int player, int letterIndex);
vector<int> sortedCoordinates(vector<int> values);
int wordScore(string word, vector<pair<int, int>> letterPositions);
bool hasNewLetter(vector<pair<int, int>> letterPositions);
void subtractingLetters(string word, vector<pair<int, int>> letterPositions, int player);
int findLetterIndex(string letter);

// FINAL FUNCTIONS
void finalCout();

int main() {
    dataVerification();
	menu();
	return 0;
}

void saveNickname(string nickname1, string nickname2){
    ofstream userFile("userdata.txt");

    if(userFile.is_open()){
        userFile << nickname1 << '\n' << nickname2 << '\n';
        userFile << wins1 << '\n' << wins2 << '\n';
        userFile.close();
    } else{
        cout << "No se pudo guardar el nickname \n";
    }
}

void dataVerification(){
    string nickname1, nickname2;
    ifstream userFile("userdata.txt");

    if(userFile >> nickname1 && userFile >> nickname2) {
        if(!(userFile >> wins1 >> wins2)){
            wins1 = 0;
            wins2 = 0;
        }
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
            finalCout();
            break;
        }
        else{
            if(key == 13){
                lastValidationMessage = "";
                for(size_t i = 0; i < word.size(); i++) {
                    if (word[i] >= 'A' && word[i] <= 'Z') {
                        word[i] = static_cast<char>(word[i] - 'A' + 'a');
                    }
                }
                bool wasFirstWord = firstWord;
                bool validPoint = wValidation(word, letterPositions, firstWord);
                if (validPoint) {
                    firstWord = false; 
                }

                if (validPoint && !wasFirstWord) {
                    if (!wordCrossesValidBoard(letterPositions)) {
                        lastValidationMessage = "La palabra debe empezar o cruzarse con una palabra valida.";
                        validPoint = false;
                    } else if (!existingLetterMatchesWord(word, letterPositions)) {
                        lastValidationMessage = "La letra nueva no coincide con una letra ya validada.";
                        validPoint = false;
                    } else if (!hasNewLetter(letterPositions)) {
                        lastValidationMessage = "La palabra debe incluir al menos una letra nueva.";
                        validPoint = false;
                    }
                }

                if(validPoint){
                    int earned = wordScore(word, letterPositions);
                    subtractingLetters(word, letterPositions, turn);

                    if(turn == 1){
                        pts1 += earned;
                    }else{
                        pts2 += earned;
                    }

                    int totalCardsUser1 = 0;
                    int totalCardsUser2 = 0;
                    for (int i = 0; i < WORD_COUNT; i++) {
                        totalCardsUser1 += player1Cantidad[i];
                        totalCardsUser2 += player2Cantidad[i];
                    }

                    if (totalCardsUser1 < 10 || totalCardsUser2 < 10) {
                        finalCout();
                        break;
                    }

                    if(turn == 1){
                        turn = 2;
                    }else{
                        turn = 1;
                    }
                    lastValidationMessage += " (+" + to_string(earned) + " puntos)";
                    for (size_t i = 0; i < letterPositions.size(); i++) {
                        validTable[letterPositions[i].first][letterPositions[i].second] =
                            table[letterPositions[i].first][letterPositions[i].second];
                    }
                } else {
                    if(turn == 1){
                        turn = 2;
                    }else{
                        turn = 1;
                    }
                    for (size_t i = 0; i < letterPositions.size(); i++) {
                        table[letterPositions[i].first][letterPositions[i].second] =
                            validTable[letterPositions[i].first][letterPositions[i].second] == 219
                                ? 219
                                : validTable[letterPositions[i].first][letterPositions[i].second];
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

void log(){
    ifstream userFile("userdata.txt");

    if(!userFile.is_open()){
        cout << "No se pudo cargar los datos.\n";
        getch();
        menu();
        return;
    }

    string name1, name2;
    int victories1 = 0;
    int victories2 = 0;

    if(userFile >> name1 >> name2 >> victories1 >> victories2){
        cout << "\033[2J\033[1;1H";
        cout << endl;
        cout << "VICTORIAS DE LOS JUGADORES\n";
        cout << name1 << ": " << victories1 << " victorias\n";
        cout << name2 << ": " << victories2 << " victorias\n";
        cout << endl;
        cout << "(Presiona cualquier tecla para volver al menu)\n";
    }else{
        cout << "No se pudieron leer las victorias.\n";
    }

    userFile.close();
    getch();
    menu();
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
            cout << "ESA OPCION ES INVALIDA" << "(" << x << ")\n";
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
    cout << "Turno: J" << turn << " | Puntos J1: " << pts1 << " | Puntos J2: " << pts2 << " ";
    if (!lastValidationMessage.empty()) {
        cout << lastValidationMessage << "\n";
    }
    cout << "\n";

    vector<string> panelLines;
    panelLines.push_back("FICHAS QUE QUEDAN");
    panelLines.push_back(turn == 1 ? "Jugador 1: " + playerName1 : "Jugador 2: " + playerName2);
    panelLines.push_back("--------------------------------");

    const int groupStarts[] = {0, 10, 12, 16, 20, 22, 27};
    const int groupEnds[] = {10, 12, 16, 20, 22, 27, 28};
    const string groupPoints[] = {"1 punto", "2 puntos", "3 puntos",
                                  "4 puntos", "5 puntos", "8 puntos", "10 puntos"};

    for (int group = 0; group < 7; group++) {
        string line = groupPoints[group] + ": ";
        for (int i = groupStarts[group]; i < groupEnds[group]; i++) {
            int remaining = turn == 1 ? player1Cantidad[i] : player2Cantidad[i];
            line += words[i] + " x" + to_string(remaining);
            if (i + 1 < groupEnds[group]) {
                line += ", ";
            }
        }
        panelLines.push_back(line);
    }

    cout << "   ";
    for (int j = 0; j < 15; j++) {
        int columnNumber = j;
        if (columnNumber < 10) {
            cout << " ";
        }
        cout << columnNumber << " ";
    }
    cout << "\n";

    for(int i = 0; i < 15; i++){
        int rowNumber = i;
        if (rowNumber < 10) {
            cout << " ";
        }
        cout << rowNumber << " ";
        for(int j = 0; j < 15; j++){
           if(i == cursorY && j == cursorX){
               cout << " " << static_cast<char>(206) << " ";
           } else {
               cout << " " << table[i][j] << " ";
           }
        }
        if (i < static_cast<int>(panelLines.size())) {
            cout << "   |   " << panelLines[i];
        }
        cout << "\n\n";
    }
}

int findLetterIndex(string letter) {
    string upperLetter = letter;
    for (size_t i = 0; i < upperLetter.size(); i++) {
        upperLetter[i] = static_cast<char>(toupper(static_cast<unsigned char>(upperLetter[i])));
    }

    for (int i = 0; i < WORD_COUNT; i++) {
        if (words[i] == upperLetter) {
            return i;
        }
    }
    return -1;
}

// Descuenta una ficha del jugador indicado (antes se hacia con una referencia "int& remaining").
void consumeTile(int player, int letterIndex) {
    if (letterIndex >= 0) {
        if (player == 1) {
            if (player1Cantidad[letterIndex] > 0) {
                player1Cantidad[letterIndex]--;
            }
        } else {
            if (player2Cantidad[letterIndex] > 0) {
                player2Cantidad[letterIndex]--;
            }
        }
    }
}

void subtractingLetters(string word, vector<pair<int, int>> letterPositions, int player) {
    size_t wordIndex = 0;

    while (wordIndex < word.size()) {
        size_t bytes = bytesEnye(word, wordIndex);
        string token;
        int positionIndex = static_cast<int>(wordIndex);

        if (bytes > 0) {
            token = "Ñ";
            if (positionIndex < static_cast<int>(letterPositions.size())) {
                pair<int, int> position = letterPositions[positionIndex];
                if (validTable[position.first][position.second] == 219) {
                    int letterIndex = findLetterIndex(token);
                    consumeTile(player, letterIndex);
                }
            }
            wordIndex += bytes;
            continue;
        }

        if (wordIndex + 1 < word.size() &&
            positionIndex + 1 < static_cast<int>(letterPositions.size())) {
            char first = static_cast<char>(toupper(static_cast<unsigned char>(word[wordIndex])));
            char second = static_cast<char>(toupper(static_cast<unsigned char>(word[wordIndex + 1])));
            if ((first == 'C' && second == 'H') ||
                (first == 'L' && second == 'L') ||
                (first == 'R' && second == 'R')) {
                token = string(1, first) + string(1, second);

                pair<int, int> position = letterPositions[positionIndex];
                if (validTable[position.first][position.second] == 219) {
                    int letterIndex = findLetterIndex(token);
                    consumeTile(player, letterIndex);
                }
                wordIndex += 2;
                continue;
            }
        }

        char current = static_cast<char>(toupper(static_cast<unsigned char>(word[wordIndex])));
        if (positionIndex < static_cast<int>(letterPositions.size())) {
            pair<int, int> position = letterPositions[positionIndex];
            if (validTable[position.first][position.second] == 219) {
                int letterIndex = findLetterIndex(string(1, current));
                consumeTile(player, letterIndex);
            }
        }
        wordIndex++;
    }
}

int singleLetterPoints(char letter) {
    switch (toupper(static_cast<unsigned char>(letter))) {
        case 'A': case 'E': case 'I': case 'L': case 'N':
        case 'O': case 'R': case 'S': case 'T': case 'U':
            return 1;
        case 'D': case 'G':
            return 2;
        case 'B': case 'C': case 'M': case 'P':
            return 3;
        case 'F': case 'H': case 'V': case 'Y':
            return 4;
        case 'Q':
            return 5;
        case 'J': case 'X':
            return 8;
        case 'Z':
            return 10;
        default:
            return 0;
    }
}

// Devuelve cuantos bytes ocupa la Ñ en la posicion i (2 en UTF-8, 1 en una sola codificacion)
// o 0 si en esa posicion no hay una Ñ. (Antes: bool isEnye(..., size_t& bytes))
size_t bytesEnye(string word, size_t i) {
    unsigned char current = static_cast<unsigned char>(word[i]);

    // UTF-8: Ñ = C3 91, ñ = C3 B1
    if (current == 0xC3 && i + 1 < word.size()) {
        unsigned char next = static_cast<unsigned char>(word[i + 1]);
        if (next == 0x91 || next == 0xB1) {
            return 2;
        }
    }

    // Codificaciones de un solo byte usadas por la consola (CP437/CP850/CP1252)
    if (current == 0xA5 || current == 0xA4 || current == 0xD1 || current == 0xF1) {
        return 1;
    }

    return 0;
}

// Ordena los valores de menor a mayor).
vector<int> sortedCoordinates(vector<int> values) {
    for (size_t i = 1; i < values.size(); i++) {
        int current = values[i];
        int j = static_cast<int>(i) - 1;
        while (j >= 0 && values[j] > current) {
            values[j + 1] = values[j];
            j--;
        }
        values[j + 1] = current;
    }
    return values;
}

bool hasNewLetter(vector<pair<int, int>> letterPositions) {
    for (size_t i = 0; i < letterPositions.size(); i++) {
        pair<int, int> position = letterPositions[i];
        if (validTable[position.first][position.second] == 219) {
            return true;
        }
    }
    return false;
}

int wordScore(string word, vector<pair<int, int>> letterPositions) {
    int total = 0;
    size_t i = 0;

    while (i < word.size()) {
        size_t bytes = bytesEnye(word, i);

        if (bytes > 0) {
            bool anyNew = false;
            for (size_t k = 0; k < bytes && i + k < letterPositions.size(); k++) {
                if (validTable[letterPositions[i + k].first][letterPositions[i + k].second] == 219) {
                    anyNew = true;
                    break;
                }
            }
            if (anyNew) {
                total += 8;
            }
            i += bytes;
            continue;
        }

        bool firstIsNew = i < letterPositions.size() &&
            validTable[letterPositions[i].first][letterPositions[i].second] == 219;

        char current = static_cast<char>(toupper(static_cast<unsigned char>(word[i])));

        // Las letras dobles se validan en su primera letra.
        if (firstIsNew && i + 1 < word.size()) {
            char next = static_cast<char>(toupper(static_cast<unsigned char>(word[i + 1])));

            if (current == 'C' && next == 'H') {
                total += 5;
                i += 2;
                continue;
            }
            if (current == 'L' && next == 'L') {
                total += 8;
                i += 2;
                continue;
            }
            if (current == 'R' && next == 'R') {
                total += 8;
                i += 2;
                continue;
            }
        }

        if (firstIsNew) {
            total += singleLetterPoints(current);
        }

        i += 1;
    }

    return total;
}

bool wordCrossesValidBoard(vector<pair<int, int>> letterPositions) {
    for (size_t p = 0; p < letterPositions.size(); p++) {
        int y = letterPositions[p].first;
        int x = letterPositions[p].second;

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

bool existingLetterMatchesWord(string word, vector<pair<int, int>> letterPositions) {
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

bool wValidation(string word, vector<pair<int, int>> letterPositions, bool firstWord){
    if (word.empty()) {
        lastValidationMessage = "La palabra no puede estar vacia.";
        return false;
    }

    if (word.size() == 1 || bytesEnye(word, 0) == word.size()) {
        lastValidationMessage = "Las palabras de una sola letra no son validas.";
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
        for (size_t i = 0; i < letterPositions.size(); i++) {
            coordinates.push_back(letterPositions[i].second);
        }
    } else {
        for (size_t i = 0; i < letterPositions.size(); i++) {
            coordinates.push_back(letterPositions[i].first);
        }
    }

    coordinates = sortedCoordinates(coordinates);
    for (int i = 1; i < static_cast<int>(coordinates.size()); i++) {
        if (coordinates[i] != coordinates[i - 1] + 1) {
            lastValidationMessage = "La palabra debe estar escrita en casillas consecutivas.";
            return false;
        }
    }

    if (firstWord) {
        bool coversCenter = false;
        for (size_t i = 0; i < letterPositions.size(); i++) {
            if (letterPositions[i].first == 7 && letterPositions[i].second == 7) {
                coversCenter = true;
                break;
            }
        }
        if (!coversCenter) {
            lastValidationMessage = "La primera palabra debe pasar por la casilla [7][7].";
            return false;
        }
    }

    int availableLetterCantidad[WORD_COUNT];
    for (int i = 0; i < WORD_COUNT; i++) {
        availableLetterCantidad[i] = turn == 1 ? player1Cantidad[i] : player2Cantidad[i];
    }

    size_t wordIndex = 0;
    size_t positionIndex = 0;
    while (wordIndex < word.size()) {
        size_t bytes = bytesEnye(word, wordIndex);
        string letter;

        if (bytes > 0) {
            letter = "Ñ";
            if (positionIndex < letterPositions.size() &&
                validTable[letterPositions[positionIndex].first][letterPositions[positionIndex].second] == 219) {
                int letterIndex = findLetterIndex(letter);
                if (letterIndex >= 0) {
                    if (availableLetterCantidad[letterIndex] <= 0) {
                        lastValidationMessage = "Ya no quedan fichas de la letra " + letter +
                            (turn == 1 ? " para el Jugador 1." : " para el Jugador 2.");
                        return false;
                    }
                    availableLetterCantidad[letterIndex]--;
                }
            }
            wordIndex += bytes;
            positionIndex++;
            continue;
        }

        if (wordIndex + 1 < word.size() && positionIndex + 1 < letterPositions.size()) {
            char first = static_cast<char>(toupper(static_cast<unsigned char>(word[wordIndex])));
            char second = static_cast<char>(toupper(static_cast<unsigned char>(word[wordIndex + 1])));

            if ((first == 'C' && second == 'H') ||
                (first == 'L' && second == 'L') ||
                (first == 'R' && second == 'R')) {
                letter = string(1, first) + string(1, second);

                if (validTable[letterPositions[positionIndex].first][letterPositions[positionIndex].second] == 219) {
                    int letterIndex = findLetterIndex(letter);
                    if (letterIndex >= 0) {
                        if (availableLetterCantidad[letterIndex] <= 0) {
                            lastValidationMessage = "Ya no quedan fichas de la letra " + letter +
                                (turn == 1 ? " para el Jugador 1." : " para el Jugador 2.");
                            return false;
                        }
                        availableLetterCantidad[letterIndex]--;
                    }
                }

                wordIndex += 2;
                positionIndex += 2;
                continue;
            }
        }

        if (positionIndex < letterPositions.size() &&
            validTable[letterPositions[positionIndex].first][letterPositions[positionIndex].second] == 219) {
            char current = static_cast<char>(toupper(static_cast<unsigned char>(word[wordIndex])));
            int letterIndex = findLetterIndex(string(1, current));
            if (letterIndex >= 0) {
                if (availableLetterCantidad[letterIndex] <= 0) {
                    string messageLetter(1, static_cast<char>(toupper(static_cast<unsigned char>(word[wordIndex]))));
                    lastValidationMessage = "Ya no quedan fichas de la letra " + messageLetter +
                        (turn == 1 ? " para el Jugador 1." : " para el Jugador 2.");
                    return false;
                }
                availableLetterCantidad[letterIndex]--;
            }
        }

        wordIndex++;
        positionIndex++;
    }


    char confirmation = ' ';
    cout << "Palabra: " << word << " ¿Desea aceptar esta palabra? y/n: ";
    cin >> confirmation;

    confirmation = static_cast<char>(tolower(static_cast<unsigned char>(confirmation)));

    while (confirmation != 'y' && confirmation != 'n') {
        cout << "Respuesta invalida. Palabra: " << word
             << " ¿Desea aceptar esta palabra? y/n: ";
        cin >> confirmation;
        confirmation = static_cast<char>(tolower(static_cast<unsigned char>(confirmation)));
    }

    if (confirmation == 'y') {
        lastValidationMessage = "PALABRA VALIDA, FELICIDADES";
        return true;
    } else {
        lastValidationMessage = "PALABRA INVALIDA";
        return false;
    }
}

void finalCout(){
    cout << "JUEGO FINALIZADO. RESUMEN DE PUNTOS: \n";
    cout << playerName1 << ": " << pts1 << " puntos\n";
    cout << playerName2 << ": " << pts2 << " puntos\n";

    string storedName1, storedName2;
    int storedWins1 = 0;
    int storedWins2 = 0;

    ifstream readFile("userdata.txt");
    if(readFile >> storedName1 >> storedName2){
        if(!(readFile >> storedWins1 >> storedWins2)){
            storedWins1 = 0;
            storedWins2 = 0;
        }
    }
    readFile.close();

    wins1 = storedWins1;
    wins2 = storedWins2;

    if(pts1 > pts2){
        cout << "Felicidades " << playerName1 << " has ganado!!\n";
        wins1++;
    }else if(pts1 < pts2){
        cout << "Felicidades " << playerName2 << " has ganado!!\n";
        wins2++;
    }else{
        cout << "El juego termino en empate.\n";
    }

    ofstream userFile("userdata.txt");
    if(userFile.is_open()){
        userFile << playerName1 << '\n' << playerName2 << '\n';
        userFile << wins1 << '\n' << wins2 << '\n';
        userFile.close();
    }else{
        cout << "No se pudo guardar el resultado.\n";
    }
}