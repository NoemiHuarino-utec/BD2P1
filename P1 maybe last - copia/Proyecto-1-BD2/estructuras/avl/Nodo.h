#ifndef NODO_H
#define NODO_H

#include <cstring>

#include "../funciones.h"

using namespace std;

template <typename TK>
struct Nodo
{
    TK key{}; // Llave del nodo, en este caso es dni por el Record ya definido
    Record record;

    int pos;
    int left;
    int right;
    int height = 0;
    int next_del = -2;

    Nodo()
    {
        pos = -1;
        left = -1;
        right = -1;
        height = 0;
    }

    explicit Nodo(Record r)
    {
        pos = -1;
        left = -1;
        right = -1;
        height = 0;

        record = r;

        key = r.dni;
    }

    Nodo(const Nodo<TK> &other)
    {
        key = other.key;

        record = other.record;

        pos = other.pos;
        left = other.left;
        right = other.right;
        height = other.height;
        next_del = other.next_del;
    }

    void copyAttributes(Nodo other)
    {
        key = other.key;
        record = other.record;
    }

    void showData() const
    {
        cout << "Resultado del nodo de AVL:\n"
             << endl;
        cout << " key: " << key
             << " - pos: " << pos
             << " - left: " << left
             << " - right: " << right
             << " - height: " << height
             << " - next_del: " << next_del << endl;
        cout << "\nDetalles de la persona:" << endl;
        record.print();
    }
};

#endif // NODO_H
