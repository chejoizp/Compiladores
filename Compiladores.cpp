#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <cctype>
#include <memory>
#include <string>
#include <unordered_set>

using namespace std;

// Definición de tipos de tokens
enum class TipoTokens {
    PalabraReservada, 
    Identificador,      
    Literal,          
    Operador,         
    Delimitador,        
    FinExpresion,            
    Invalido           
};

// Estructura del token
struct Token {
    TipoTokens type;
    string value;
};


Token token;
vector<Token> tokens;


// Clase para el analizador léxico
class AnalizadorLexico {
public:
    AnalizadorLexico(const string& input) : input(input), pos(0) {
        palabrasReservadas = { "if", "else", "while", "for", "return", "int" };
    }

    Token getNextToken() {
        while (pos < input.size() && isspace(input[pos])) {
            pos++;  // Saltar espacios en blanco
        }

        if (pos >= input.size()) return { TipoTokens::FinExpresion, "" };

        char caracter = input[pos];

        // Números y literales
        if (isdigit(caracter)) {
            string digito;
            while (pos < input.size() && (isdigit(input[pos]) || input[pos] == '.')) {
                digito += input[pos++];
            }
            return { TipoTokens::Literal, digito };
        }

        // Palabras reservadas
        if (isalpha(caracter)) {
            string cadenas;
            while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) {
                cadenas += input[pos++];
            }
            if (palabrasReservadas.find(cadenas) != palabrasReservadas.end()) {
                return { TipoTokens::PalabraReservada, cadenas };
            }
            else {
                return { TipoTokens::Identificador, cadenas };
            }
        }

        // Operadores
        if (caracter == '+') { pos++; return { TipoTokens::Operador, "+" }; }
        if (caracter== '-') { pos++; return { TipoTokens::Operador, "-" }; }
        if (caracter == '*') { pos++; return { TipoTokens::Operador, "*" }; }
        if (caracter == '/') { pos++; return { TipoTokens::Operador, "/" }; }

        // Delimitadores
        if (caracter == '(') { pos++; return { TipoTokens ::Delimitador, "(" }; }
        if (caracter == ')') { pos++; return { TipoTokens::Delimitador, ")" }; }
        if (caracter == ',') { pos++; return { TipoTokens::Delimitador, "," }; }
        if (caracter == ';') { pos++; return { TipoTokens::Delimitador, ";" }; }
        if (caracter == '=') { pos++; return { TipoTokens::Delimitador, "=" }; }

        // Si encontramos un carácter no válido, lo marcamos y detenemos el análisis
        cerr << "Error: Caracter no válido encontrado: '" << caracter << "'\n";
        exit(1);  // Termina el programa si se encuentra un carácter no válido
    }

private:
    string input;
    size_t pos;
    unordered_set<string> palabrasReservadas; 
};


// Función para imprimir los tokens agrupados
void MostrarTokens() {
    cout << "Tokens por categoría:\n";
    cout << "\nPalabras Reservadas:\n";
    for (const auto& token : tokens) {
        if (token.type == TipoTokens::PalabraReservada) {
            cout << token.value << " ";
        }
    }
    cout << "\n\nIdentificadores:\n";
    for (const auto& token : tokens) {
        if (token.type == TipoTokens::Identificador) {
            cout << token.value << " ";
        }
    }
    cout << "\n\nLiterales:\n";
    for (const auto& token : tokens) {
        if (token.type == TipoTokens::Literal) {
            cout << token.value << " ";
        }
    }
    cout << "\n\nOperadores:\n";
    for (const auto& token : tokens) {
        if (token.type == TipoTokens::Operador) {
            cout << token.value << " ";
        }
    }
    cout << "\n\nDelimitadores:\n";
    for (const auto& token : tokens) {
        if (token.type == TipoTokens::Delimitador) {
            cout << token.value << " ";
        }
    }
    cout << endl;
}


void GuardarEnArchivo() {
    ofstream outFile("tokens.txt");
    for (const auto& token : tokens) {
        outFile << "Token: " << token.value << " Tipo: ";
        switch (token.type) {
        case TipoTokens::PalabraReservada: outFile << "Palabra Reservada"; break;
        case TipoTokens::Identificador: outFile << "Identificador"; break;
        case TipoTokens::Literal: outFile << "Literal"; break;
        case TipoTokens::Operador: outFile << "Operador"; break;
        case TipoTokens::Delimitador: outFile << "Delimitador"; break;
        default: outFile << "Inválido"; break;
        }
        outFile << endl;
    }
    outFile.close();
    cout << "Tokens guardados en 'tokens.txt'." << endl;
}


// Función principal
int main() {
    int opcion;
    string expresion;
    AnalizadorLexico* Analisis = nullptr;

    cout << "Proyecto Analizador Lexico y Sintactico" << endl;
    do {
        cout << "\nBienvenido, estas son las opciones del programa" << endl;
        cout << "1. Ingresar expresion" << endl;
        cout << "2. Analisis Lexico" << endl;
        cout << "3. Analisis Sintactico" << endl;
        cout << "4. Tabla de Simbolos" << endl;
        cout << "5. Salir" << endl;
        cout << "\nEscriba el numero de la opcion a ejecutar: ";
        cin >> opcion;
        switch (opcion) {
        case 1:
            cout << "Introduce una expresion: ";
            cin.ignore();
            getline(cin, expresion);
            Analisis = new AnalizadorLexico(expresion);  // Aseguramos que Analisis reciba la expresión actualizada
            tokens.clear();  // Limpiamos los tokens previos antes de procesar la nueva expresión
            while (true) {
                Token token = Analisis->getNextToken();
                if (token.type == TipoTokens::FinExpresion) break;
                tokens.push_back(token);
            }
            GuardarEnArchivo();
            break;
        case 2:
            if (tokens.empty()) {
                cout << "No se han analizado tokens. Ingresa una expresión primero.\n";
            }
            else {
                MostrarTokens();
            }
            break;
        case 3:
            break;
        case 4:
            break;
        default:
            break;
        }

    } while (opcion != 5);

    // Liberamos la memoria de Analisis al final
    delete Analisis;

    return 0;
}

