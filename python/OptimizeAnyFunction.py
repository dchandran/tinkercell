"""
category: Optimization
name: Optimize any Python function
description: Write a function and get the distribution of parameters that satisfies that function
icon: python.png
menu: yes
tool: no
"""

s = "#Edit the Objective function and run the code.\n\
from tinkercell import *\n\
from tc2py import *\n\
from numpy import *\n\
from PSO import *\n\
\n\
def Objective():\n\
    #ss = tc_getSteadyState()\n\
    return (numpy.random.normal(0,1))\n\
optimizer = ParticleSwarm()\n\
optimizer.numpoints = 50\n\
optimizer.maxiter = 10\n\
optimizer.minimize = False\n\
optimizer.title = \"Nonmonotic test\"\n\
runs = 5 #5 separate runs\n\
g = optimizer.run(Objective,runs)\n\
"
tc_displayCode(s)

