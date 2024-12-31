#include <iostream>
#include "parserSQL.h"
#include "../estructuras/avl/AVLFile.h"
#include "../estructuras/extendiblehash/Extendible.h"
#include "../estructuras/sequentialfile/SequentialFile.h"

using namespace std;

vector<Table> tables;

// Función para guardar tablas en un archivo CSV
void saveTablesToCSV(const string &filePath)
{
    ofstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error: No se pudo crear o abrir el archivo " << filePath << " para escribir." << endl;
        return;
    }

    for (const auto &table : tables)
    {
        file << table.index << "," << table.name << "," << table.file << endl;
    }

    file.close();
    cout << "Tablas guardadas en " << filePath << endl;
}

// Función para cargar tablas desde un archivo CSV
void loadTablesFromCSV(const string &filePath)
{
    ifstream file(filePath);
    if (!file.is_open())
    {
        cout << "No se encontró el archivo " << filePath << ". Se iniciará sin tablas cargadas." << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string index, name, fileData;
        if (getline(ss, index, ',') && getline(ss, name, ',') && getline(ss, fileData))
        {
            Table table = {name, fileData, index};
            tables.push_back(table);
        }
    }

    file.close();
    cout << "Tablas cargadas desde " << filePath << endl;
}

int main()
{
    const string csvFilePath = "D:/bd2/P1 maybe last - copia/Proyecto-1-BD2/parser/tables.csv";

    // Cargar tablas desde el archivo CSV al inicio
    loadTablesFromCSV(csvFilePath);

    AVLFile<long> avlFile("D:/bd2/P1 maybe last - copia/Proyecto-1-BD2/estructuras/avl/avl_tree.dat");
    ExtendibleHashing<long> extendibleHashing("D:/bd2/P1 maybe last - copia/Proyecto-1-BD2/estructuras/extendiblehash/dni-person");
    SequentialFile sequentialFile("D:/bd2/P1 maybe last - copia/Proyecto-1-BD2/estructuras/sequentialfile/main.dat",
                                  "D:/bd2/P1 maybe last - copia/Proyecto-1-BD2/estructuras/sequentialfile/aux.dat");

    while (true)
    {
        cout << "Ingrese una consulta SQL (o 'exit' para salir):" << endl;
        string input;
        getline(cin, input);

        if (input == "exit")
        {
            // Guardar tablas en el archivo CSV antes de salir
            saveTablesToCSV(csvFilePath);
            cout << "Saliendo del programa..." << endl;
            break;
        }

        Scanner scanner(input.c_str());
        Parser parser(&scanner, &avlFile, &extendibleHashing, &sequentialFile, tables);

        try
        {
            parser.parse();
            cout << "Consulta procesada correctamente." << endl;
        }
        catch (const exception &e)
        {
            cerr << "Error al procesar la consulta: " << e.what() << endl;
        }
    }

    return 0;
}
