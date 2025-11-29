#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <map>
#include <list>

using namespace std;

// ===============================
// ENUMERACIONES Y ESTRUCTURAS
// ===============================

enum class TipoToken {
    DOCUMENTO_INICIO, DOCUMENTO_FIN, CONFIGURAR,
    SECCION, SUBSECCION, SUBSUBSECCION, CAPITULO,
    NEGRITA, CURSIVA, SUBRAYADO, PARRAFO, TACHADO,
    LISTA_SIMPLE, LISTA_NUMERADA, ELEMENTO, FIN_LISTA,
    ECUACION, FORMULA, TABLA, FILA, COLUMNA, FIN_TABLA,
    FIGURA, LEYENDA, REFERENCIA, TITULO, AUTOR, FECHA,
    CENTRAR, IZQUIERDA, DERECHA, JUSTIFICAR,
    SALTO_LINEA, SALTO_PAGINA, NUEVA_PAGINA,
    
    PARENTESIS_IZQ, PARENTESIS_DER, LLAVE_IZQ, LLAVE_DER,
    CORCHETE_IZQ, CORCHETE_DER, COMA, PUNTO_COMA, IGUAL,
    PORCENTAJE, DOLAR, AMPERSAND,
    
    IDENTIFICADOR, CADENA, NUMERO, TEXTO_SIMPLE,
    
    FIN_ARCHIVO
};

const int FIN = 666;
const int ERROR_SINTACTICO = 999;

struct Token {
    TipoToken tipo;
    string valor;
    int linea;
    int columna;
    
    Token(TipoToken t, const string& v, int l, int c) 
        : tipo(t), valor(v), linea(l), columna(c) {}
    
    string toString() const {
        static map<TipoToken, string> nombresTipo = {
            {TipoToken::DOCUMENTO_INICIO, "DOCUMENTO_INICIO"},
            {TipoToken::DOCUMENTO_FIN, "DOCUMENTO_FIN"},
            {TipoToken::CONFIGURAR, "CONFIGURAR"},
            {TipoToken::SECCION, "SECCION"},
            {TipoToken::SUBSECCION, "SUBSECCION"},
            {TipoToken::SUBSUBSECCION, "SUBSUBSECCION"},
            {TipoToken::CAPITULO, "CAPITULO"},
            {TipoToken::NEGRITA, "NEGRITA"},
            {TipoToken::CURSIVA, "CURSIVA"},
            {TipoToken::SUBRAYADO, "SUBRAYADO"},
            {TipoToken::TACHADO, "TACHADO"},
            {TipoToken::PARRAFO, "PARRAFO"},
            {TipoToken::LISTA_SIMPLE, "LISTA_SIMPLE"},
            {TipoToken::LISTA_NUMERADA, "LISTA_NUMERADA"},
            {TipoToken::ELEMENTO, "ELEMENTO"},
            {TipoToken::FIN_LISTA, "FIN_LISTA"},
            {TipoToken::ECUACION, "ECUACION"},
            {TipoToken::FORMULA, "FORMULA"},
            {TipoToken::TITULO, "TITULO"},
            {TipoToken::AUTOR, "AUTOR"},
            {TipoToken::FECHA, "FECHA"},
            {TipoToken::SALTO_LINEA, "SALTO_LINEA"},
            {TipoToken::SALTO_PAGINA, "SALTO_PAGINA"},
            {TipoToken::NUEVA_PAGINA, "NUEVA_PAGINA"},
            {TipoToken::PUNTO_COMA, "PUNTO_COMA"},
            {TipoToken::PARENTESIS_IZQ, "PARENTESIS_IZQ"},
            {TipoToken::PARENTESIS_DER, "PARENTESIS_DER"},
            {TipoToken::CADENA, "CADENA"},
            {TipoToken::COMA, "COMA"},
            {TipoToken::TEXTO_SIMPLE, "TEXTO_SIMPLE"}
        };
        
        auto it = nombresTipo.find(tipo);
        if (it != nombresTipo.end()) {
            return it->second + "('" + valor + "')";
        }
        return "TOKEN('" + valor + "')";
    }
    
    int getTokenValue() const {
        return static_cast<int>(tipo);
    }
};

// ===============================
// TABLA DE SIMBOLOS
// ===============================

class Atributos {
public:
    string lexema;
    int token;
    string tipo;
    string valor;
    string estado;
    
    Atributos() : lexema(""), token(-999), tipo(""), valor("nulo"), estado("") {}
    
    Atributos(string lex, int tok, string tip, string val, string est) 
        : lexema(lex), token(tok), tipo(tip), valor(val), estado(est) {}
    
    void Mostrar() {
        cout << "Tipo(" << tipo << ") \t";
        cout << "Lexema(" << lexema << ") \t";
        cout << "Token(" << token << ") \t";
        cout << "Valor(" << valor << ") \t";
        cout << "Estado(" << estado << ")" << endl;
    }
};

class TablaSimbolos {
private:
    list<Atributos> tabla;

public:
    void Insertar(string lex, int tok, string tip, string val, string est) {
        Atributos attr(lex, tok, tip, val, est);
        tabla.push_back(attr);
    }
    
    void Mostrar() {
        cout << "\nTABLA DE SIMBOLOS:\n";
        cout << "==================\n";
        for (auto item : tabla) {
            item.Mostrar();
        }
    }
    
    bool Buscar(string lex, Atributos& attr) {
        for (auto item : tabla) {
            if (item.lexema == lex) {
                attr = item;
                return true;
            }
        }
        return false;
    }
    
    bool BuscarPalabraClave(string lex, Atributos& attr) {
        for (auto item : tabla) {
            if (item.lexema == lex && item.tipo == "pclave") {
                attr = item;
                return true;
            }
        }
        return false;
    }
};

// ===============================
// ESTRUCTURAS DEL ARBOL SINTACTICO
// ===============================

struct NodoAST {
    virtual ~NodoAST() = default;
    virtual string toString() const = 0;
    virtual string toLatex() const = 0;
};

struct NodoDocumento : public NodoAST {
    vector<unique_ptr<NodoAST>> hijos;
    
    string toString() const override {
        return "Documento(" + to_string(hijos.size()) + " hijos)";
    }
    
    string toLatex() const override {
        stringstream ss;
        for (auto& hijo : hijos) {
            ss << hijo->toLatex();
        }
        return ss.str();
    }
};

struct NodoConfiguracion : public NodoAST {
    string clave;
    string valor;
    NodoConfiguracion(const string& k, const string& v) : clave(k), valor(v) {}
    
