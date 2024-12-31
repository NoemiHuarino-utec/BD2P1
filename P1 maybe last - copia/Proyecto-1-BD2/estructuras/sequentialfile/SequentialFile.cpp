#include "SequentialFile.h"
#include <unordered_set>


SequentialFile::SequentialFile(const std::string &mainFile, const std::string &auxFile)
    : mainFile(mainFile), auxFile(auxFile) {
    auxMaxSize = static_cast<int>(std::log2(loadRecords(mainFile).size() + 1));
}

std::vector<Record> SequentialFile::loadRecords(const std::string &file) {
    std::vector<Record> records;
    std::ifstream inFile(file, std::ios::binary);
    if (inFile.is_open()) {
        Record record;
        while (inFile.read(reinterpret_cast<char *>(&record), sizeof(Record))) {
            if (!record.is_deleted) {
                records.push_back(record);
            }
        }
        inFile.close();
    }
    return records;
}

void SequentialFile::saveRecords(const std::string &file, const std::vector<Record> &records) {
    std::ofstream outFile(file, std::ios::binary | std::ios::trunc);
    for (const auto &record : records) {
        outFile.write(reinterpret_cast<const char *>(&record), sizeof(Record));
    }
    outFile.close();
}

void SequentialFile::appendRecord(const std::string &file, const Record &record) {
    std::ofstream outFile(file, std::ios::binary | std::ios::app);
    outFile.write(reinterpret_cast<const char *>(&record), sizeof(Record));
    outFile.close();
}

bool SequentialFile::add(const Record &record) {
    // Cargar registros existentes
    auto auxRecords = loadRecords(auxFile);

    // Verificar si el registro ya existe en el archivo auxiliar
    for (const auto &existingRecord : auxRecords) {
        if (existingRecord.dni == record.dni) {
            return false;  // Registro duplicado, no agregar
        }
    }

    // Si no está duplicado, agregarlo
    appendRecord(auxFile, record);
    auxRecords = loadRecords(auxFile);

    // Si el archivo auxiliar alcanza el tamaño máximo, realizar el merge
    if (auxRecords.size() >= auxMaxSize) {
        merge();
    }

    return true;
}


Record SequentialFile::search(long dni) {
    auto mainRecords = loadRecords(mainFile);
    auto auxRecords = loadRecords(auxFile);

    auto it = std::lower_bound(mainRecords.begin(), mainRecords.end(), dni, [](const Record &a, long key) {
        return a.dni < key;
    });

    if (it != mainRecords.end() && it->dni == dni) {
        return *it;
    }

    for (const auto &record : auxRecords) {
        if (record.dni == dni) {
            return record;
        }
    }

    throw std::runtime_error("Record not found");
}

/*
std::vector<Record> SequentialFile::rangeSearch(long startSueldo, long endSueldo) {
    auto mainRecords = loadRecords(mainFile);
    auto auxRecords = loadRecords(auxFile);

    std::vector<Record> result;

    for (const auto &record : mainRecords) {
        if (record.ingreso_mensual >= startSueldo && record.ingreso_mensual <= endSueldo) {
            result.push_back(record);
        }
    }

    for (const auto &record : auxRecords) {
        if (record.ingreso_mensual >= startSueldo && record.ingreso_mensual <= endSueldo) {
            result.push_back(record);
        }
    }

    return result;
}*/


std::vector<Record> SequentialFile::rangeSearch(long startSueldo, long endSueldo) {
    auto mainRecords = loadRecords(mainFile);
    auto auxRecords = loadRecords(auxFile);

    std::vector<Record> result;
    std::unordered_set<long> seenDni;  // Para evitar duplicados, usaremos un set de DNI

    // Buscar en mainFile
    for (const auto &record : mainRecords) {
        if (!record.is_deleted && record.ingreso_mensual >= startSueldo && record.ingreso_mensual <= endSueldo) {
            // Verificar si el registro ya ha sido agregado
            if (seenDni.find(record.dni) == seenDni.end()) {
                result.push_back(record);
                seenDni.insert(record.dni);  // Marcamos el DNI como visto
            }
        }
    }

    // Buscar en auxFile
    for (const auto &record : auxRecords) {
        if (!record.is_deleted && record.ingreso_mensual >= startSueldo && record.ingreso_mensual <= endSueldo) {
            // Verificar si el registro ya ha sido agregado
            if (seenDni.find(record.dni) == seenDni.end()) {
                result.push_back(record);
                seenDni.insert(record.dni);  // Marcamos el DNI como visto
            }
        }
    }

    return result;
}



void SequentialFile::remove(long dni) {
    auto mainRecords = loadRecords(mainFile);
    auto auxRecords = loadRecords(auxFile);

    bool found = false;
    for (auto &record : mainRecords) {
        if (record.dni == dni) {
            record.is_deleted = true;
            found = true;
            break;
        }
    }

    for (auto &record : auxRecords) {
        if (record.dni == dni) {
            record.is_deleted = true;
            found = true;
            break;
        }
    }

    if (!found) {
        throw std::runtime_error("Record not found");
    }

    saveRecords(mainFile, mainRecords);
    saveRecords(auxFile, auxRecords);
}

void SequentialFile::merge() {
    auto mainRecords = loadRecords(mainFile);
    auto auxRecords = loadRecords(auxFile);

    std::sort(auxRecords.begin(), auxRecords.end(), [](const Record &a, const Record &b) {
        return a.dni < b.dni;
    });

    std::vector<Record> mergedRecords;
    std::merge(mainRecords.begin(), mainRecords.end(), auxRecords.begin(), auxRecords.end(), std::back_inserter(mergedRecords), [](const Record &a, const Record &b) {
        return a.dni < b.dni;
    });

    saveRecords(mainFile, mergedRecords);

    std::ofstream outFile(auxFile, std::ios::binary | std::ios::trunc);
    outFile.close();
}