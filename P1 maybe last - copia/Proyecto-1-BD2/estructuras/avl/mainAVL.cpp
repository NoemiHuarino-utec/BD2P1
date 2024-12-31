#include <iostream>
#include <fstream>
#include <sstream>
#include "AVLFile.h"
#include "Nodo.h"
using namespace std;

int main()
{
    // Archivo CSV con los datos
    AVLFile<long> avlTree("avl_tree.dat");
    avlTree.loadData("../../data/SmallAdultDataset.csv");

    // Pruebas de inserción
    cout << "Pruebas de Inserción: " << endl;
    for (const auto &node : avlTree.seekAll())
    {
        node.showData();
    }

    // Prueba de eliminación
    cout << "||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
    cout << "\nPrueba de Eliminación: " << endl;
    long deleteKey = 96165; // dni a eliminar
    if (avlTree.remove(deleteKey))
    {
        cout << "Nodo eliminado con éxito." << endl;
    }
    else
    {
        cerr << "No se pudo eliminar el nodo con clave: " << deleteKey << endl;
    }

    // Pruebas de búsqueda por rango
    cout << "\n||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
    cout << "\nPrueba de Búsqueda por Rango:\n " << endl;
    auto results = avlTree.rangeSearch(3000, 4000); // Rango de sueldos entre 5000 y 10000
    for (const auto &nodo : results)
    {
        nodo.showData();
    }

    // Prueba de búsqueda
    cout << "||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
    cout << "\nPrueba de Búsqueda: " << endl;
    long searchKey = 96165; // DNI a buscar

    auto foundNodePtr = avlTree.search(searchKey);
    if (!foundNodePtr)
    {
        cerr << "No se encontró el nodo con la clave: " << searchKey << endl;
    }
    else
    {
        foundNodePtr->showData();
    }

    return 0;
}