    string toString() const override {
        return "Configurar(" + clave + " = " + valor + ")";
    }
    
    string toLatex() const override {
        return ""; // Las configuraciones se manejan en el preámbulo
    }
};

struct NodoSeccion : public NodoAST {
    string nivel;
    string titulo;
    NodoSeccion(const string& l, const string& t) : nivel(l), titulo(t) {}
    
    string toString() const override {
        return nivel + "(\"" + titulo + "\")";
    }
    
    string toLatex() const override {
        return "\\" + nivel + "{" + titulo + "}\n\n";
    }
};

struct NodoTexto : public NodoAST {
    string contenido;
    string estilo;
    NodoTexto(const string& c, const string& s = "normal") 
        : contenido(c), estilo(s) {}
    
    string toString() const override {
        return "Texto[" + estilo + "](\"" + contenido + "\")";
    }
    
    string toLatex() const override {
        if (estilo == "negrita") {
            return "\\textbf{" + contenido + "}\n\n";
        } else if (estilo == "cursiva") {
            return "\\textit{" + contenido + "}\n\n";
        } else if (estilo == "subrayado") {
            return "\\underline{" + contenido + "}\n\n";
        } else if (estilo == "tachado") {
            return "\\sout{" + contenido + "}\n\n";
        } else {
            return contenido + "\n\n";
        }
    }
};

struct NodoLista : public NodoAST {
    string tipo;
    vector<unique_ptr<NodoAST>> elementos;
    
    NodoLista(const string& t) : tipo(t) {}
    
    string toString() const override {
        return "Lista[" + tipo + "](" + to_string(elementos.size()) + " elementos)";
    }
    
    string toLatex() const override {
        stringstream ss;
        if (tipo == "simple") {
            ss << "\\begin{itemize}\n";
        } else {
            ss << "\\begin{enumerate}\n";
        }
        
        for (auto& elem : elementos) {
            ss << "  \\item " << elem->toLatex();
        }
        
        if (tipo == "simple") {
            ss << "\\end{itemize}\n\n";
        } else {
            ss << "\\end{enumerate}\n\n";
        }
        return ss.str();
    }
};

struct NodoElementoLista : public NodoAST {
    string contenido;
    
    NodoElementoLista(const string& c) : contenido(c) {}
    
    string toString() const override {
        return "Elemento(\"" + contenido + "\")";
    }
    
    string toLatex() const override {
        return contenido + "\n";
    }
};

struct NodoEcuacion : public NodoAST {
    string contenido;
    string tipo;
    NodoEcuacion(const string& c, const string& t = "ecuacion") : contenido(c), tipo(t) {}
    
    string toString() const override {
        return "Ecuacion[" + tipo + "](" + contenido + ")";
    }
    
    string toLatex() const override {
        if (tipo == "ecuacion") {
            return "\\begin{equation}\n" + contenido + "\n\\end{equation}\n\n";
        } else {
            return "$" + contenido + "$\n\n";
        }
    }
};

struct NodoMetadatos : public NodoAST {
    string tipo;
    string contenido;
    NodoMetadatos(const string& t, const string& c) : tipo(t), contenido(c) {}
    
    string toString() const override {
        return tipo + "(\"" + contenido + "\")";
    }
    
    string toLatex() const override {
        return ""; // Se maneja en el preámbulo
    }
};

struct NodoComando : public NodoAST {
    string comando;
    
    NodoComando(const string& cmd) : comando(cmd) {}
    
    string toString() const override {
        return "Comando(" + comando + ")";
    }
    
    string toLatex() const override {
        if (comando == "salto_linea") {
            return "\\\\\n";
        } else if (comando == "salto_pagina") {
            return "\\pagebreak\n\n";
        } else if (comando == "nueva_pagina") {
            return "\\newpage\n\n";
        }
        return "";
    }
};

// ===============================
// ANALIZADOR LEXICO
// ===============================

class AnalizadorLexico {
private:
    string entrada;
    size_t posicion;
    int linea;
    int columna;
    TablaSimbolos tablaSimbolos;
    
    char mirarSiguiente() {
        return posicion < entrada.length() ? entrada[posicion] : '\0';
    }
    
    char avanzar() {
        if (posicion >= entrada.length()) return '\0';
        char c = entrada[posicion++];
        if (c == '\n') {
            linea++;
            columna = 1;
        } else {
            columna++;
        }
        return c;
    }
    
    void saltarEspacios() {
        while (isspace(mirarSiguiente()) && mirarSiguiente() != '\n') {
            avanzar();
        }
    }
    
    void saltarComentario() {
        while (mirarSiguiente() != '\n' && mirarSiguiente() != '\0') {
            avanzar();
        }
    }
    
    Token leerCadena() {
        avanzar(); // saltar comilla inicial
        string valor;
        int lineaInicio = linea;
        int columnaInicio = columna;
        
        while (mirarSiguiente() != '"' && mirarSiguiente() != '\0') {
            if (mirarSiguiente() == '\\') {
                avanzar();
                switch (mirarSiguiente()) {
                    case 'n': valor += '\n'; break;
                    case 't': valor += '\t'; break;
                    case '"': valor += '"'; break;
                    case '\\': valor += '\\'; break;
                    default: valor += '\\'; valor += mirarSiguiente(); break;
                }
                avanzar();
            } else {
                valor += avanzar();
            }
        }
        
        if (mirarSiguiente() == '"') {
            avanzar();
        }
        
        return Token(TipoToken::CADENA, valor, lineaInicio, columnaInicio);
    }
    
    Token leerIdentificador() {
        string valor;
        int lineaInicio = linea;
        int columnaInicio = columna;
        
        while (isalnum(mirarSiguiente()) || mirarSiguiente() == '_') {
            valor += avanzar();
        }
        
        string valorMinusculas = valor;
        transform(valorMinusculas.begin(), valorMinusculas.end(), valorMinusculas.begin(), ::tolower);
        
        Atributos attr;
        if (tablaSimbolos.BuscarPalabraClave(valorMinusculas, attr)) {
            return Token(static_cast<TipoToken>(attr.token), valor, lineaInicio, columnaInicio);
        }
        
        tablaSimbolos.Insertar(valor, static_cast<int>(TipoToken::IDENTIFICADOR), "identificador", "nulo", "no_asignado");
        return Token(TipoToken::IDENTIFICADOR, valor, lineaInicio, columnaInicio);
    }
    
