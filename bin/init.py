try:
  from numpy import *
  print 'NumPy loaded\n'
  from scipy import *
  print 'SciPy loaded\n'
  from pysces import *
  print 'PySCeS loaded\n'
  import networkx as nx
  print 'NetworkX Loaded\n'
  from nxAutoLayout import *
  from tc2pysces import *
  print 'Python modules loaded: Numpy, Scipy, PySCeS, NetworkX. Edit init.py to add more.'
except ImportError:
  allLoaded = False;
