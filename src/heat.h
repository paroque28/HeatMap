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
            printf(" %8.3f", *(matrix+(i*cols)+j));
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
T* getTemperatureMatrix(const unsigned int size, T left,T right, T top, T bottom, T accurancy)
{
    const unsigned int n = size*size;
    T* A  = static_cast<T*>(malloc(sizeof(T)*n*n));
    T* b = static_cast<T*>(malloc(sizeof(T)*n));
    T* x_last = static_cast<T*>(malloc(sizeof(T)*n));
    T* y = static_cast<T*>(malloc(sizeof(T)*n));
    T* x = static_cast<T*>(malloc(sizeof(T)*n));
    memset (A,0,n*n*sizeof(T));
    unsigned int i = 0, j = 0, row = 0;

    //set x to prom
    T prom = (left+right+top+bottom)/4;

    //#pragma omp parallel for schedule(dynamic,5) private(i) num_threads(nthreads)
    for (i = 0; i < n; ++i) {
        x[i] = prom;
    }
    //set b vector
    //#pragma omp parallel for schedule(dynamic,1) collapse(2) private(i,j,row) num_threads(nthreads)
    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j) {
            row = (i * size) + j; //rows on b, cols on A
            *(A+(row*n)+row) = 4;
            if (i == 0 || j == 0 || i == size - 1 || j == size - 1) {//bordes
                if (i == 0) {//borde arriba
                    b[row] = top;
                    *(A+(row*n)+(i+1)*size+(j))=-1;
                } else if (i == size - 1){//borde abajo
                    b[row] = bottom;
                    *(A+(row*n)+(i-1)*size+(j))=-1;
                }
                else{// bordes izq y der excepto si es borde de arriba y abajo)
                    *(A+(row*n)+(i+1)*size+(j))=-1;
                    *(A+(row*n)+(i-1)*size+(j))=-1;
                    if(j==0){
                        b[row] = left;
                        *(A+(row*n)+(i)*size+(j+1))=-1;
                    }
                    else if(j==size-1){
                        b[row] = right;
                        *(A+(row*n)+(i)*size+(j-1))=-1;
                    }
                    continue;
                }
                if(j==0) { //esquinas izquierda
                    b[row] += left;
                    *(A+(row*n)+(i)*size+(j+1))=-1;
                }
                else if(j==size-1){ //esquinas derecha
                    b[row] += right;
                    *(A+(row*n)+(i)*size+(j-1))=-1;
                }
                else// resto de los bordes
                {
                    *(A+(row*n)+(i)*size+(j+1))=-1;
                    *(A+(row*n)+(i)*size+(j-1))=-1;
                }
            }
            else
            {
                b[row] = 0;
                *(A+(row*n)+(i+1)*size+(j))=-1;
                *(A+(row*n)+(i-1)*size+(j))=-1;
                *(A+(row*n)+(i)*size+(j+1))=-1;
                *(A+(row*n)+(i)*size+(j-1))=-1;
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
            y[i] = (b[i] / *(A+(i*n)+i));
            for (j = 0; j < n; j++)
            {
                if (j != i) {
                    y[i] = y[i] - ((*(A+(i*n)+j) / (*(A+(i*n)+i))) * x[j]);
                    x[i] = y[i];

                }
            }
        }
        //#pragma omp parallel for schedule(dynamic,2) private(i) num_threads(nthreads)
        for (i = 0; i < n; ++i) {
            if(std::abs(x[i]-x_last[i])<accurancy) {
                flag = false;
            }

        }
    }
    print("x",x,size,size);
    free(A);
    free(b);
    free(y);
    free(x_last);
    return x;
}

template <typename  T>
T* getVectores(T* temp, unsigned int size, unsigned int density, T left,T right, T top, T bottom, T k, unsigned int* numVectores)
{
    if(size<density) density = size;
    T* vectores = static_cast<T*>(malloc(sizeof(T)*4*density*density));
    unsigned int step = size/density;
    T xNxt,xPrv, yNxt,yPrv;
    T normaMaxima = 0;
    unsigned int x,y;
    //#pragma omp parallel for schedule(dynamic) collapse(2) private(x,y,xNxt,xPrv, yNxt,yPrv) num_threads(nthreads)
    for (unsigned int i = 0; i < density; i++) {
        for (unsigned int j = 0; j < density; j++) {
            x = i*step + step/2;
            y = j*step + step/2;
            *(vectores+(((i*density)+j)*4))=x;
            *(vectores+(((i*density)+j)*4)+1)=y;
            //check for borders
            if(step>x) xPrv = left;
            else xPrv = *(temp + ((x-step)*size)+y);
            if(step>y) yPrv = top;
            else yPrv = *(temp + (x*size)+y-step);
            if(x+step>=size) xNxt = right;
            else xNxt = *(temp + ((x+step)*size)+y);
            if(y+step>=size) yNxt = bottom;
            else yNxt = *(temp + (x*size)+y+step);


            *(vectores+(((i*density)+j)*4)+2)=x-k*((xNxt-xPrv)/(2*step));
            *(vectores+(((i*density)+j)*4)+3)=y-k*((yNxt-yPrv)/(2*step));
            T norma = *(vectores+(((i*density)+j)*4)+2) * *(vectores+(((i*density)+j)*4)+2) + *(vectores+(((i*density)+j)*4)+3) * *(vectores+(((i*density)+j)*4)+3);
            if (normaMaxima< norma) normaMaxima = norma;

        }
    }
    normaMaxima = sqrt(normaMaxima);
    //for (int l = 0; l < density * density; ++l) {
    //    *(vectores+(l*4)+2)  =(*(vectores+(l*4)+2) * (step/normaMaxima)) + *(vectores+(l*4));
    //    *(vectores+(l*4)+3)  =(*(vectores+(l*4)+3) * (step/normaMaxima)) + *(vectores+(l*4)+1);

   // }
    printvectors("Vectors:", vectores,4,density*density);
    *numVectores = density*density;
    return vectores;
}

#endif //HEATMAP_HEAT_H
