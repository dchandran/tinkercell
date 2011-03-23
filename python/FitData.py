"""
category: Optimization
name: Fit time-series data
description: fit model parameters to time-series data
icon: curve.png
menu: yes
tool: no
"""
from tinkercell import *
from tc2py import *
from numpy import *

fname = tc_getFilename()
indx = []

def leastSquares(params)
     
    sim = tc_simulate(

if len(filename) > 0:
    delim = '\t'
    if fname.count('csv') > 0:
        delim = ','
    dat = genfromtxt(fname, dtype=<type 'float'>, comments='#', delimiter=delim)
    
