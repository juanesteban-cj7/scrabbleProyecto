#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void dataVerification(){
    fstream UserLog("userdata.txt"); 
    UserLog << "Escribiendo cosas para el txt, testing solamente jejejej";
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
    cout << "JUEGO INICIADO"; 
}
string changeName(){
    string newNickname; 
    cout << "Ingresa tu nuevo nickname: "; 
    cin >> newNickname;
    return newNickname;  
}
void log(){
    cout << "FUNCION EN DESARROLLO"; 
}
void exit(){
    cout << "SALIENDO DEL PROGRAMA..."; 
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
            cout << "ESA OPCIÓN ES INVÁLIDA" << "(" << x << ")"; 
    }
}

int main() {
    dataVerification();
	int option = menu();
	interactivePanel(option); 
	return 0;
}
