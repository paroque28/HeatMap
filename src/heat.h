//
// Created by pablorod on 5/26/2017.
//

#ifndef HEATMAP_HEAT_H
#define HEATMAP_HEAT_H

#include <iostream>

template <typename  T>
void print(const std::string& str, T* matrix, unsigned int rows, unsigned int cols) {
    std::cout << str << "\n";
    for(int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf(" %8.3f", matrix[(i*cols)+j]);
        }
        printf("\n");
    }
    printf("\n");
}


template <typename  T>
T* getTemperatureMatrix(unsigned int size, T left,T right, T top, T bottom, T accurancy)
{
    unsigned int n = size*size;
    T A [n][n], b[n],x[n], x_last[n], y[n];
    T* A_ptr = &(**A);
    memset (A_ptr,0,n*n*sizeof(T));
    unsigned int i = 0, j = 0, k = 0, row = 0, col = 0;

    //set x to prom
    T prom = (left+right+top+bottom)/4;
    for (i = 0; i < n; ++i) {
        x[i] = prom;
    }
    print("x",x,size,size);
    //set b vector

    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            row = (i * size) + j; //rows on b, cols on A
            A[row][row] = 4;
            if (i == 0 || j == 0 || i == size - 1 || j == size - 1) {//bordes
                if (i == 0) {//borde arriba
                    b[row] = top;
                    A[row][(i+1)*size+(j)]=-1;
                } else if (i == size - 1){//borde abajo
                    b[row] = bottom;
                    A[row][(i-1)*size+(j)]=-1;
                }
                else{// bordes izq y der excepto si es borde de arriba y abajo)
                    A[row][(i+1)*size+(j)]=-1;
                    A[row][(i-1)*size+(j)]=-1;
                    if(j==0){
                        b[row] = left;
                        A[row][(i)*size+(j+1)]=-1;
                    }
                    else if(j==size-1){
                        b[row] = right;
                        A[row][(i)*size+(j-1)]=-1;
                    }
                    continue;
                }
                if(j==0) { //esquinas izquierda
                    b[row] += left;
                    A[row][(i)*size+(j+1)]=-1;
                }
                else if(j==size-1){ //esquinas derecha
                    b[row] += right;
                    A[row][(i)*size+(j-1)]=-1;
                }
                else// resto de los bordes
                {
                    A[row][(i)*size+(j+1)]=-1;
                    A[row][(i)*size+(j-1)]=-1;
                }
            }
            else
            {
                b[row] = 0;
                A[row][(i+1)*size+(j)]=-1;
                A[row][(i-1)*size+(j)]=-1;
                A[row][(i)*size+(j+1)]=-1;
                A[row][(i)*size+(j-1)]=-1;
            }
        }
    }
    print("b",b,size,size);

    // Liebmann
    bool flag = true;
    while (flag)
    {
        memcpy(x_last, x, n *  sizeof(T));
        for (i = 0; i < n; i++)
        {
            y[i] = (b[i] / A[i][i]);
            for (j = 0; j < n; j++)
            {
                if (j != i) {
                    y[i] = y[i] - ((A[i][j] / A[i][i]) * x[j]);
                    x[i] = y[i];

                }
            }
        }

        for (i = 0; i < n; ++i) {
            if(std::abs(x[i]-x_last[i])<accurancy) {
                flag = false;
                continue;
            }

        }
    }
    print("x",x,size,size);
    return x;
}

template <typename  T>
T* getVectores(T* temp, unsigned int size)
{
    T t [size][size][2] ;
    T* t_ptr = &(***t);

}

#endif //HEATMAP_HEAT_H
