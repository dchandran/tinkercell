try:
    outputOff();
    from numpy import *
    from scipy import *
    from pysces import *
    import networkx as nx
    from nxAutoLayout import *
    from tc2pysces import *
    outputOn();
    print 'Python modules loaded: Numpy, Scipy, PySCeS, NetworkX. Edit init.py to add more.'
except ImportError:
    allLoaded = False;
