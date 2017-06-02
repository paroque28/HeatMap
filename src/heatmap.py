import numpy as np
import matplotlib.pyplot as plt

print("Welcome to MatPlotLib")
def grafico(n,x,y,u,v,mat):
    mat.reverse()
    mat=np.array(mat)

    plt.title("Heat Map")

    plt.imshow(mat, interpolation='bilinear', extent=[0,n-1,0,n-1])
    plt.colorbar().set_label('Temperature')

    if x!=[] or y!=[] or u!=[] or v!=[]:
        plt.quiver(x, y, u, v, alpha=.5)
        plt.quiver(x, y, u, v, edgecolor='k', facecolor='None', linewidth=.5)

    plt.show()

