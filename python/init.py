try:
    from tinkercell import *
    from numpy import *
    from scipy import *
    print 'Python modules loaded: Numpy and Scipy'
except ImportError:
    allLoaded = False;
    print 'Numpy and Scipy were not found'

try:
    from pysces import *
    import networkx as nx
    from nxAutoLayout import *
    from tc2pysces import *
    print 'Python modules loaded: PySCeS, NetworkX. Edit init.py to add more'
except ImportError:
    allLoaded = False;
    print 'PySCeS and/or NetworkX were not found'
