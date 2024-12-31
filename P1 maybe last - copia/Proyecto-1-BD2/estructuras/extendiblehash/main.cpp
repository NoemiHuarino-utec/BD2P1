#include <iostream>
#include <fstream>
#include <string>
#include "../funciones.h"
#include "Extendible.h"

using namespace std;

int main()
{
    // Crear instancia de ExtendibleHashing
    ExtendibleHashing<long> hashTable("dni-person", 2);

    // Cargar e insertar datos desde el archivo CSV
    loadAndInsertData("../../data/SmallAdultDataset.csv", hashTable);

    // Realizar una búsqueda de prueba

    // Range search
    // Realizar búsqueda por rango en ingreso mensual
    /*
    long incomeMin = 3000;
    long incomeMax = 5000;

    cout << "Performing range search by income (" << incomeMin << " to " << incomeMax << "):" << endl;
    auto incomeRangeResults = hashTable.rangeSearchByIncome(incomeMin, incomeMax);

    for (const auto &record : incomeRangeResults)
    {
        record.print();
    }
    */

    // Remove a specific record
    // Eliminar un registro (opcional)
    long removeDNI = 59237933; // Cambiar a un DNI válido para eliminar
    hashTable.remove(removeDNI);
    cout << "Record with DNI " << removeDNI << " removed successfully (if it existed)." << endl;

    long searchDNI = 59237933; // Cambiar a un DNI válido del archivo
    auto result = hashTable.search(searchDNI);
    if (result.first)
    {
        cout << "Record found:" << endl;
        result.second.print();
    }
    else
    {
        cout << "Record not found for DNI: " << searchDNI << endl;
    }
    // Insert a new record
    Record newRecord;
    newRecord.dni = 99999999;
    strcpy(newRecord.nombre, "Nuevo Nombre");
    strcpy(newRecord.ocupacion, "Nueva Ocupacion");
    strcpy(newRecord.estado_civil, "Soltero");
    newRecord.ingreso_mensual = 5000;
    newRecord.edad = 30;
    strcpy(newRecord.sexo, "M");
    hashTable.insert(newRecord.dni, newRecord);

    // Verify insertion
    result = hashTable.search(newRecord.dni);
    if (result.first)
    {
        cout << "New record inserted successfully:" << endl;
        result.second.print();
    }
    else
    {
        cout << "Failed to insert new record" << endl;
    }

    return 0;
}