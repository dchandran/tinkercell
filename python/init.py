try:
    from tinkercell import *
    from tc2py import *
    print '_tinkercell imported'
except ImportError:
    print 'TinkerCell Python module not found'

try:
    from numpy import *
    print 'Numpy imported'
except ImportError:
    print 'Numpy not loaded'

try:
    from scipy import *
    print 'Scipy imported'
except ImportError:
    print 'Scipy not loaded'

try:
    from pysces import *
    from tc2pysces import *
    print 'Pysces imported'
except ImportError:
    print 'Pysces not loaded'

try:
    import networkx as nx
    from nxAutoLayout import *
    print 'NetworkX imported'
except ImportError:
    print 'NetworkX not loaded'

