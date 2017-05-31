//
// Created by pablorod on 5/26/2017.
//

#ifndef HEATMAP_HEAT_H
#define HEATMAP_HEAT_H
#define nthreads 10
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
void printvectors(const std::string& str, T* vector, unsigned int sizeVector, unsigned int sizeArray) {
    std::cout << str << "\n";
    for(int i = 0; i < sizeArray; i++) {
        for (int j = 0; j < sizeVector; ++j) {
            printf(" %8.3f", *(vector+(i*sizeVector)+j));
        }
        printf("\n");
    }
    printf("\n");
}


template <typename  T>
T* getTemperatureMatrix(unsigned int size, T left,T right, T top, T bottom, T accurancy)
{
    unsigned int n = size*size;
    T A [n][n], b[n], x_last[n], y[n];
    T* x = static_cast<T*>(malloc(sizeof(T)*n));
    T* A_ptr = &(**A);
    memset (A_ptr,0,n*n*sizeof(T));
    unsigned int i = 0, j = 0, row = 0;

    //set x to prom
    T prom = (left+right+top+bottom)/4;

    #pragma omp parallel for schedule(dynamic,5) private(i) num_threads(nthreads)
    for (i = 0; i < n; ++i) {
        x[i] = prom;
    }
    //set b vector
    #pragma omp parallel for schedule(dynamic,1) collapse(2) private(i,j,row) num_threads(nthreads)
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
        #pragma omp parallel for schedule(dynamic,2) private(i) num_threads(nthreads)
        for (i = 0; i < n; ++i) {
            if(std::abs(x[i]-x_last[i])<accurancy) {
                flag = false;
            }

        }
    }
    print("x",x,size,size);
    return x;
}

template <typename  T>
T* getVectores(T* temp, unsigned int size, unsigned int density, T left,T right, T top, T bottom, T k, unsigned int* numVectores)
{
    if(size<density) density = size;
    T* t_ptr = static_cast<T*>(malloc(sizeof(T)*4*density*density));
    unsigned int step = size/density;
    T xNxt,xPrv, yNxt,yPrv;
    unsigned int x,y;
    //#pragma omp parallel for schedule(dynamic) collapse(2) private(x,y,xNxt,xPrv, yNxt,yPrv) num_threads(nthreads)
    for (unsigned int i = 0; i < density; i++) {
        for (unsigned int j = 0; j < density; j++) {
            x = i*step;
            y = j*step;
            *(t_ptr+(((i*size)+j)*4))=x;
            *(t_ptr+(((i*size)+j)*4)+1)=y;
            //check for borders
            if(step>x) xPrv = left;
            else xPrv = *(temp + ((x-step)*size)+y);
            if(step>y) yPrv = top;
            else yPrv = *(temp + (x*size)+y-step);
            if(x+step>=size) xNxt = right;
            else xNxt = *(temp + ((x+step)*size)+y);
            if(y+step>=size) yNxt = bottom;
            else yNxt = *(temp + (x*size)+y+step);

            *(t_ptr+(((i*size)+j)*4)+2)=-k*((xNxt-xPrv)/(2*step));
            *(t_ptr+(((i*size)+j)*4)+3)=-k*((yNxt-yPrv)/(2*step));
        }
    }
    printvectors("Vectors:", t_ptr,4,density*density);
    *numVectores = density*density;
    return t_ptr;
}

#endif //HEATMAP_HEAT_H
