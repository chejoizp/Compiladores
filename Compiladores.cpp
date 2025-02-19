#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <cctype>
#include <memory>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <iomanip>  // Para usar setw()

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
                return { TipoTokens::PalabraReservada, lowerCadenas };  // Mantiene la versión original
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
    int id; // Identificador único para cada variable
};

vector<Simbolo> tablaSimbolos;  // Vector que almacena los identificadores

// Función para actualizar la tabla de símbolos
void ActualizarTablaSimbolos() {
    string tipoActual = "";  // Esta variable se restablece en cada llamada, para actualizar el tipo en cada nueva expresión.
    int contadorId = 1;  // Contador para asignar el número a los identificadores

    // Recorrer todos los tokens y asignarles el tipo adecuado
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TipoTokens::PalabraReservada) {
            // Actualizar el tipo si es una palabra reservada de tipo (int, float, etc.)
            if (tokens[i].value == "int" || tokens[i].value == "float" || tokens[i].value == "double" || tokens[i].value == "char") {
                tipoActual = tokens[i].value;  // Actualizar tipoActual al tipo encontrado
            }
        }
        else if (tokens[i].type == TipoTokens::Identificador) {
            bool existe = false;

            // Verificar si el identificador ya está en la tabla de símbolos
            for (const auto& simbolo : tablaSimbolos) {
                if (simbolo.nombre == tokens[i].value) {
                    existe = true;
                    break;
                }
            }

            // Si no existe, agregarlo con el tipo correcto
            if (!existe) {
                tablaSimbolos.push_back({ tokens[i].value, TipoTokens::Identificador, tipoActual.empty() ? "Desconocido" : tipoActual, contadorId });
                contadorId++;  // Incrementar el identificador único
            }
        }
    }
}

// Función para mostrar la tabla de símbolos
void MostrarTablaSimbolos() {
    cout << "\nTabla de Simbolos:\n";
    cout << "=====================================\n";
    cout << setw(15) << left << "Identificador"
        << setw(20) << left << "Token"
        << setw(10) << left << "Tipo" << endl;
    cout << "=====================================\n";
    for (const auto& simbolo : tablaSimbolos) {
        cout << setw(15) << left << simbolo.nombre  // Alineación a la izquierda con un ancho de 15
            << setw(20) << left << "<id, " + to_string(simbolo.id) + ">"  // Alineación a la izquierda con un ancho de 20
            << setw(10) << left << simbolo.tipo << endl;  // Alineación a la izquierda con un ancho de 10
    }
}

