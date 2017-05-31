#include <python2.7/Python.h>
#include <iostream>

int main() {
    int matriz[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };

    std::string pyMatr = "";

    pyMatr += "[";
    for (int row = 0 ; row < 3 ; row++) {
        pyMatr += "[";

        for (int col = 0 ; col < 3 ; col++) {
            pyMatr += std::to_string(matriz[row][col]);
            if (col != 2) {
                pyMatr += ",";
            }
        }

        pyMatr += "]";
        if (row != 2) {
            pyMatr += ",";
        }
    }
    pyMatr += "]";

    std::string n = "3";

    std::string pythoncmd = "grafico(" + n + ", " + pyMatr + ")";

    std::cout << pythoncmd << std::endl;

    char * writable = new char[pythoncmd.size() + 1];
    std::copy(pythoncmd.begin(), pythoncmd.end(), writable);
    writable[pythoncmd.size()] = '\0';

    Py_Initialize(); // Inicializa el interprete de Python.

    // Indica que el actual directorio presenta modulos
    PyRun_SimpleString("import sys");
    // Intente primero sin esto:
    //PyRun_SimpleString("import numpy as np");
    //PyRun_SimpleString("import matplotlib.pyplot as plt");
    PyRun_SimpleString("sys.path.append('./')");

    // Importa el modulo plot:
    PyRun_SimpleString("from heatmap import *");
    PyRun_SimpleString(writable);

    Py_Finalize();
    delete[] writable;
    return 0;
}
