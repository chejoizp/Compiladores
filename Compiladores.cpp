#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <cctype>
#include <memory>
#include <string>
#include <unordered_set>
#include <algorithm>

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

// Clase para el analizador léxico
class AnalizadorLexico {
public:
    AnalizadorLexico(const string& input) : input(input), pos(0) {
        palabrasReservadas = { "if", "else", "while", "for", "return", "int", "float", "double", "char" };
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

        // Palabras reservadas e identificadores
        if (isalpha(caracter)) {
            string cadenas;
            while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) {
                cadenas += input[pos++];
            }

            // Convertir a minúsculas para comparación
            string lowerCadenas = cadenas;
            transform(lowerCadenas.begin(), lowerCadenas.end(), lowerCadenas.begin(), ::tolower);

            if (palabrasReservadas.find(lowerCadenas) != palabrasReservadas.end()) {
                return { TipoTokens::PalabraReservada, cadenas };  // Mantiene la versión original
            }
            else {
                return { TipoTokens::Identificador, cadenas };
            }
        }

        // Operadores
        if (caracter == '+' || caracter == '-' || caracter == '*' || caracter == '/') {
            pos++;
            return { TipoTokens::Operador, string(1, caracter) };
        }

        // Delimitadores
        if (caracter == '(' || caracter == ')' || caracter == ',' || caracter == ';' || caracter == '=') {
            pos++;
            return { TipoTokens::Delimitador, string(1, caracter) };
        }

        cerr << "Error: Caracter no válido encontrado: '" << caracter << "'\n";
        exit(1);
    }

private:
    string input;
    size_t pos;
    unordered_set<string> palabrasReservadas;
};

// Vector global de tokens
vector<Token> tokens;

// Función para tokenizar una operación matemática
vector<Token> tokenize(const string& operacion) {
    vector<Token> result;
    AnalizadorLexico analizador(operacion);
    Token token;
    while ((token = analizador.getNextToken()).type != TipoTokens::FinExpresion) {
        result.push_back(token);
    }
    return result;
}

// Estructura para la tabla de símbolos
struct Simbolo {
    string nombre;
    TipoTokens token;
    string tipo;
};

vector<Simbolo> tablaSimbolos;  // Vector que almacena los identificadores

// Función para actualizar la tabla de símbolos
void ActualizarTablaSimbolos() {
    tablaSimbolos.clear();
    string tipoActual = "";

    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TipoTokens::PalabraReservada && tokens[i].value == "int") {
            tipoActual = "int";  // Si hay una declaración de int, guardar el tipo
        }
        else if (tokens[i].type == TipoTokens::Identificador) {
            bool existe = false;
            for (const auto& simbolo : tablaSimbolos) {
                if (simbolo.nombre == tokens[i].value) {
                    existe = true;
                    break;
                }
            }
            if (!existe) {
                tablaSimbolos.push_back({ tokens[i].value, TipoTokens::Identificador, tipoActual.empty() ? "Desconocido" : tipoActual });
            }
            tipoActual = "";  // Reiniciar tipo después de asignarlo
        }
    }
}

// Función para mostrar la tabla de símbolos
void MostrarTablaSimbolos() {
    cout << "\nTabla de Simbolos:\n";
    cout << "===============================\n";
    cout << "Identificador\tToken\t\tTipo\n";
    cout << "-------------------------------\n";
    for (const auto& simbolo : tablaSimbolos) {
        cout << simbolo.nombre << "\t\tIdentificador\t" << simbolo.tipo << "\n";
    }
}

// Función para mostrar tokens agrupados
void MostrarTokens() {
    cout << "Tokens por categoria:\n";
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

// Función principal
int main() {
    int opcion;
    do {
        cout << "\nBienvenido, estas son las opciones del programa" << endl;
        cout << "1. Ingresar expresion" << endl;
        cout << "2. Analisis Lexico" << endl;
        cout << "3. Analisis Sintactico" << endl;
        cout << "4. Tabla de Simbolos" << endl;
        cout << "5. Salir" << endl;
        cout << "\nEscriba el numero de la opcion a ejecutar: ";
        cin >> opcion;
        cin.ignore();

        if (opcion == 1) {
            string operacion;
            char continuar;
            do {
                cout << "Ingresa una expresion: ";
                getline(cin, operacion);
                vector<Token> newTokens = tokenize(operacion);

                // Evitar duplicados en los tokens
                for (const auto& newToken : newTokens) {
                    bool existe = false;
                    for (const auto& token : tokens) {
                        if (newToken.value == token.value) {
                            existe = true;
                            break;
                        }
                    }
                    if (!existe) {
                        tokens.push_back(newToken);
                    }
                }

                cout << "¿Deseas ingresar otra expresion? (s/n): ";
                cin >> continuar;
                cin.ignore();  // Ignorar el salto de línea después de la entrada
            } while (continuar == 's' || continuar == 'S');

        }
        else if (opcion == 2) {
            MostrarTokens();
        }
        else if (opcion == 3 || opcion == 4) {
            ActualizarTablaSimbolos();
            MostrarTablaSimbolos();
        }
    } while (opcion != 5);

    return 0;
}