    Token leerTextoSimple() {
        string valor;
        int lineaInicio = linea;
        int columnaInicio = columna;
        
        while (mirarSiguiente() != ';' && mirarSiguiente() != '\n' && mirarSiguiente() != '\0') {
            valor += avanzar();
        }
        
        // Eliminar espacios al final
        while (!valor.empty() && isspace(valor.back())) {
            valor.pop_back();
        }
        
        if (!valor.empty()) {
            tablaSimbolos.Insertar(valor, static_cast<int>(TipoToken::TEXTO_SIMPLE), "texto", valor, "asignado");
        }
        
        return Token(TipoToken::TEXTO_SIMPLE, valor, lineaInicio, columnaInicio);
    }

public:
    AnalizadorLexico(const string& entrada) 
        : entrada(entrada), posicion(0), linea(1), columna(1) {
        
        // Palabras reservadas
        tablaSimbolos.Insertar("documento_inicio", static_cast<int>(TipoToken::DOCUMENTO_INICIO), "pclave", "-", "-");
        tablaSimbolos.Insertar("documento_fin", static_cast<int>(TipoToken::DOCUMENTO_FIN), "pclave", "-", "-");
        tablaSimbolos.Insertar("configurar", static_cast<int>(TipoToken::CONFIGURAR), "pclave", "-", "-");
        tablaSimbolos.Insertar("seccion", static_cast<int>(TipoToken::SECCION), "pclave", "-", "-");
        tablaSimbolos.Insertar("subseccion", static_cast<int>(TipoToken::SUBSECCION), "pclave", "-", "-");
        tablaSimbolos.Insertar("subsubseccion", static_cast<int>(TipoToken::SUBSUBSECCION), "pclave", "-", "-");
        tablaSimbolos.Insertar("capitulo", static_cast<int>(TipoToken::CAPITULO), "pclave", "-", "-");
        tablaSimbolos.Insertar("negrita", static_cast<int>(TipoToken::NEGRITA), "pclave", "-", "-");
        tablaSimbolos.Insertar("cursiva", static_cast<int>(TipoToken::CURSIVA), "pclave", "-", "-");
        tablaSimbolos.Insertar("subrayado", static_cast<int>(TipoToken::SUBRAYADO), "pclave", "-", "-");
        tablaSimbolos.Insertar("tachado", static_cast<int>(TipoToken::TACHADO), "pclave", "-", "-");
        tablaSimbolos.Insertar("parrafo", static_cast<int>(TipoToken::PARRAFO), "pclave", "-", "-");
        tablaSimbolos.Insertar("lista_simple", static_cast<int>(TipoToken::LISTA_SIMPLE), "pclave", "-", "-");
        tablaSimbolos.Insertar("lista_numerada", static_cast<int>(TipoToken::LISTA_NUMERADA), "pclave", "-", "-");
        tablaSimbolos.Insertar("elemento", static_cast<int>(TipoToken::ELEMENTO), "pclave", "-", "-");
        tablaSimbolos.Insertar("fin_lista", static_cast<int>(TipoToken::FIN_LISTA), "pclave", "-", "-");
        tablaSimbolos.Insertar("ecuacion", static_cast<int>(TipoToken::ECUACION), "pclave", "-", "-");
        tablaSimbolos.Insertar("formula", static_cast<int>(TipoToken::FORMULA), "pclave", "-", "-");
        tablaSimbolos.Insertar("titulo", static_cast<int>(TipoToken::TITULO), "pclave", "-", "-");
        tablaSimbolos.Insertar("autor", static_cast<int>(TipoToken::AUTOR), "pclave", "-", "-");
        tablaSimbolos.Insertar("fecha", static_cast<int>(TipoToken::FECHA), "pclave", "-", "-");
        tablaSimbolos.Insertar("salto_linea", static_cast<int>(TipoToken::SALTO_LINEA), "pclave", "-", "-");
        tablaSimbolos.Insertar("salto_pagina", static_cast<int>(TipoToken::SALTO_PAGINA), "pclave", "-", "-");
        tablaSimbolos.Insertar("nueva_pagina", static_cast<int>(TipoToken::NUEVA_PAGINA), "pclave", "-", "-");
        
        // Símbolos
        tablaSimbolos.Insertar("(", static_cast<int>(TipoToken::PARENTESIS_IZQ), "pclave", "-", "-");
        tablaSimbolos.Insertar(")", static_cast<int>(TipoToken::PARENTESIS_DER), "pclave", "-", "-");
        tablaSimbolos.Insertar(",", static_cast<int>(TipoToken::COMA), "pclave", "-", "-");
        tablaSimbolos.Insertar(";", static_cast<int>(TipoToken::PUNTO_COMA), "pclave", "-", "-");
    }
    
    vector<Token> analizar() {
        vector<Token> tokens;
        
        while (posicion < entrada.length()) {
            saltarEspacios();
            
            char c = mirarSiguiente();
            if (c == '\0') break;
            
            int lineaActual = linea;
            int columnaActual = columna;
            
            if (c == '"') {
                tokens.push_back(leerCadena());
            } else if (c == '%') {
                saltarComentario();
            } else if (c == '(') {
                tokens.push_back(Token(TipoToken::PARENTESIS_IZQ, "(", lineaActual, columnaActual));
                avanzar();
            } else if (c == ')') {
                tokens.push_back(Token(TipoToken::PARENTESIS_DER, ")", lineaActual, columnaActual));
                avanzar();
            } else if (c == ',') {
                tokens.push_back(Token(TipoToken::COMA, ",", lineaActual, columnaActual));
                avanzar();
            } else if (c == ';') {
                tokens.push_back(Token(TipoToken::PUNTO_COMA, ";", lineaActual, columnaActual));
                avanzar();
            } else if (c == '\n') {
                avanzar();
            } else if (isalpha(c) || c == '_') {
                Token token = leerIdentificador();
                tokens.push_back(token);
                
                // Si es un comando sin paréntesis, leer texto simple hasta ;
                if (token.tipo == TipoToken::PARRAFO || token.tipo == TipoToken::NEGRITA ||
                    token.tipo == TipoToken::CURSIVA || token.tipo == TipoToken::SUBRAYADO ||
                    token.tipo == TipoToken::TACHADO) {
                    saltarEspacios();
                    if (mirarSiguiente() != '(' && mirarSiguiente() != ';') {
                        Token textoSimple = leerTextoSimple();
                        if (!textoSimple.valor.empty()) {
                            tokens.push_back(textoSimple);
                        }
                    }
                }
            } else {
                avanzar(); // Ignorar caracteres desconocidos
            }
        }
        
        tokens.push_back(Token(TipoToken::FIN_ARCHIVO, "", linea, columna));
        return tokens;
    }
    
