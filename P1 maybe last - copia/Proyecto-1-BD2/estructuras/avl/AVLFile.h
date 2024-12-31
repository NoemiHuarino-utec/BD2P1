#ifndef AVLFILE_H
#define AVLFILE_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <utility>

#include "Nodo.h"

using namespace std;

template <typename TK>
class AVLFile
{
private:
    std::streamsize ROOT_SIZE = sizeof(int);
    std::streamsize FDR_SIZE = sizeof(int);
    std::streamsize KEY_ATT_SIZE = sizeof(char[20]);
    std::streamsize HEADER_SIZE = ROOT_SIZE + FDR_SIZE + KEY_ATT_SIZE;
    int NULL_POS = -1;
    int NOT_FL = -2;

private:
    // FILE
    string file_name;
    fstream file;

    // HEADER
    int root;
    int first_deleted_record; // -1: NULL_POS: end of list, -2: NOT_FL : not in FL, other_positive_number: in FL

    // FOR BALANCE
    int parent_last_rotated_node1 = NULL_POS;
    int parent_last_rotated_node2 = NULL_POS;

public:
    explicit AVLFile(std::string file_name) : file_name(file_name)
    {
        ifstream in_file(this->file_name, ios::binary);

        // Empty file or it doesn't exists
        if (!in_file.is_open() | fileEmpty(in_file))
        {
            ofstream out_file(this->file_name, ios::binary);
            root = NULL_POS;
            first_deleted_record = NULL_POS;
            writeFileHeader(out_file);
            out_file.close();
        }
        else
        {
            std::tie(root, first_deleted_record) = readFileHeader(in_file);
        }

        in_file.close();
    }

    ~AVLFile() = default;


    bool fileEmpty(ifstream &file)
    {
        return file.peek() == ifstream::traits_type::eof();
    }

    void add(Nodo<TK> record)
    {
        file.open(file_name, ios::binary | ios::in | ios::out);

        Nodo<TK> parent_record;
        bool x = add(root, parent_record, record);
        writeFileRoot();

        file.close();
    }

    std::shared_ptr<Nodo<TK>> search(TK key)
    {
        file.open(file_name, ios::binary | ios::in | ios::out);

        auto result = search(root, key);

        file.close();

        return result; // Devuelve el puntero al nodo encontrado o nullptr
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
                isHeader = false;
                continue;
            }

            Record record;
            record.read(line);

            Nodo<TK> node(record);
            this->add(node);
        }

        file.close();
    }

    int height()
    {
        return height(root);
    }

    TK minValue()
    {
        file.open(file_name, ios::binary | ios::in | ios::out);

        TK min_value = minValue(root);

        file.close();

        return min_value;
    }

    TK maxValue()
    {
        file.open(file_name, ios::binary | ios::in | ios::out);

        TK max_value = maxValue(root);

        file.close();

        return max_value;
    }

    bool isBalanced()
    {
        return isBalanced(root);
    }

    int size()
    {
        file.open(file_name, ios::binary | ios::in);

        int sz = size(root);

        file.close();

        return sz;
    }

    std::vector<Nodo<TK>> rangeSearch(int begin_key, int end_key)
    {
        vector<Nodo<TK>> records;

        rangeSearch(root, records, begin_key, end_key);

        return records;
    }

    bool remove(TK key)
    {
        file.open(file_name, ios::binary | ios::in | ios::out);

        Nodo<TK> parent_record;
        bool removed = remove(root, parent_record, key);
        writeFileRoot();

        file.close();

        return removed;
    }

    std::vector<Nodo<TK>> seekAll()
    {
        vector<Nodo<TK>> records;

        seekAll(root, records);

        return records;
    }

    std::vector<Nodo<TK>> seekFreeList()
    {
        vector<Nodo<TK>> records;

        seekFreeList(first_deleted_record, records);

        return records;
    }

    int getRootPos()
    {
        return root;
    }

    Nodo<TK> getRoot()
    {
        return getRecordByPos(root);
    }

