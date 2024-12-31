#ifndef SEQUENTIALFILE_H
#define SEQUENTIALFILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include "../funciones.h"

class SequentialFile
{
private:
    std::string mainFile;
    std::string auxFile;
    int auxMaxSize;

    std::vector<Record> loadRecords(const std::string &file)
    {
        std::vector<Record> records;
        std::ifstream inFile(file, std::ios::binary);
        if (inFile.is_open())
        {
            Record record;
            while (inFile.read(reinterpret_cast<char *>(&record), sizeof(Record)))
            {
                if (!record.is_deleted)
                {
                    records.push_back(record);
                }
            }
            inFile.close();
        }
        return records;
    }

    void saveRecords(const std::string &file, const std::vector<Record> &records)
    {
        std::ofstream outFile(file, std::ios::binary | std::ios::trunc);
        for (const auto &record : records)
        {
            outFile.write(reinterpret_cast<const char *>(&record), sizeof(Record));
        }
        outFile.close();
    }

    void appendRecord(const std::string &file, const Record &record)
    {
        std::ofstream outFile(file, std::ios::binary | std::ios::app);
        outFile.write(reinterpret_cast<const char *>(&record), sizeof(Record));
        outFile.close();
    }

public:
    SequentialFile(const std::string &mainFile, const std::string &auxFile)
        : mainFile(mainFile), auxFile(auxFile)
    {
        auxMaxSize = static_cast<int>(std::log2(loadRecords(mainFile).size() + 1));
    }

    void loadData(const std::string &csvFile)
    {
        std::ifstream file(csvFile);

        if (!file.is_open())
        {
            throw std::runtime_error("Error al abrir el archivo " + csvFile);
        }

        std::string line;
        bool isHeader = true;

        while (std::getline(file, line))
        {
            if (isHeader)
            {
                isHeader = false; // Ignorar encabezado
                continue;
            }

            Record record;
            record.read(line); // Asume que Record tiene un método `read`

            if (!this->add(record))
            {
                std::cerr << "Registro duplicado (no agregado): " << record.dni << std::endl;
            }
        }

        file.close();
    }

    bool add(const Record &record)
    {
        auto auxRecords = loadRecords(auxFile);

        for (const auto &existingRecord : auxRecords)
        {
            if (existingRecord.dni == record.dni)
            {
                return false;
            }
        }

        appendRecord(auxFile, record);
        auxRecords = loadRecords(auxFile);

        if (auxRecords.size() >= auxMaxSize)
        {
            merge();
        }

        return true;
    }

    Record search(long dni)
    {
        auto mainRecords = loadRecords(mainFile);
        auto auxRecords = loadRecords(auxFile);

        auto it = std::lower_bound(mainRecords.begin(), mainRecords.end(), dni, [](const Record &a, long key)
                                   { return a.dni < key; });

        if (it != mainRecords.end() && it->dni == dni)
        {
            return *it;
        }

        for (const auto &record : auxRecords)
        {
            if (record.dni == dni)
            {
                return record;
            }
        }

        throw std::runtime_error("Record not found");
    }

    std::vector<Record> rangeSearch(long startSueldo, long endSueldo)
    {
        auto mainRecords = loadRecords(mainFile);
        auto auxRecords = loadRecords(auxFile);

        std::vector<Record> result;
        std::unordered_set<long> seenDni;

        for (const auto &record : mainRecords)
        {
            if (!record.is_deleted && record.ingreso_mensual >= startSueldo && record.ingreso_mensual <= endSueldo)
            {
                if (seenDni.find(record.dni) == seenDni.end())
                {
                    result.push_back(record);
                    seenDni.insert(record.dni);
                }
            }
        }

        for (const auto &record : auxRecords)
        {
            if (!record.is_deleted && record.ingreso_mensual >= startSueldo && record.ingreso_mensual <= endSueldo)
            {
                if (seenDni.find(record.dni) == seenDni.end())
                {
                    result.push_back(record);
                    seenDni.insert(record.dni);
                }
            }
        }

        return result;
    }

    void remove(long dni)
    {
        auto mainRecords = loadRecords(mainFile);
        auto auxRecords = loadRecords(auxFile);

        bool found = false;
        for (auto &record : mainRecords)
        {
            if (record.dni == dni)
            {
                record.is_deleted = true;
                found = true;
                break;
            }
        }

        for (auto &record : auxRecords)
        {
            if (record.dni == dni)
            {
                record.is_deleted = true;
                found = true;
                break;
            }
        }

        if (!found)
        {
            throw std::runtime_error("Record not found");
        }

        saveRecords(mainFile, mainRecords);
        saveRecords(auxFile, auxRecords);
    }

    void merge()
    {
        auto mainRecords = loadRecords(mainFile);
        auto auxRecords = loadRecords(auxFile);

        std::sort(auxRecords.begin(), auxRecords.end(), [](const Record &a, const Record &b)
                  { return a.dni < b.dni; });

        std::vector<Record> mergedRecords;
        std::merge(mainRecords.begin(), mainRecords.end(), auxRecords.begin(), auxRecords.end(), std::back_inserter(mergedRecords), [](const Record &a, const Record &b)
                   { return a.dni < b.dni; });

        saveRecords(mainFile, mergedRecords);

        std::ofstream outFile(auxFile, std::ios::binary | std::ios::trunc);
        outFile.close();
    }

    std::vector<Record> seekAll()
    {
        return loadRecords(mainFile);
    }
};

void loadCSVDataSeq(const std::string &csvFilePath, SequentialFile &sequentialFile)
{
    std::ifstream file(csvFilePath);

    if (!file.is_open())
    {
        std::cerr << "Error al abrir el archivo " << csvFilePath << std::endl;
        return;
    }

    std::string line;
    bool isHeader = true;

    while (std::getline(file, line))
    {
        if (isHeader)
        {
            isHeader = false;
            continue;
        }

        Record record;
        record.read(line);

        if (sequentialFile.add(record))
        {
            std::cout << "Registro agregado: " << record.dni << std::endl;
        }
        else
        {
            std::cout << "Registro duplicado (no agregado): " << record.dni << std::endl;
        }
    }

    file.close();
}

#endif // SEQUENTIALFILE_H