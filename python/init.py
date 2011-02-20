try:
    from tinkercell import *
    from tc2py import *
except ImportError:
    print 'TinkerCell Python module not found'

try:
    from numpy import *
    print 'Python modules loaded: Numpy'
except ImportError:
    print 'Python modules not loaded: Numpy'

try:
    from scipy import *
    print 'Python modules loaded: Scipy'
except ImportError:
    print 'Python modules not loaded: Scipy'

try:
    from pysces import *
    from tc2pysces import *
    print 'Python module loaded: PySCeS'
except ImportError:
    print 'Python module not loaded: PySCeS'

try:
    import networkx as nx
    from nxAutoLayout import *
    print 'Python module loaded: NetworkX'
except ImportError:
    print 'Python module not loaded: NetworkX'

