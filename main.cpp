#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char table[15][15];

//MENU FUNCTIONS
void saveNickname(const string& nickname); 
void dataVerification(string& nickname); 
int menu(); 
void start(); 
string changeName(); 
void log(); 
void exit(); 
void interactivePanel(int x); 

// IN-GAME FUNCTIONS
void dashboard(); 

int main() {
    string nickname;
    dataVerification(nickname);
	int option = menu();
	interactivePanel(option); 
	return 0;
}

void saveNickname(const string& nickname){
    ofstream userFile("userdata.txt"); 

    if(userFile.is_open()){
        userFile << nickname; 
        userFile.close(); 
    } else{
        cout << "No se pudo guardar el nickname \n";
    }
}

void dataVerification(string& nickname){
    ifstream userFile("userdata.txt"); 

    if(userFile >> nickname) {
        cout << "Bienvenido nuevamente, " << nickname << "\n"; 
    } else{
        cout << "Primera vez que ejecutas el programa. \n"; 
        cout << "Ingresa tu nickname: "; 
        cin >> nickname; 

        saveNickname(nickname); 

        cout << "Bienvenido, " << nickname << "\n"; 
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
    return option;
}

void start(){
    dashboard(); 
    for(int i = 0; i < 15; i++){
        for(int j = 0; j < 15; j++){
           cout << table[i][j] << " "; 
        }
        cout << "\n\n"; 
    }
}

string changeName(){
    string newNickname; 
    cout << "Ingresa tu nuevo nickname: "; 
    cin >> newNickname;
    saveNickname(newNickname);
    return newNickname;  
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
            cout << changeName();  
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
