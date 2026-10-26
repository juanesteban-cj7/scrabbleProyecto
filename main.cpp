#include <iostream>
#include <fstream> // EDICION DE ARCHIVOS
#include <string>
#include <conio.h>  
using namespace std;

const int KEY_UP = 72;
const int KEY_DOWN = 80;
const int KEY_LEFT = 75;
const int KEY_RIGHT = 77;

char table[15][15];



//MENU FUNCTIONS
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
void dashboardPrint(); 

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

    userFile.close(); 
}

int menu() {
    int option;

    cout << "----------------------------" << endl;
    cout << "         ____                 " << endl;
    cout << R"(|\   /| |     |\   | |    | )" << endl;
    cout << R"(| \ / | |____ | \  | |    | )" << endl;
    cout << R"(|     | |     |  \ | |    | )" << endl;
    cout << R"(|     | |____ |   \| |____| )" << endl;
    cout << "                            " << endl;
    cout << "----------------------------" << endl;
    cout << "Seleccione la opcion que desea\n- Iniciar Juego (1)\n- Cambiar nombre (2)\n- Mostrar Registro de resultados (3)\n- Salir del juego (4)\n = ";
    cin >> option;
    interactivePanel(option);
    return option;
}

void start(){
    dashboard();
     
    int coordX = 7;  
    int coordY = 7;
    table[coordY][coordX] = 206;
    
    cout << "OPRIME CUALQUIER BOTON\n";

    while(true){
        int key = _getch();

        if (key == 0 || key == 224) { // 0 y 224 inician teclas especiales
            int newY = coordY; 
            int newX = coordX; 

            switch (_getch()) { // Segunda verificación dentro de las teclas especiales
                case 72:
                    newY--; 
                    break;
                case 80:
                    newY++; 
                    break;
                case 75:
                    newX--; 
                    break;
                case 77:
                    newX++; 
                    break;
            }
            if(newY >= 0 && newY < 15 && newX >= 0 && newX < 15){ // CASILLA ANTERIOR REGRESA A SER UN BLOQUE
                table[coordY][coordX] = 219; 
                coordY = newY; 
                coordX = newX; 
                table[coordY][coordX] = 206; 
            }
            dashboardPrint();
        }
        else if(key == 46){
            cout << "Programa detenido.";
            break; 
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

    return newNickname1;
}

void log(){
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
        }
    }
}

void dashboardPrint(){
    for(int i = 0; i < 15; i++){
        for(int j = 0; j < 15; j++){
           cout << table[i][j] << "  "; 
        }
        cout << "\n\n"; 
    }
}
