//
// Created by pablorod on 5/31/2017.
//

#ifndef HEATMAP_TOPYTHON_H
#define HEATMAP_TOPYTHON_H
#include <python2.7/Python.h>
#include <string>
#include <iostream>

template <typename  T>
void sendToPython(T* matriz, T* vectores, unsigned int size, unsigned int numVectors)
{
    std::string pyMatr = "[";
    for (int i = 0; i < size; ++i) {
        pyMatr += "[";
        for (int j = 0; j < size-1; ++j) {
            pyMatr += std::to_string(*(matriz+(i*size)+j));
            pyMatr += ",";
        }
        pyMatr += std::to_string(*(matriz+(i*size)+size-1));
        pyMatr += "]";
        if(i != size-1) pyMatr += ",";
    }
    pyMatr += "]";
    //std::cout<<pyMatr<<std::endl;
    std::string x = "[",y = "[",u = "[",v = "[";
    for (int k = 0; k < numVectors; ++k) {
        if(*(vectores+k*4+3) != 0 && *(vectores+k*4+2) != 0) {
            x += std::to_string(*(vectores + k * 4 + 1));
            y += std::to_string(*(vectores + k * 4));
            u += std::to_string(*(vectores + k * 4 + 3));
            v += std::to_string(*(vectores + k * 4 + 2));
            if (k != (numVectors) - 1) {
                x += ",";
                y += ",";
                u += ",";
                v += ",";
            }
        }
    }
    x += "]";y += "]";u += "]";v += "]";
    std::string pythoncmd = "grafico(" + std::to_string(size) + ", " + x + ", " + y + ", " + u + ", " + v + ", " + pyMatr + ")";
    //std::cout<< pythoncmd <<std::endl;

    char * writable = new char[pythoncmd.size() + 1];
    std::copy(pythoncmd.begin(), pythoncmd.end(), writable);
    writable[pythoncmd.size()] = '\0';

    Py_Initialize(); // Inicializa el interprete de Python.

    // Indica que el actual directorio presenta modulos
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./')");

    // Importa el modulo plot:
    PyRun_SimpleString("from heatmap import *");
    PyRun_SimpleString(writable);

    Py_Finalize();
    delete[] writable;
}
#endif //HEATMAP_TOPYTHON_H
