#ifndef EXTENDIBLEHASHINGBUCKET_H
#define EXTENDIBLEHASHINGBUCKET_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstring>
#include <chrono>
#include <random>
#include <math.h>
#include <regex>

using namespace std;

constexpr size_t FB = 4;

template <typename Record>
struct Bucket
{
    int _localDepth;
    long _bits;
    int _size;
    pair<long, Record> _values[FB];
    long _next;

public:
    Bucket() = default;

    explicit Bucket(int localDepth, long bits) : _localDepth(localDepth), _bits(bits), _size(0), _next(-1) {}

    pair<bool, Record> search(long key)
    {
        for (long i = 0; i < _size; ++i)
        {
            if (_values[i].first == key)
            {
                if (_values[i].second.is_deleted)
                {
                    // Si está eliminado, devolver como no encontrado
                    return {false, Record{}};
                }
                return {true, _values[i].second};
            }
        }
        return {false, Record{}};
    }

    void remove(long key)
    {
        for (int i = 0; i < _size; ++i)
        {
            if (_values[i].first == key)
            {
                _values[i].second.is_deleted = true; // Marcar como eliminado
                return;
            }
        }
    }

    void print()
    {
        cout << "Sizeof: " << sizeof(Bucket) << endl;
        cout << "Local Depth: " << this->_localDepth << endl;
        cout << "Bits: " << this->_bits << endl;
        cout << "Size: " << this->_size << endl;

        cout << "NEXT: " << this->_next << endl;

        cout << "Values: " << endl;
        for (int i = 0; i < this->_size; i++)
        {
            cout << this->_values[i].first << " ";
        }
        cout << endl;
        cout << endl;
    }
};

#endif // EXTENDIBLEHASHINGBUCKET_H
