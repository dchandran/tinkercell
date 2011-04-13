"""
category: Optimization
name: Minimize a formula (steady state)
description: Minimize the given formula. Values are computed at steady state
icon: down.png
menu: yes
tool: no
"""
from tinkercell import *
from tc2py import *
from numpy import *
import CrossEntropy

#get formula
optimFormula = tc_getStringDialog("Enter formula: ")

#objective function for CrossEntropy
def FitFormula_Objective():
    ss = tc_getSteadyState()
    for i in range(0,ss.rows):
        locals()[ tc_getRowName(ss,i) ] = tc_getMatrixValue(ss,i,0)
    return (eval(optimFormula))

minimize = True
res = CrossEntropy.OptimizeParameters(FitFormula_Objective, "Minimize " + optimFormula, 100, 100, minimize)
mu = res[0]
sigma2 = res[1]
paramnames = res[2]
CrossEntropy.DoPCA(mu, sigma2, paramnames)
n = len(mu)
params = tc_createMatrix(n, 1)
for i in range(0,n):
    tc_setMatrixValue(params, i, 0, mu[i])
    tc_setRowName(params, i, paramnames[i])
tc_setParameters(params,1)

