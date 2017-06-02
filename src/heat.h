/**
 * @file heat.h
 * @brief Calculo de temperaturas en una matriz.
 *
 * Calcula las temperaturas de los diferentes puntos de una matriz que representan una placa delgada rectangular.
 */

#ifndef HEATMAP_HEAT_H
#define HEATMAP_HEAT_H
#define nthreads 10
//#define PARALELO
#include <iostream>
template <typename  T>
/**
 * @brief Imprime  la matriz de manera ordenada y con formato.
 * @param matrix La matriz.
 * @param rows Cantidad de filas de la matriz.
 * @param cols Cantidad de columnas de la matriz.
 */
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

/**
 * @brief Imprime un vector de forma ordenada y con formato.
 * @param vector El vector
 * @param sizeVector Tamano del vector.
 * @param sizeArray Tamano del arreglo.
 */
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

/**
 * @brief Calcula la temperatura de un punto dado.
 * @param size Tamano de la matriz (nxn).
 * @param left Temperatura en el borde izquierdo.
 * @param right Temperatura en el borde derecho.
 * @param top Termperatura en el borde superior.
 * @param bottom Temperatura en el borde inferior.
 * @param accurancy Error tolerado.
 * @return Matriz con las temperaturas calculadas
 */
template <typename  T>
T* getTemperatureMatrix(const unsigned int size, T left,T right, T top, T bottom, T accurancy)
{
    const unsigned int n = size*size;
    T* x = static_cast<T*>(malloc(sizeof(T)*n));

    //set x to prom
    unsigned int count =0;
    T prom= 0;
    if(top == top) {prom += top; count++;}
    if(right == right) {prom += right; count++;}
    if(left == left) {prom += left; count++;}
    if(bottom == bottom) {prom += bottom; count++;}

    prom/=count;
    #ifdef PARALELO
        std::cout<<"Paralelismo activo"<<std::endl;
        #pragma omp parallel for schedule(dynamic)
    #endif
    for (int l = 0; l < n; ++l) {
        x[l]=prom;
    }
    bool flag = true;
    while (flag) {
#ifdef PARALELO
        #pragma omp parallel for schedule(dynamic) collapse(2)
#endif
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                T sum[4] ;
                unsigned int num = 4;
                unsigned int index = 0;
                bool izq = true, der=true , abajo = true, arriba = true;

                    if (i == 0) {
                        arriba = false;
                        if (top != top)num--;
                        else sum[index++] = top;
                    }
                    if (j == 0) {
                        izq = false;
                        if (left != left)num--;
                        else sum[index++] = left;
                    }
                    if (i == size - 1) {
                        abajo = false;
                        if (bottom != bottom)num--;
                        else sum[index++] = bottom;
                    }
                    if (j == size - 1) {
                        der = false;
                        if (right != right)num--;
                        else sum[index++] = right;
                    }

                    if(abajo)sum[index++]=x[((i+1) * size) + (j)];
                    if(arriba)sum[index++]=x[((i-1) * size) + (j)];
                    if(der)sum[index++]=x[((i) * size) + (j+1)];
                    if(izq)sum[index]=x[((i) * size) + (j-1)];

                T temp = 0;
                for (int k = 0; k < num; ++k) {
                    temp += sum[k];
                }
                temp = temp /num;

                flag = false;
                if(!flag) flag = std::abs(x[((i) * size) + (j)] - temp) > accurancy;

                x[((i) * size) + (j)] = temp;
            }
        }
        //print("x",x,size,size);
    }
    print("x",x,size,size);

    return x;
}

/**
 * @brief Calcula la temperatura de la matriz utilizando el metodo de Liebmann.
 * @param size Tamano de la matriz (nxn).
 * @param left Temperatura en el borde izquierdo.
 * @param right Temperatura en el borde derecho.
 * @param top Temperatura en el borde superior.
 * @param bottom Temperatura en el borde inferior.
 * @param accurancy Error tolerado.
 * @return Matriz con las temperaturas calculadas
 */
template <typename  T>
T* getTemperatureMatrixLiebmann(const unsigned int size, T left,T right, T top, T bottom, T accurancy)
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
 #ifdef PARALELO
    #pragma omp parallel for schedule(dynamic,5) private(i) num_threads(nthreads)
 #endif
    for (i = 0; i < n; ++i) {
        x[i] = prom;
    }
    //set b vector
    #ifdef PARALELO
        #pragma omp parallel for schedule(dynamic,1) collapse(2) private(i,j,row) num_threads(nthreads)
    #endif
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
    //print("A",A,n,n);
    //print("b",b,size,size);

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
#ifdef PARALELO
    #pragma omp parallel for schedule(dynamic,2) private(i) num_threads(nthreads)
#endif
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
/**
 * @brief Calcula los vectores de temperatura.
 * @param temp Matriz de temperaturas.
 * @param size Tamano de la matriz (nxn).
 * @param density Cantidad de vectores que quiere ver.
 * @param left Temperatura en el borde izquierdo.
 * @param right Temperatura en el borde derecho.
 * @param top Temperatura en el borde superior.
 * @param bottom Temperatura en el borde inferior.
 * @param k Constante fisica.
 * @param numVectores Return de vectores.
 * @return Vectores de temperatura
 */
T* getVectores(T* temp, unsigned int size, unsigned int density, T left,T right, T top, T bottom, T k, unsigned int* numVectores)
{
    if(size<density||size <15) density = size;
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
            if(step>x) xPrv = top;
            else xPrv = *(temp + ((x-step)*size)+y);
            if(step>y) yPrv = left;
            else yPrv = *(temp + (x*size)+y-step);
            if(x+step>=size) xNxt = bottom;
            else xNxt = *(temp + ((x+step)*size)+y);
            if(y+step>=size) yNxt = right;
            else yNxt = *(temp + (x*size)+y+step);


            *(vectores+(((i*density)+j)*4)+2)=-k*((xNxt-xPrv));
            *(vectores+(((i*density)+j)*4)+3)=-k*((yNxt-yPrv));
            if(*(vectores+(((i*density)+j)*4)+2)!=*(vectores+(((i*density)+j)*4)+2)) *(vectores+(((i*density)+j)*4)+2) = 0;
            if(*(vectores+(((i*density)+j)*4)+3)!=*(vectores+(((i*density)+j)*4)+3)) *(vectores+(((i*density)+j)*4)+3) = 0;
        }
    }
    printvectors("vectores",vectores,4,density*density);
    *numVectores = density*density;
    return vectores;
}

#endif //HEATMAP_HEAT_H
