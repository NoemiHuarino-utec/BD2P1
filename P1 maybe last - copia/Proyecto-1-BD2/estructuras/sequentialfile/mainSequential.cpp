#include <iostream>
#include <fstream>
#include <sstream>
#include "SequentialFile.h"

using namespace std;

int main()
{
    SequentialFile sequentialFile("main.dat", "aux.dat");
    std::string csvFilePath = "../../data/SmallAdultDataset.csv";

    // Cargar los datos desde el CSV
    loadCSVDataSeq(csvFilePath, sequentialFile);
    // Mostrar el número total de elementos insertados

    // Prueba de búsqueda
    cout << "\n||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
    cout << "\nPrueba de Busqueda: " << endl;
    long searchKey = 51118137; // DNI a buscar
    try
    {
        Record foundRecord = sequentialFile.search(searchKey);
        cout << "Registro encontrado: " << foundRecord.dni << endl;
        cout << "Datos del registro: " << endl;
        foundRecord.print();
    }
    catch (const exception &e)
    {
        cerr << "No se encontró el registro con el DNI: " << searchKey << endl;
    }

    // Prueba de eliminación
    cout << "\nPrueba de Eliminacion: " << endl;
    long deleteKey = 35549204; // DNI a eliminar
    try
    {
        sequentialFile.remove(deleteKey);
        cout << "Registro con DNI " << deleteKey << " eliminado con éxito." << endl;
    }
    catch (const exception &e)
    {
        cerr << "No se pudo eliminar el registro con DNI: " << deleteKey << endl;
    }

    // Pruebas de búsqueda por rango
    cout << "\n||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
    cout << "\nPrueba de Busqueda por Rango:\n " << endl;
    long startSueldo = 3000, endSueldo = 4000; // Rango de ingreso_mensual
    auto results = sequentialFile.rangeSearch(startSueldo, endSueldo);
    for (const auto &record : results)
    {
        cout << "Registro encontrado en el rango: " << record.ingreso_mensual << endl;
        cout << "Datos del registro: " << endl;
        record.print();
    }

    return 0;
}
