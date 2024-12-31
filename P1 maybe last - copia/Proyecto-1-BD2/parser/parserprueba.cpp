#include <iostream>
#include "tokensSQL.h"
#include "parserSQL.h"

using namespace std;

int main()
{
    const char *input =
        "CREATE TABLE Adult FROM FILE 'D:/bd2/P1 maybe last/Proyecto-1-BD2/data/SmallAdultDataset.csv' USING INDEX AVL('DNI');"
        "INSERT INTO Adult VALUES ('12345678', 'Mariel Tovar Tolentino', 'CS', 'Soltero/a', '5000', '21', 'F');"
        "SELECT * FROM Adult WHERE DNI = '12345678';"
        "SELECT * FROM Adult WHERE SUELDO BETWEEN '4000' AND '5000';"
        "DELETE FROM Adult WHERE DNI='12345678';";

    // extendible
    "CREATE TABLE Employee FROM FILE 'D:/bd2/P1 maybe last/Proyecto-1-BD2/data/SmallAdultDataset.csv' USING INDEX EXTENDIBLE('DNI');"
    "INSERT INTO Employee VALUES ('98765432', 'John Doe', 'IT', 'Casado/a', '7000', '30', 'M');"
    "SELECT * FROM Employee WHERE DNI = '98765432';" // 73060889
    "SELECT * FROM Employee WHERE DNI = '59237933';"
    "DELETE FROM Employee WHERE DNI='59237933';";
    // DELETE FROM Employee WHERE DNI='98765432';

    // SEQUENTIAL

    "CREATE TABLE Employee FROM FILE 'path/to/file.csv' USING INDEX SEQUENTIAL('DNI');"
    "INSERT INTO Employee VALUES ('12345645', 'John Doe', 'Engineer', 'Single', '5000', '30', 'M');"
    "SELECT * FROM Employee WHERE DNI = '12345645';"
    "DELETE FROM Employee WHERE DNI = '12345645';"

        // Crear el escáner
        Scanner scanner(input);
    // Crear la instancia de AVLFile
    AVLFile<long> avlFile("C:/Users/Public/bd2/Proyecto-1-BD2/estructuras/avl/avl_tree.dat");

    // Crear el parser con ambos argumentos
    Parser parser(&scanner, &avlFile);
    parser.parse();

    return 0;
}