    void mostrarTablaSimbolos() {
        tablaSimbolos.Mostrar();
    }
};

// ===============================
// ANALIZADOR SINTACTICO
// ===============================

class AnalizadorSintactico {
private:
    vector<Token> tokens;
    size_t actual;
    int estado;
    int tTransicion[100][100];
    unique_ptr<NodoDocumento> ast;
    
    void inicializarTablaTransiciones() {
        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                tTransicion[i][j] = ERROR_SINTACTICO;
            }
        }

        // documento_inicio ;
        tTransicion[0][static_cast<int>(TipoToken::DOCUMENTO_INICIO)] = 1;
        tTransicion[1][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 101;
        tTransicion[101][static_cast<int>(TipoToken::PARENTESIS_DER)] = 102;
        tTransicion[102][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;
        
        tTransicion[1][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // Metadatos
        tTransicion[2][static_cast<int>(TipoToken::TITULO)] = 3;
        tTransicion[2][static_cast<int>(TipoToken::AUTOR)] = 3;
        tTransicion[2][static_cast<int>(TipoToken::FECHA)] = 3;
        tTransicion[3][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 4;
        tTransicion[4][static_cast<int>(TipoToken::CADENA)] = 5;
        tTransicion[5][static_cast<int>(TipoToken::PARENTESIS_DER)] = 6;
        tTransicion[6][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // configurar
        tTransicion[2][static_cast<int>(TipoToken::CONFIGURAR)] = 7;
        tTransicion[7][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 8;
        tTransicion[8][static_cast<int>(TipoToken::CADENA)] = 9;
        tTransicion[9][static_cast<int>(TipoToken::COMA)] = 10;
        tTransicion[10][static_cast<int>(TipoToken::CADENA)] = 11;
        tTransicion[11][static_cast<int>(TipoToken::PARENTESIS_DER)] = 12;
        tTransicion[12][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // Secciones
        tTransicion[2][static_cast<int>(TipoToken::SECCION)] = 13;
        tTransicion[2][static_cast<int>(TipoToken::SUBSECCION)] = 13;
        tTransicion[2][static_cast<int>(TipoToken::SUBSUBSECCION)] = 13;
        tTransicion[2][static_cast<int>(TipoToken::CAPITULO)] = 13;
        tTransicion[13][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 14;
        tTransicion[14][static_cast<int>(TipoToken::CADENA)] = 15;
        tTransicion[15][static_cast<int>(TipoToken::PARENTESIS_DER)] = 16;
        tTransicion[16][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // Texto con paréntesis
        tTransicion[2][static_cast<int>(TipoToken::PARRAFO)] = 17;
        tTransicion[2][static_cast<int>(TipoToken::NEGRITA)] = 17;
        tTransicion[2][static_cast<int>(TipoToken::CURSIVA)] = 17;
        tTransicion[2][static_cast<int>(TipoToken::SUBRAYADO)] = 17;
        tTransicion[2][static_cast<int>(TipoToken::TACHADO)] = 17;
        tTransicion[17][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 18;
        tTransicion[18][static_cast<int>(TipoToken::CADENA)] = 19;
        tTransicion[19][static_cast<int>(TipoToken::PARENTESIS_DER)] = 20;
        tTransicion[20][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // Texto simple
        tTransicion[17][static_cast<int>(TipoToken::TEXTO_SIMPLE)] = 21;
        tTransicion[21][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // Listas
        tTransicion[2][static_cast<int>(TipoToken::LISTA_SIMPLE)] = 22;
        tTransicion[2][static_cast<int>(TipoToken::LISTA_NUMERADA)] = 22;
        tTransicion[22][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 103;
        tTransicion[103][static_cast<int>(TipoToken::PARENTESIS_DER)] = 104;
        tTransicion[104][static_cast<int>(TipoToken::PUNTO_COMA)] = 23;
        tTransicion[22][static_cast<int>(TipoToken::PUNTO_COMA)] = 23;

        // Elementos de lista
        tTransicion[23][static_cast<int>(TipoToken::ELEMENTO)] = 24;
        tTransicion[24][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 25;
        tTransicion[25][static_cast<int>(TipoToken::CADENA)] = 26;
        tTransicion[26][static_cast<int>(TipoToken::PARENTESIS_DER)] = 27;
        tTransicion[27][static_cast<int>(TipoToken::PUNTO_COMA)] = 23;
        tTransicion[24][static_cast<int>(TipoToken::TEXTO_SIMPLE)] = 28;
        tTransicion[28][static_cast<int>(TipoToken::PUNTO_COMA)] = 23;

        // Fin lista
        tTransicion[23][static_cast<int>(TipoToken::FIN_LISTA)] = 29;
        tTransicion[29][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 105;
        tTransicion[105][static_cast<int>(TipoToken::PARENTESIS_DER)] = 106;
        tTransicion[106][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;
        tTransicion[29][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // Ecuaciones
        tTransicion[2][static_cast<int>(TipoToken::ECUACION)] = 30;
        tTransicion[2][static_cast<int>(TipoToken::FORMULA)] = 30;
        tTransicion[30][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 31;
        tTransicion[31][static_cast<int>(TipoToken::CADENA)] = 32;
        tTransicion[32][static_cast<int>(TipoToken::PARENTESIS_DER)] = 33;
        tTransicion[33][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // Comandos especiales
        tTransicion[2][static_cast<int>(TipoToken::SALTO_LINEA)] = 34;
        tTransicion[2][static_cast<int>(TipoToken::SALTO_PAGINA)] = 34;
        tTransicion[2][static_cast<int>(TipoToken::NUEVA_PAGINA)] = 34;
        tTransicion[34][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 107;
        tTransicion[107][static_cast<int>(TipoToken::PARENTESIS_DER)] = 108;
        tTransicion[108][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;
        tTransicion[34][static_cast<int>(TipoToken::PUNTO_COMA)] = 2;

        // documento_fin
        tTransicion[2][static_cast<int>(TipoToken::DOCUMENTO_FIN)] = 35;
        tTransicion[35][static_cast<int>(TipoToken::PARENTESIS_IZQ)] = 109;
        tTransicion[109][static_cast<int>(TipoToken::PARENTESIS_DER)] = 110;
        tTransicion[110][static_cast<int>(TipoToken::PUNTO_COMA)] = 36;
        tTransicion[35][static_cast<int>(TipoToken::PUNTO_COMA)] = 36;
    }

    Token& mirarSiguiente() {
        return tokens[actual];
    }
    
    Token& avanzar() {
        if (!estaAlFinal()) actual++;
        return tokens[actual - 1];
    }
    
    bool estaAlFinal() {
        return mirarSiguiente().tipo == TipoToken::FIN_ARCHIVO;
    }
    
bool analizarConTablaTransiciones() {
    estado = 0;
    actual = 0;
    
    cout << "Iniciando analisis sintactico...\n";
    cout << "Estado inicial: 0\n\n";
    
    int pasoNumero = 0;
    while (!estaAlFinal()) {
        Token& token = mirarSiguiente();
        int tokenType = token.getTokenValue();
        
        pasoNumero++;
        cout << "Paso " << pasoNumero << ": Estado=" << estado 
             << " Token=" << token.toString() << "\n";
        
        int nuevoEstado = tTransicion[estado][tokenType];
        
        if (nuevoEstado == ERROR_SINTACTICO) {
            cout << "\n┌─────────────────────────────────────┐\n";
            cout << "│  ERROR SINTACTICO DETECTADO        │\n";
            cout << "└─────────────────────────────────────┘\n\n";
            cout << "Ubicacion: Linea " << token.linea << ", Columna " << token.columna << "\n";
            cout << "Token inesperado: " << token.toString() << "\n";
            cout << "Estado actual: " << estado << "\n";
            
            // Mensajes de error más específicos
            if (token.tipo != TipoToken::PUNTO_COMA && estado >= 2 && estado <= 34) {
                cout << "ERROR: Falta punto y coma (;) al final del comando\n";
            } else if (token.tipo == TipoToken::PUNTO_COMA && estado != 2) {
                cout << "ERROR: Punto y coma en posición incorrecta\n";
            }
            
            return false;
        }
        
        // Validación adicional para punto y coma
        if (estado >= 2 && estado <= 34 && token.tipo != TipoToken::PUNTO_COMA) {
            // Estados que requieren punto y coma
            if (nuevoEstado != estado && tTransicion[estado][static_cast<int>(TipoToken::PUNTO_COMA)] != ERROR_SINTACTICO) {
                cout << "\nERROR: Se esperaba punto y coma (;) en línea " << token.linea << "\n";
                return false;
            }
        }
        
        cout << "   -> Transicion a estado: " << nuevoEstado << "\n";
        
        estado = nuevoEstado;
        avanzar();
        
        if (estado == 36) {
            cout << "\n┌─────────────────────────────────────┐\n";
            cout << "│  ESTADO FINAL ALCANZADO (36)       │\n";
            cout << "└─────────────────────────────────────┘\n";
            cout << "Analisis sintactico completado exitosamente.\n";
            return true;
        }
    }
        
        if (estado != 36) {
            cout << "\n┌─────────────────────────────────────┐\n";
            cout << "│  ERROR: DOCUMENTO INCOMPLETO        │\n";
            cout << "└─────────────────────────────────────┘\n\n";
            cout << "Estado final alcanzado: " << estado << " (se esperaba: 36)\n\n";
            
            if (estado < 35) {
                cout << "POSIBLE CAUSA:\n";
                cout << "  - Falta el comando: documento_fin();\n";
            } else if (estado == 35) {
                cout << "POSIBLE CAUSA:\n";
                cout << "  - documento_fin debe tener parentesis: documento_fin();\n";
            }
            
            return false;
        }
        
        return true;
    }
    
    // Métodos para construir el AST
    void construirAST() {
        ast = make_unique<NodoDocumento>();
        actual = 0;
        
        while (!estaAlFinal()) {
            Token& token = mirarSiguiente();
            
            if (token.tipo == TipoToken::DOCUMENTO_INICIO) {
                avanzar();
                if (mirarSiguiente().tipo == TipoToken::PARENTESIS_IZQ) {
                    avanzar();
                    if (mirarSiguiente().tipo == TipoToken::PARENTESIS_DER) {
                        avanzar();
                    }
                }
                if (mirarSiguiente().tipo == TipoToken::PUNTO_COMA) {
                    avanzar();
                }
            }
            else if (token.tipo == TipoToken::DOCUMENTO_FIN) {
                avanzar();
                if (mirarSiguiente().tipo == TipoToken::PARENTESIS_IZQ) {
                    avanzar();
                    if (mirarSiguiente().tipo == TipoToken::PARENTESIS_DER) {
                        avanzar();
                    }
                }
                if (mirarSiguiente().tipo == TipoToken::PUNTO_COMA) {
                    avanzar();
                }
                break;
            }
            else if (token.tipo == TipoToken::TITULO || token.tipo == TipoToken::AUTOR || 
                     token.tipo == TipoToken::FECHA) {
                string tipoMeta = token.valor;
                avanzar();
                avanzar(); // (
                string contenido = mirarSiguiente().valor;
                avanzar(); // cadena
                avanzar(); // )
                avanzar(); // ;
                
                string tipo = (token.tipo == TipoToken::TITULO) ? "titulo" :
                             (token.tipo == TipoToken::AUTOR) ? "autor" : "fecha";
                ast->hijos.push_back(make_unique<NodoMetadatos>(tipo, contenido));
            }
            else if (token.tipo == TipoToken::CONFIGURAR) {
                avanzar();
                avanzar(); // (
                string clave = mirarSiguiente().valor;
                avanzar(); // cadena
                avanzar(); // ,
                string valor = mirarSiguiente().valor;
                avanzar(); // cadena
                avanzar(); // )
                avanzar(); // ;
                
                ast->hijos.push_back(make_unique<NodoConfiguracion>(clave, valor));
            }
            else if (token.tipo == TipoToken::SECCION || token.tipo == TipoToken::SUBSECCION ||
                     token.tipo == TipoToken::SUBSUBSECCION || token.tipo == TipoToken::CAPITULO) {
                string nivel = (token.tipo == TipoToken::SECCION) ? "section" :
                              (token.tipo == TipoToken::SUBSECCION) ? "subsection" :
                              (token.tipo == TipoToken::SUBSUBSECCION) ? "subsubsection" : "chapter";
                avanzar();
                avanzar(); // (
                string titulo = mirarSiguiente().valor;
                avanzar(); // cadena
                avanzar(); // )
                avanzar(); // ;
                
                ast->hijos.push_back(make_unique<NodoSeccion>(nivel, titulo));
            }
            else if (token.tipo == TipoToken::PARRAFO || token.tipo == TipoToken::NEGRITA ||
                     token.tipo == TipoToken::CURSIVA || token.tipo == TipoToken::SUBRAYADO ||
                     token.tipo == TipoToken::TACHADO) {
                string estilo = (token.tipo == TipoToken::NEGRITA) ? "negrita" :
                               (token.tipo == TipoToken::CURSIVA) ? "cursiva" :
                               (token.tipo == TipoToken::SUBRAYADO) ? "subrayado" :
                               (token.tipo == TipoToken::TACHADO) ? "tachado" : "normal";
                avanzar();
                
                string contenido;
                if (mirarSiguiente().tipo == TipoToken::PARENTESIS_IZQ) {
                    avanzar(); // (
                    contenido = mirarSiguiente().valor;
                    avanzar(); // cadena
                    avanzar(); // )
                } else if (mirarSiguiente().tipo == TipoToken::TEXTO_SIMPLE) {
                    contenido = mirarSiguiente().valor;
                    avanzar();
                }
                avanzar(); // ;
                
                ast->hijos.push_back(make_unique<NodoTexto>(contenido, estilo));
            }
            else if (token.tipo == TipoToken::LISTA_SIMPLE || token.tipo == TipoToken::LISTA_NUMERADA) {
                string tipoLista = (token.tipo == TipoToken::LISTA_SIMPLE) ? "simple" : "numerada";
                avanzar();
                
                if (mirarSiguiente().tipo == TipoToken::PARENTESIS_IZQ) {
                    avanzar(); // (
                    avanzar(); // )
                }
                avanzar(); // ;
                
                auto lista = make_unique<NodoLista>(tipoLista);
                
                while (mirarSiguiente().tipo == TipoToken::ELEMENTO) {
                    avanzar();
                    string contenido;
                    
                    if (mirarSiguiente().tipo == TipoToken::PARENTESIS_IZQ) {
                        avanzar(); // (
                        contenido = mirarSiguiente().valor;
                        avanzar(); // cadena
                        avanzar(); // )
                    } else if (mirarSiguiente().tipo == TipoToken::TEXTO_SIMPLE) {
                        contenido = mirarSiguiente().valor;
                        avanzar();
                    }
                    avanzar(); // ;
                    
                    lista->elementos.push_back(make_unique<NodoElementoLista>(contenido));
                }
                
                if (mirarSiguiente().tipo == TipoToken::FIN_LISTA) {
                    avanzar();
                    if (mirarSiguiente().tipo == TipoToken::PARENTESIS_IZQ) {
                        avanzar(); // (
                        avanzar(); // )
                    }
                    avanzar(); // ;
                }
                
                ast->hijos.push_back(move(lista));
            }
            else if (token.tipo == TipoToken::ECUACION || token.tipo == TipoToken::FORMULA) {
                string tipo = (token.tipo == TipoToken::ECUACION) ? "ecuacion" : "formula";
                avanzar();
                avanzar(); // (
                string contenido = mirarSiguiente().valor;
                avanzar(); // cadena
                avanzar(); // )
                avanzar(); // ;
                
                ast->hijos.push_back(make_unique<NodoEcuacion>(contenido, tipo));
            }
            else if (token.tipo == TipoToken::SALTO_LINEA || token.tipo == TipoToken::SALTO_PAGINA ||
                     token.tipo == TipoToken::NUEVA_PAGINA) {
                string comando = (token.tipo == TipoToken::SALTO_LINEA) ? "salto_linea" :
                                (token.tipo == TipoToken::SALTO_PAGINA) ? "salto_pagina" : "nueva_pagina";
                avanzar();
                if (mirarSiguiente().tipo == TipoToken::PARENTESIS_IZQ) {
                    avanzar(); // (
                    avanzar(); // )
                }
                avanzar(); // ;
                
                ast->hijos.push_back(make_unique<NodoComando>(comando));
            }
            else {
                avanzar();
            }
        }
    }

public:
    AnalizadorSintactico(const vector<Token>& tokens) 
        : tokens(tokens), actual(0), estado(0) {
        inicializarTablaTransiciones();
    }
    
    bool analizarSintaxis() {
        bool resultado = analizarConTablaTransiciones();
        if (resultado) {
            construirAST();
        }
        return resultado;
    }
    
    unique_ptr<NodoDocumento> obtenerAST() {
        return move(ast);
    }
    
    void mostrarTokens() {
        cout << "\nTOKENS RECONOCIDOS:\n";
        cout << "===================\n";
        for (size_t i = 0; i < tokens.size() && i < 50; ++i) {
            cout << "Linea " << tokens[i].linea << ", Col " << tokens[i].columna 
                 << ": " << tokens[i].toString() << endl;
        }
        if (tokens.size() > 50) {
            cout << "... y " << (tokens.size() - 50) << " tokens mas.\n";
        }
    }
};

// ===============================
// GENERADOR DE LATEX
// ===============================

class GeneradorLatex {
private:
    string titulo;
    string autor;
    string fecha;
    bool tieneTitulo;
    
public:
    GeneradorLatex() : tieneTitulo(false) {}
    
    string generar(NodoDocumento* ast) {
        stringstream salida;
        
        // Primero extraer metadatos
        titulo = "";
        autor = "";
        fecha = "";
        tieneTitulo = false;
        
        for (auto& hijo : ast->hijos) {
            if (auto meta = dynamic_cast<NodoMetadatos*>(hijo.get())) {
                if (meta->tipo == "titulo") {
                    titulo = meta->contenido;
                    tieneTitulo = true;
                } else if (meta->tipo == "autor") {
                    autor = meta->contenido;
                } else if (meta->tipo == "fecha") {
                    fecha = meta->contenido;
                }
            }
        }
        
        // Preámbulo
        salida << "\\documentclass{article}\n";
        salida << "\\usepackage[utf8]{inputenc}\n";
        salida << "\\usepackage[spanish]{babel}\n";
        salida << "\\usepackage{amsmath}\n";
        salida << "\\usepackage{amssymb}\n";
        salida << "\\usepackage{ulem}\n";
        salida << "\n";
        
        // Metadatos
        if (!titulo.empty()) {
            salida << "\\title{" << titulo << "}\n";
        }
        if (!autor.empty()) {
            salida << "\\author{" << autor << "}\n";
        }
        if (!fecha.empty()) {
            salida << "\\date{" << fecha << "}\n";
        }
        
        salida << "\n\\begin{document}\n\n";
        
        if (tieneTitulo) {
            salida << "\\maketitle\n\n";
        }
        
        // Contenido
        for (auto& hijo : ast->hijos) {
            if (!dynamic_cast<NodoMetadatos*>(hijo.get()) && 
                !dynamic_cast<NodoConfiguracion*>(hijo.get())) {
                salida << hijo->toLatex();
            }
        }
        
        salida << "\\end{document}\n";
        
        return salida.str();
    }
};

// ===============================
// FUNCIONES UTILITARIAS
// ===============================

string leerArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        throw runtime_error("No se pudo abrir el archivo: " + nombreArchivo);
    }
    
    stringstream buffer;
    buffer << archivo.rdbuf();
    return buffer.str();
}

void escribirArchivo(const string& nombreArchivo, const string& contenido) {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        throw runtime_error("No se pudo crear el archivo: " + nombreArchivo);
    }
    
    archivo << contenido;
}

// ===============================
// FUNCIONES DEL MENU
// ===============================

void compilarArchivo() {
    string nombreArchivo;
    cout << "Nombre del archivo .texes a compilar: ";
    cin >> nombreArchivo;
    
    if (nombreArchivo.find(".texes") == string::npos) {
        nombreArchivo += ".texes";
    }
    
    try {
        string entrada = leerArchivo(nombreArchivo);
        cout << "\n=== COMPILANDO: " << nombreArchivo << " ===\n";
        
        // Análisis léxico
        AnalizadorLexico lexico(entrada);
        auto tokens = lexico.analizar();
        
        cout << "\n[1/3] Analisis lexico completado: " << tokens.size() << " tokens\n";
        
        // Análisis sintáctico
        AnalizadorSintactico sintactico(tokens);
        
        if (!sintactico.analizarSintaxis()) {
            cout << "\nERROR: El analisis sintactico fallo.\n";
            cout << "Por favor revisa la sintaxis de tu documento.\n";
            return;
        }
        
        cout << "[2/3] Analisis sintactico completado exitosamente\n";
        
        // Generación de código
        auto ast = sintactico.obtenerAST();
        GeneradorLatex generador;
        string salidaLatex = generador.generar(ast.get());
        
        cout << "[3/3] Generacion de codigo LaTeX completada\n";
        
        // Guardar archivo
        string nombreArchivoSalida = nombreArchivo.substr(0, nombreArchivo.find(".texes")) + ".tex";
        escribirArchivo(nombreArchivoSalida, salidaLatex);
        
        cout << "\n=== COMPILACION EXITOSA ===\n";
        cout << "Archivo generado: " << nombreArchivoSalida << "\n";
        cout << "Para generar PDF: pdflatex " << nombreArchivoSalida << "\n";
        
        // Estadísticas
        cout << "\nESTADISTICAS:\n";
        cout << "- Tokens: " << tokens.size() << "\n";
        cout << "- Nodos AST: " << ast->hijos.size() << "\n";
        cout << "- Lineas LaTeX: " << count(salidaLatex.begin(), salidaLatex.end(), '\n') << "\n";
        
    } catch (const exception& e) {
        cout << "\nERROR: " << e.what() << "\n";
    }
}

void crearNuevoDocumento() {
    string nombreArchivo;
    cout << "Nombre del archivo (sin extension): ";
    cin >> nombreArchivo;
    nombreArchivo += ".texes";
    
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "Error al crear el archivo.\n";
        return;
    }
    
    archivo << R"(documento_inicio();

titulo("Mi Nuevo Documento");
autor("Tu Nombre");
fecha("2024");

seccion("Introduccion");
parrafo("Este es mi primer documento usando el compilador LaTeX en español.");

subseccion("Objetivos");
lista_simple();
elemento("Aprender a usar el compilador");
elemento("Crear documentos academicos");
elemento("Escribir matematicas facilmente");
fin_lista();

seccion("Desarrollo");
parrafo("Aqui puedes escribir el contenido principal de tu documento.");

negrita("Texto importante en negrita");
cursiva("Texto con enfasis en cursiva");
subrayado("Texto subrayado para resaltar");

subseccion("Formulas Matematicas");
ecuacion("E = mc^2");
ecuacion("\\sum_{i=1}^{n} i = \\frac{n(n+1)}{2}");

subseccion("Texto Simple");
parrafo Este es un ejemplo de texto sin comillas;
parrafo que demuestra la flexibilidad del compilador;

salto_linea();

documento_fin();
)";
    
    cout << "\nArchivo creado: " << nombreArchivo << "\n";
    cout << "Ahora puedes compilarlo con la opcion 1.\n";
}

void compilarInteractivo() {
    cout << "\n=== EDITOR INTERACTIVO ===\n";
    cout << "Escribe tu documento linea por linea.\n";
    cout << "Escribe 'FIN' para terminar y compilar.\n\n";
    
    stringstream entrada;
    string linea;
    int numeroLinea = 1;
    
    while (true) {
        cout << numeroLinea << "> ";
        getline(cin, linea);
        
        if (linea == "FIN" || linea == "fin") {
            break;
        }
        
        entrada << linea << "\n";
        numeroLinea++;
    }
    
    try {
        string contenido = entrada.str();
        if (contenido.empty()) {
            cout << "\nERROR: No se ingreso contenido.\n";
            return;
        }
        
        cout << "\n=================================\n";
        cout << "Procesando documento...\n";
        cout << "=================================\n\n";
        
        // Mostrar el contenido que se va a compilar
        cout << "CONTENIDO A COMPILAR:\n";
        cout << "-----------------\n";
        cout << contenido;
        cout << "-----------------\n\n";
        
        // Análisis léxico
        cout << "[PASO 1/3] Analizando tokens...\n";
        AnalizadorLexico lexico(contenido);
        auto tokens = lexico.analizar();
        cout << "   -> " << tokens.size() << " tokens reconocidos\n\n";
        
        // Mostrar algunos tokens para debug
        cout << "PRIMEROS TOKENS:\n";
        for (size_t i = 0; i < min(size_t(10), tokens.size()); i++) {
            cout << "   " << tokens[i].toString() << "\n";
        }
        cout << "\n";
        
        // Análisis sintáctico
        cout << "[PASO 2/3] Analizando sintaxis...\n";
        AnalizadorSintactico sintactico(tokens);
        
        bool sintaxisCorrecta = sintactico.analizarSintaxis();
        cout << "\n";
        
        if (!sintaxisCorrecta) {
            cout << "╔════════════════════════════════════════╗\n";
            cout << "║  ERROR: SINTAXIS INCORRECTA           ║\n";
            cout << "╚════════════════════════════════════════╝\n\n";
            cout << "AYUDA: Revisa que tu documento tenga:\n";
            cout << "  1. documento_inicio(); al principio\n";
            cout << "  2. documento_fin(); al final\n";
            cout << "  3. Punto y coma (;) al final de cada linea\n";
            cout << "  4. Parentesis () despues de los comandos\n\n";
            return;
        }
        
        cout << "   -> Sintaxis correcta!\n\n";
        
        // Generación de código
        cout << "[PASO 3/3] Generando codigo LaTeX...\n";
        auto ast = sintactico.obtenerAST();
        
        if (!ast) {
            cout << "ERROR: No se pudo construir el arbol sintactico.\n";
            return;
        }
        
        cout << "   -> AST construido con " << ast->hijos.size() << " nodos\n";
        
        GeneradorLatex generador;
        string salidaLatex = generador.generar(ast.get());
        
        cout << "   -> " << count(salidaLatex.begin(), salidaLatex.end(), '\n') 
             << " lineas de LaTeX generadas\n\n";
        
        // Guardar archivo
        string nombreArchivoSalida = "documento_interactivo.tex";
        escribirArchivo(nombreArchivoSalida, salidaLatex);
        
        // Resultado exitoso
        cout << "╔════════════════════════════════════════╗\n";
        cout << "║     ✓ COMPILACION EXITOSA             ║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        
        cout << "ARCHIVO GENERADO: " << nombreArchivoSalida << "\n\n";
        
        cout << "PREVIEW DEL CODIGO LATEX:\n";
        cout << "========================\n";
        if (salidaLatex.length() > 800) {
            cout << salidaLatex.substr(0, 800) << "\n...(truncado)...\n";
        } else {
            cout << salidaLatex << "\n";
        }
        cout << "========================\n\n";
        
        cout << "PARA GENERAR PDF:\n";
        cout << "  pdflatex " << nombreArchivoSalida << "\n\n";
        
        cout << "ESTADISTICAS:\n";
        cout << "  - Tokens reconocidos: " << tokens.size() << "\n";
        cout << "  - Nodos en AST: " << ast->hijos.size() << "\n";
        cout << "  - Lineas de LaTeX: " << count(salidaLatex.begin(), salidaLatex.end(), '\n') << "\n\n";
        
    } catch (const exception& e) {
        cout << "\n╔════════════════════════════════════════╗\n";
        cout << "║  ERROR DURANTE LA COMPILACION         ║\n";
        cout << "╚════════════════════════════════════════╝\n\n";
        cout << "Mensaje de error: " << e.what() << "\n\n";
        cout << "Por favor verifica tu codigo y vuelve a intentar.\n";
    }
}

void mostrarEjemplos() {
    cout << "\n=== EJEMPLOS DE SINTAXIS ===\n\n";
    
    cout << "1. ESTRUCTURA BASICA:\n";
    cout << "   documento_inicio();\n";
    cout << "   titulo(\"Mi Documento\");\n";
    cout << "   autor(\"Tu Nombre\");\n";
    cout << "   seccion(\"Introduccion\");\n";
    cout << "   parrafo(\"Texto aqui\");\n";
    cout << "   documento_fin();\n\n";
    
    cout << "2. FORMATO DE TEXTO:\n";
    cout << "   negrita(\"Texto en negrita\");\n";
    cout << "   cursiva(\"Texto en cursiva\");\n";
    cout << "   subrayado(\"Texto subrayado\");\n";
    cout << "   tachado(\"Texto tachado\");\n\n";
    
    cout << "3. LISTAS:\n";
    cout << "   lista_simple();\n";
    cout << "   elemento(\"Primer item\");\n";
    cout << "   elemento(\"Segundo item\");\n";
    cout << "   fin_lista();\n\n";
    
    cout << "4. MATEMATICAS:\n";
    cout << "   ecuacion(\"E = mc^2\");\n";
    cout << "   formula(\"a^2 + b^2 = c^2\");\n\n";
    
    cout << "5. TEXTO SIN COMILLAS:\n";
    cout << "   parrafo Texto simple sin comillas;\n";
    cout << "   negrita Texto en negrita sin comillas;\n\n";
    
    cout << "6. COMANDOS ESPECIALES:\n";
    cout << "   salto_linea();\n";
    cout << "   nueva_pagina();\n";
}

void probarAnalizadorLexico() {
    cout << "\n=== PROBADOR DE ANALISIS LEXICO ===\n";
    cout << "Ingresa codigo (escribe 'FIN' para terminar):\n\n";
    
    stringstream entrada;
    string linea;
    
    while (true) {
        cout << "> ";
        getline(cin, linea);
        
        if (linea == "FIN" || linea == "fin") {
            break;
        }
        
        entrada << linea << "\n";
    }
    
    try {
        AnalizadorLexico lexico(entrada.str());
        auto tokens = lexico.analizar();
        
        cout << "\n=== TOKENS RECONOCIDOS ===\n";
        for (const auto& token : tokens) {
            cout << "L" << token.linea << ":C" << token.columna 
                 << " -> " << token.toString() << endl;
        }
        
        cout << "\nTotal: " << tokens.size() << " tokens\n";
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void mostrarMenu() {
    cout << "\n========================================\n";
    cout << "  COMPILADOR LATEX EN ESPANOL v3.0\n";
    cout << "========================================\n";
    cout << "1. Compilar archivo existente (.texes)\n";
    cout << "2. Crear nuevo documento\n";
    cout << "3. Editor interactivo\n";
    cout << "4. Ver ejemplos de sintaxis\n";
    cout << "5. Probar analizador lexico\n";
    cout << "6. Salir\n";
    cout << "========================================\n";
    cout << "Opcion: ";
}

// ===============================
// FUNCION PRINCIPAL
// ===============================

int main() {
    cout << "\n";
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║  COMPILADOR LATEX EN ESPANOL v3.0     ║\n";
    cout << "║  Con analisis completo y generacion   ║\n";
    cout << "╚════════════════════════════════════════╝\n";
    
    int opcion;
    
    do {
        mostrarMenu();
        cin >> opcion;
        cin.ignore();
        
        switch (opcion) {
            case 1:
                compilarArchivo();
                break;
            case 2:
                crearNuevoDocumento();
                break;
            case 3:
                compilarInteractivo();
                break;
            case 4:
                mostrarEjemplos();
                break;
            case 5:
                probarAnalizadorLexico();
                break;
            case 6:
                cout << "\n¡Hasta luego!\n";
                break;
            default:
                cout << "\nOpcion no valida.\n";
        }
        
        if (opcion != 6) {
            cout << "\nPresiona Enter para continuar...";
            cin.get();
        }
        
    } while (opcion != 6);
    
    return 0;
}