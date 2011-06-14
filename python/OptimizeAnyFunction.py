"""
category: Optimization
name: Optimize any Python function
description: Write a function and get the distribution of parameters that satisfies that function
icon: python.png
menu: yes
tool: no
"""
fout = open("example.py","w")
s = "#Copy this example code into the Coding Window in TinkerCell.\n\
#Edit the Objective function and run the code.\n\
from tinkercell import *\n\
from tc2py import *\n\
from numpy import *\n\
import CrossEntropy\n\
#objective function for CrossEntropy\n\
def Objective():\n\
    ss = tc_getSteadyState()\n\
    return (numpy.random.normal(0,1))\n\
#minimize or maximize?\n\
minimize = False\n\
res = CrossEntropy.OptimizeParameters(Objective, \"name of function\", 100, 100, minimize)\n\
#get the results and analyze results\n\
mu = res[0]\n\
sigma2 = res[1]\n\
paramnames = res[2]\n\
CrossEntropy.DoPCA(mu, sigma2, paramnames)\n\
#now we set the parameters back in the model (option)\n\
n = len(mu)\n\
params = tc_createMatrix(n, 1)\n\
for i in range(0,n):\n\
    tc_setMatrixValue(params, i, 0, mu[i])\n\
    tc_setRowName(params, i, paramnames[i])\n\
tc_setParameters(params,1)\n\
"
fout.write(s)
fout.close()
tc_openUrl("example.py")

