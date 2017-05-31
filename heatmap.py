import matplotlib.pyplot as plt
import numpy as np

def grafico(n,x,y,u,v,mat):


    mat.reverse()
    mat=np.array(mat)

    plt.imshow(mat, interpolation='sinc', extent=[0,n,0,n], vmin=0.0)
    plt.colorbar().set_label('Temperatura')
    if x==[] or y==[] or u==[] or v==[]: 
        plt.quiver(x, y, u, v, alpha=.5)
        plt.quiver(x, y, u, v, edgecolor='k', facecolor='None', linewidth=.5)

        
    plt.show()

