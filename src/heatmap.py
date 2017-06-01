import sys
print(sys.version)
import matplotlib.pyplot as plt
import numpy as np

def grafico(n,x,y,u,v,mat):


    mat.reverse()
    print(mat)

    mat=np.array(mat)
    print("este es n")
    print(n)

    plt.title("Heat Map")

    plt.imshow(mat, interpolation='bilinear', extent=[0,n-1,0,n-1], vmin=0.0)
    plt.colorbar().set_label('Temperatura')
    if x!=[] and y!=[] and u!=[] and v!=[]:
        plt.quiver(x, y, u, v, alpha=.5)
        plt.quiver(x, y, u, v, edgecolor='k', facecolor='None', linewidth=.5)

        
    plt.show()