private:
    // FILE METHODS
    void writeFileRoot()
    {
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<char *>(&root), ROOT_SIZE);
    }

    void writeFileHeader(std::fstream &out_file)
    {
        // ROOT
        out_file.seekp(0, ios::beg);
        out_file.write(reinterpret_cast<char *>(&root), ROOT_SIZE);

        // FIRST_DELETED_RECORD
        out_file.seekp(ROOT_SIZE, ios::beg);
        out_file.write(reinterpret_cast<char *>(&first_deleted_record), FDR_SIZE);
    }

    void writeFileHeader(std::ofstream &out_file)
    {
        // ROOT
        out_file.seekp(0, ios::beg);
        out_file.write(reinterpret_cast<char *>(&root), ROOT_SIZE);

        // FIRST_DELETED_RECORD
        out_file.seekp(ROOT_SIZE, ios::beg);
        out_file.write(reinterpret_cast<char *>(&first_deleted_record), FDR_SIZE);
    }

    // pair<root, first_deleted_record>
    std::pair<int, int> readFileHeader(std::ifstream &in_file)
    {
        // ROOT
        int file_root;
        in_file.seekg(0, ios::beg);
        in_file.read(reinterpret_cast<char *>(&file_root), ROOT_SIZE);

        // FIRST_DELETED_RECORD
        int file_fdr;
        in_file.seekg(ROOT_SIZE, ios::beg);
        in_file.read(reinterpret_cast<char *>(&file_fdr), FDR_SIZE);

        return make_pair(file_root, file_fdr);
    }

    Nodo<TK> getRecordByPos(int pos)
    {
        if (pos == NULL_POS)
        {
            return Nodo<TK>{};
        }

        fstream get_file(file_name, ios::binary | ios::in);

        Nodo<TK> record;

        get_file.seekg(HEADER_SIZE + pos * sizeof(Nodo<TK>), ios::beg);
        get_file.read(reinterpret_cast<char *>(&record), sizeof(Nodo<TK>));

        get_file.close();

        return record;
    }

    void writeRecord(Nodo<TK> record)
    {
        fstream write_file(file_name, ios::binary | ios::in | ios::out);

        write_file.seekp(HEADER_SIZE + record.pos * sizeof(Nodo<TK>), ios::beg);
        write_file.write(reinterpret_cast<char *>(&record), sizeof(Nodo<TK>));

        write_file.close();
    }

    // AVL-FILE STRUCTURE METHODS
    bool add(int &pos, Nodo<TK> parent_current, Nodo<TK> &record)
    {
        if (pos == NULL_POS)
        {
            if (first_deleted_record != NULL_POS)
            {
                Nodo<TK> deleted_record = getRecordByPos(first_deleted_record);
                first_deleted_record = deleted_record.next_del;

                pos = deleted_record.pos;

                fstream out_file(file_name, ios::binary | ios::in | ios::out);
                writeFileHeader(out_file);
                out_file.close();
            }
            else
            {
                file.seekg(0, ios::end);
                pos = ((int)file.tellg() - (int)HEADER_SIZE) / (int)sizeof(Nodo<TK>);
            }

            record.pos = pos;
            writeRecord(record);

            return true;
        }
        else
        {
            Nodo<TK> current;
            file.seekg(HEADER_SIZE + pos * sizeof(Nodo<TK>), ios::beg);
            file.read(reinterpret_cast<char *>(&current), sizeof(Nodo<TK>));

            bool added = false;

            if (record.key < current.key)
            {
                added = add(current.left, current, record);
            }
            else if (record.key > current.key)
            {
                added = add(current.right, current, record);
            }

            if (!added)
            {
                return false;
            }

            updateHeight(current);
            balance(current, parent_current);

            // Check if record has been rotated and don't write to avoid overwrite smth
            if (parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos)
            {
                writeRecord(current);
            }
            else
            {
                if (parent_last_rotated_node1 != NULL_POS && parent_last_rotated_node1 == current.pos)
                {
                    parent_last_rotated_node1 = NULL_POS;
                }
                else if (parent_last_rotated_node2 != NULL_POS && parent_last_rotated_node2 == current.pos)
                {
                    parent_last_rotated_node2 = NULL_POS;
                }
            }
        }
        return true;
    }

    int height(int pos)
    {
        if (pos == NULL_POS)
        {
            return NULL_POS;
        }

        Nodo<TK> record = getRecordByPos(pos);

        if (record.pos == -1)
        {
            return -1;
        }
        return record.height;
    }

    void updateHeight(Nodo<TK> &record)
    {
        if (record.next_del != NOT_FL)
        {
            return;
        }
        record.height = max(height(record.left), height(record.right)) + 1;
    }

    int balancingFactor(Nodo<TK> record)
    {
        int l_h = height(record.left);
        int r_h = height(record.right);

        return l_h - r_h;
    }

    void balance(Nodo<TK> &record, Nodo<TK> &parent_record)
    {
        if (record.next_del != -2)
        {
            return;
        }

        // Left subtree is higher. Right rotation required.
        if (balancingFactor(record) >= 2)
        {
            // If child's right subtree is higher, rotate such node to the left.
            if (balancingFactor(getRecordByPos(record.left)) <= NULL_POS)
            {
                Nodo<TK> left_record = getRecordByPos(record.left);
                parent_last_rotated_node1 = left_rota(left_record, record);
            }
            parent_last_rotated_node2 = right_rota(record, parent_record);
        }
        else if (balancingFactor(record) <= -2)
        {
            // If child's left subtree is higher, rotate such node to the right.
            if (balancingFactor(getRecordByPos(record.right)) >= 1)
            {
                Nodo<TK> right_record = getRecordByPos(record.right);
                parent_last_rotated_node1 = right_rota(right_record, record);
            }
            parent_last_rotated_node2 = left_rota(record, parent_record);
        }
    }

    int left_rota(Nodo<TK> &record, Nodo<TK> &parent_record)
    {
        if (record.right != NULL_POS)
        {
            Nodo<TK> right_record = getRecordByPos(record.right);

            record.right = right_record.left;
            right_record.left = record.pos;

            writeRecord(record);
            writeRecord(right_record);

            updateHeight(record);
            writeRecord(record);

            updateHeight(right_record);
            writeRecord(right_record);

            writeRecord(record);
            writeRecord(right_record);

            if (parent_record.pos != record.pos && parent_record.pos != right_record.pos)
            {
                if (parent_record.left == record.pos)
                {
                    parent_record.left = right_record.pos;
                }
                else if (parent_record.right == record.pos)
                {
                    parent_record.right = right_record.pos;
                }

                writeRecord(parent_record);
            }

            if (record.pos == root)
            {
                root = right_record.pos;
            }

            return parent_record.pos;
        }

        return NULL_POS;
    }

    int right_rota(Nodo<TK> &record, Nodo<TK> &parent_record)
    {
        if (record.left != NULL_POS)
        {
            Nodo<TK> left_record = getRecordByPos(record.left);

            record.left = left_record.right;
            left_record.right = record.pos;

            writeRecord(record);
            writeRecord(left_record);

            updateHeight(record);
            writeRecord(record);

            updateHeight(left_record);
            writeRecord(left_record);

            writeRecord(record);
            writeRecord(left_record);

            if (parent_record.pos != record.pos && parent_record.pos != left_record.pos && parent_record.pos != NULL_POS)
            {
                if (parent_record.left == record.pos)
                {
                    parent_record.left = left_record.pos;
                }
                else if (parent_record.right == record.pos)
                {
                    parent_record.right = left_record.pos;
                }

                writeRecord(parent_record);
            }

            if (record.pos == root)
            {
                root = left_record.pos;
            }

            return parent_record.pos;
        }

        return NULL_POS;
    }

    std::shared_ptr<Nodo<TK>> search(int pos, TK key)
    {
        if (pos == NULL_POS)
        {
            return nullptr; // Nodo no encontrado
        }

        auto current = std::make_shared<Nodo<TK>>();
        file.seekg(HEADER_SIZE + pos * sizeof(Nodo<TK>), ios::beg);
        file.read(reinterpret_cast<char *>(&(*current)), sizeof(Nodo<TK>));

        if (current->next_del != -2)
        {
            return nullptr; // Ignorar nodos eliminados
        }

        if (key < current->key)
        {
            return search(current->left, key);
        }
        else if (key > current->key)
        {
            return search(current->right, key);
        }
        else
        {
            return current; // Nodo encontrado
        }
    }

    TK minValue(int pos)
    {
        if (pos == NULL_POS)
        {
            throw invalid_argument("The tree is empty");
        }

        Nodo<TK> current;
        file.seekg(HEADER_SIZE + pos * sizeof(Nodo<TK>), ios::beg);
        file.read(reinterpret_cast<char *>(&current), sizeof(Nodo<TK>));

        if (current.left == NULL_POS)
        {
            return current.key;
        }
        else
        {
            return minValue(current.left);
        }
    }

    TK maxValue(int pos)
    {
        if (pos == NULL_POS)
        {
            throw invalid_argument("The tree is empty");
        }

        Nodo<TK> current;
        file.seekg(HEADER_SIZE + pos * sizeof(Nodo<TK>), ios::beg);
        file.read(reinterpret_cast<char *>(&current), sizeof(Nodo<TK>));

        if (current.right == NULL_POS)
        {
            return current.key;
        }
        else
        {
            return maxValue(current.right);
        }
    }

    Nodo<TK> maxRecord(int pos)
    {
        if (pos == NULL_POS)
        {
            throw invalid_argument("The tree is empty");
        }

        Nodo<TK> current;
        file.seekg(HEADER_SIZE + pos * sizeof(Nodo<TK>), ios::beg);
        file.read(reinterpret_cast<char *>(&current), sizeof(Nodo<TK>));

        if (current.right == NULL_POS)
        {
            return current;
        }
        else
        {
            return maxRecord(current.right);
        }
    }

    bool isBalanced(int pos)
    {
        Nodo<TK> record = getRecordByPos(pos);

        if (pos == NULL_POS || (balancingFactor(record) >= NULL_POS && balancingFactor(record) <= 1) && pos != -1)
        {
            return true;
        }
        return false;
    }

    int size(int pos)
    {
        if (pos == NULL_POS)
        {
            return 0;
        }
        else
        {
            Nodo<TK> current;
            file.seekg(HEADER_SIZE + pos * sizeof(Nodo<TK>), ios::beg);
            file.read(reinterpret_cast<char *>(&current), sizeof(Nodo<TK>));

            return 1 + size(current.left) + size(current.right);
        }
    }

    void rangeSearch(int pos, std::vector<Nodo<TK>> &records, int &begin_key, int &end_key)
    {
        if (pos == NULL_POS)
        {
            return;
        }

        Nodo<TK> current = getRecordByPos(pos);

        rangeSearch(current.left, records, begin_key, end_key);

        if (begin_key <= current.record.ingreso_mensual && current.record.ingreso_mensual <= end_key)
        {
            records.push_back(current);
        }

        rangeSearch(current.right, records, begin_key, end_key);
    }

    bool remove(int pos, Nodo<TK> &parent_current, TK key)
    {
        if (pos == NULL_POS)
        {
            return false;
        }

        Nodo<TK> current;
        file.seekg(HEADER_SIZE + pos * sizeof(Nodo<TK>), ios::beg);
        file.read(reinterpret_cast<char *>(&current), sizeof(Nodo<TK>));

        bool removed = false;
        if (key < current.key)
        {
            removed = remove(current.left, current, key);

            updateHeight(current);
            balance(current, parent_current);

            // Check if record has been rotated and don't write to avoid overwrite smth
            // Also check if its child hash been removed
            if (!removed && parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos)
            {
                writeRecord(current);
            }
            else
            {
                if (parent_last_rotated_node1 != NULL_POS && parent_last_rotated_node1 == current.pos)
                {
                    parent_last_rotated_node1 = NULL_POS;
                }
                else if (parent_last_rotated_node2 != NULL_POS && parent_last_rotated_node2 == current.pos)
                {
                    parent_last_rotated_node2 = NULL_POS;
                }
            }
        }
        else if (key > current.key)
        {
            removed = remove(current.right, current, key);
            updateHeight(current);
            balance(current, parent_current);

            // Check if record has been rotated and don't write to avoid overwrite smth
            if (!removed && parent_last_rotated_node1 != current.pos && parent_last_rotated_node2 != current.pos)
            {
                writeRecord(current);
            }
            else
            {
                if (parent_last_rotated_node1 != NULL_POS && parent_last_rotated_node1 == current.pos)
                {
                    parent_last_rotated_node1 = NULL_POS;
                }
                else if (parent_last_rotated_node2 != NULL_POS && parent_last_rotated_node2 == current.pos)
                {
                    parent_last_rotated_node2 = NULL_POS;
                }
            }
        }
        else
        {
            // LEAF NODE
            if (current.left == -1 && current.right == -1)
            {
                // "REMOVE" IT FROM FILE
                current.next_del = first_deleted_record;
                first_deleted_record = current.pos;

                writeRecord(current);

                fstream out_file(file_name, ios::binary | ios::in | ios::out);
                writeFileHeader(out_file);
                out_file.close();

                // UPDATE PARENT
                if (parent_current.left == pos)
                {
                    parent_current.left = NULL_POS;
                }
                else if (parent_current.right == pos)
                {
                    parent_current.right = NULL_POS;
                }

                writeRecord(parent_current);

                removed = true;
            }
            else if (current.left == NULL_POS)
            {
                // "REMOVE" IT FROM FILE
                current.next_del = first_deleted_record;
                first_deleted_record = current.pos;

                writeRecord(current);

                fstream out_file(file_name, ios::binary | ios::in | ios::out);
                writeFileHeader(out_file);
                out_file.close();

                // UPDATE PARENT
                if (parent_current.left == pos)
                {
                    parent_current.left = current.right;
                }
                else if (parent_current.right == pos)
                {
                    parent_current.right = current.right;
                }

                writeRecord(parent_current);

                removed = true;
            }
            else if (current.right == NULL_POS)
            {
                // "REMOVE" IT FROM FILE
                current.next_del = first_deleted_record;
                first_deleted_record = current.pos;

                writeRecord(current);

                fstream out_file(file_name, ios::binary | ios::in | ios::out);
                writeFileHeader(out_file);
                out_file.close();

                // UPDATE PARENT
                if (parent_current.left == pos)
                {
                    parent_current.left = current.left;
                }
                else if (parent_current.right == pos)
                {
                    parent_current.right = current.left;
                }

                writeRecord(parent_current);

                removed = true;
            }
            else
            {
                Nodo<TK> replacement = maxRecord(current.left);
                Nodo<TK> copy_current = current;

                current.copyAttributes(replacement);
                writeRecord(current);

                replacement.copyAttributes(copy_current);
                writeRecord(replacement);

                removed = remove(current.left, current, replacement.key);
            }
        }

        return removed;
    }

    void seekAll(int pos, std::vector<Nodo<TK>> &records)
    {
        if (pos == NULL_POS)
        {
            return;
        }

        Nodo<TK> current = getRecordByPos(pos);

        // Check if the current record is not in the free list
        records.push_back(current);

        seekAll(current.left, records);
        seekAll(current.right, records);
    }

    void seekFreeList(int pos, std::vector<Nodo<TK>> &records)
    {
        if (pos == NULL_POS || pos == -2)
        {
            return;
        }

        Nodo<TK> current = getRecordByPos(pos);

        // Check if the current record is in the free list
        records.push_back(current);

        seekFreeList(current.next_del, records);
    }
};

#endif // AVLFILE_H