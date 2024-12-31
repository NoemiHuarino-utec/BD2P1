#ifndef RECORD_H
#define RECORD_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

struct Record
{
  long dni;                // Documento Nacional de Identidad
  char nombre[200];        // Nombre completo
  char ocupacion[200];     // Ocupación
  char estado_civil[200];  // Estado civil
  long ingreso_mensual;    // Ingreso mensual
  int edad;                // Edad
  char sexo[2];            // Sexo (M/F)
  bool is_deleted = false; // Indicador de registro eliminado
  long next;               // Siguiente registro en la lista - hash

  Record() = default;

  // Leer una línea del archivo CSV
  void read(string fila);

  // Vaciar registro
  void empty_record();

  // Imprimir datos del registro
  void print() const;

  // Convertir a string
  string toString() const;
};

#endif // RECORD_H
