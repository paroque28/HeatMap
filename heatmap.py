import matplotlib.pyplot as plt
import numpy as np

def grafico(n,mat):


    mat.reverse()
    mat=np.array(mat)

    plt.imshow(mat, interpolation='sinc', extent=[0,n,0,n], vmin=0.0)
    plt.colorbar().set_label('Temperatura')
    plt.show()