// Función para guardar la tabla de símbolos en un archivo
void GuardarTablaSimbolosEnArchivo(const string& archivo) {
    ofstream outFile(archivo);
    if (!outFile.is_open()) {
        cerr << "Error al abrir el archivo para escritura." << endl;
        return;
    }

    outFile << "Tabla de Simbolos:\n";
    outFile << "=====================================\n";
    outFile << setw(15) << left << "Identificador"
        << setw(20) << left << "Token"
        << setw(10) << left << "Tipo" << endl;
    outFile << "=====================================\n";
    for (const auto& simbolo : tablaSimbolos) {
        outFile << setw(15) << left << simbolo.nombre  // Alineación a la izquierda con un ancho de 15
            << setw(20) << left << "<id, " + to_string(simbolo.id) + ">"  // Alineación a la izquierda con un ancho de 20
            << setw(10) << left << simbolo.tipo << endl;  // Alineación a la izquierda con un ancho de 10
    }

    outFile.close();
    cout << "Tabla de símbolos guardada en el archivo " << archivo << endl;
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

// Estructura del árbol sintáctico
struct Nodo {
    string valor;
    vector<shared_ptr<Nodo>> hijos;
};

// Analizador sintáctico utilizando los tokens
class AnalizadorSintactico {
public:
    AnalizadorSintactico(const vector<Token>& tokens) : tokens(tokens), pos(0) {}

    void parse() {
        shared_ptr<Nodo> raiz = expresion();
        if (pos < tokens.size()) {
            throw runtime_error("Error sintáctico: tokens adicionales después de la expresión.");
        }

        // Si llegamos aquí, la expresión es válida
        cout << "Análisis sintáctico completado sin errores." << endl;
        cout << "Árbol sintáctico:" << endl;
        imprimirArbol(raiz);
    }

private:
    vector<Token> tokens;
    size_t pos;

    shared_ptr<Nodo> expresion() {
        shared_ptr<Nodo> nodoTermino = termino();
        while (pos < tokens.size() && (tokens[pos].value == "+" || tokens[pos].value == "-")) {
            string operador = tokens[pos].value;
            pos++;
            shared_ptr<Nodo> nodoTerminoDerecho = termino();
            shared_ptr<Nodo> nodoOperacion = make_shared<Nodo>();
            nodoOperacion->valor = operador;
            nodoOperacion->hijos.push_back(nodoTermino);
            nodoOperacion->hijos.push_back(nodoTerminoDerecho);
            nodoTermino = nodoOperacion;
        }
        return nodoTermino;
    }

    shared_ptr<Nodo> termino() {
        shared_ptr<Nodo> nodoFactor = factor();
        while (pos < tokens.size() && (tokens[pos].value == "*" || tokens[pos].value == "/")) {
            string operador = tokens[pos].value;
            pos++;
            shared_ptr<Nodo> nodoFactorDerecho = factor();
            shared_ptr<Nodo> nodoOperacion = make_shared<Nodo>();
            nodoOperacion->valor = operador;
            nodoOperacion->hijos.push_back(nodoFactor);
            nodoOperacion->hijos.push_back(nodoFactorDerecho);
            nodoFactor = nodoOperacion;
        }
        return nodoFactor;
    }

    shared_ptr<Nodo> factor() {
        if (pos >= tokens.size()) {
            throw runtime_error("Error sintáctico: se esperaba un número o un identificador.");
        }

        shared_ptr<Nodo> nodo;
        if (tokens[pos].type == TipoTokens::Literal || tokens[pos].type == TipoTokens::Identificador) {
            nodo = make_shared<Nodo>();
            nodo->valor = tokens[pos].value;
            pos++;
        }
        else if (tokens[pos].value == "(") {
            pos++;  // Consumir '('
            nodo = expresion();
            if (pos >= tokens.size() || tokens[pos].value != ")") {
                throw runtime_error("Error sintáctico: se esperaba un paréntesis de cierre.");
            }
            pos++;  // Consumir ')'
        }
        else {
            throw runtime_error("Error sintáctico: token inesperado.");
        }

        return nodo;
    }

    void imprimirArbol(shared_ptr<Nodo> nodo, int nivel = 0) {
        if (!nodo) return;

        // Mostrar el valor del nodo con indentación
        cout << string(nivel * 4, ' ') << nodo->valor << endl;

        // Recursivamente imprimir los hijos
        for (const auto& hijo : nodo->hijos) {
            imprimirArbol(hijo, nivel + 1);
        }
    }
};

// Función principal
int main() {
    int opcion;
    do {
        cout << "\nBienvenido, estas son las opciones del programa" << endl;
        cout << "1. Ingresar expresion" << endl;
        cout << "2. Analisis Lexico" << endl;
        cout << "3. Analisis Sintactico" << endl;
        cout << "4. Tabla de Simbolos" << endl;
        cout << "5. Guardar Tabla de Simbolos en archivo" << endl;
        cout << "6. Salir" << endl;
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

        else if (opcion == 3) {
            AnalizadorSintactico analizador(tokens);
            try {
                analizador.parse();
            }
            catch (const runtime_error& e) {
                cerr << "Error: " << e.what() << endl;
            }
        }
        else if (opcion == 4) {
            ActualizarTablaSimbolos();
            MostrarTablaSimbolos();
        }
        else if (opcion == 5) {
            GuardarTablaSimbolosEnArchivo("tabla_simbolos.txt");
        }

    } while (opcion != 6);

    return 0;
}