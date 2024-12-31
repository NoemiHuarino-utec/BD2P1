#ifndef FUNCIONES_H
#define FUNCIONES_H

#include "Record.h"

// Implementación de los métodos
void Record::read(string fila)
{
    // Limpiar datos
    strcpy(this->nombre, "\0");
    strcpy(this->ocupacion, "\0");
    strcpy(this->estado_civil, "\0");
    strcpy(this->sexo, "\0");

    int col = 0;
    string data = "";

    for (const auto &c : fila)
    {
        if (c == ',' || &c == &fila.back()) // Separador de columnas o último carácter
        {
            if (&c == &fila.back()) // Agregar último carácter al dato si es el final
                data += c;

            try
            {
                switch (col)
                {
                case 0:
                    this->dni = stol(data);
                    break;
                case 1:
                    strcpy(this->nombre, data.c_str());
                    break;
                case 2:
                    strcpy(this->ocupacion, data.c_str());
                    break;
                case 3:
                    strcpy(this->estado_civil, data.c_str());
                    break;
                case 4:
                    this->ingreso_mensual = stol(data);
                    break;
                case 5:
                    this->edad = stoi(data);
                    break;
                case 6:
                    strcpy(this->sexo, data.c_str());
                    break;
                }
            }
            catch (const invalid_argument &e)
            {
                cerr << "Invalid argument: " << data << " in column " << col << endl;
                throw;
            }
            catch (const out_of_range &e)
            {
                cerr << "Out of range: " << data << " in column " << col << endl;
                throw;
            }

            data = "";
            col++;
        }
        else
        {
            data += c;
        }
    }
}

void Record::empty_record()
{
    this->dni = -1; // Valor indicador de que está vacío
    this->next = -1;
}

void Record::print() const
{
    cout << endl;
    cout << "DNI: " << this->dni << endl;
    cout << "Nombre: " << this->nombre << endl;
    cout << "Ocupación: " << this->ocupacion << endl;
    cout << "Estado civil: " << this->estado_civil << endl;
    cout << "Ingreso mensual: " << this->ingreso_mensual << endl;
    cout << "Edad: " << this->edad << endl;
    cout << "Sexo: " << this->sexo << endl;
    cout << "............................................................" << endl;
    cout << endl;
}

string Record::toString() const
{
    std::stringstream ss;
    ss << dni << ", " << this->nombre << ", " << this->ocupacion << ", " << this->estado_civil
       << ", " << this->ingreso_mensual << ", " << this->edad << ", " << this->sexo;
    return ss.str();
}

#endif // FUNCIONES_H