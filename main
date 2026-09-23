#include <iostream>
#include <string>
using namespace std;

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

void interactivePanel(int x){
    switch(x){
        case 1: 
            cout << "INICIANDO JUEGO..."; 
            break; 
        case 2: 
            cout << "TRANSLADANDO AL PANEL DE CAMBIAR NOMBRE..."; 
            break; 
        case 3: 
            cout << "MOSTRANDO REGISTRO..."; 
            break; 
        case 4: 
            cout << "SALIENDO DEL JUEGO..."; 
            break; 
        default: 
            cout << "ESA OPCIÓN ES INVÁLIDA" << "(" << x << ")"; 
    }
}

int main() {
	int option = menu();
	interactivePanel(option); 
	return 0;
}
