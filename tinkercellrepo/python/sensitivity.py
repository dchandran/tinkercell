"""
category: Perturbation
name: Local sensitivity analysis
description: Compute slope of a function with respect to a class of parameters
menu: yes
tool: yes
"""

family = tc_getStringDialog("Please select a family of components, e.g. Promoter")
formula = tc_getStringDialog("Enter the target formula involving component names")

def Objective(formula):
    ss = tc_getSteadyState()
    for i in range(0,ss.rows):
        locals()[ tc_getRowName(ss,i) ] = tc_getMatrixValue(ss,i,0)
    return (eval(formula))

#get just relevant parameters
params = tc_getParameters(tc_itemsOfFamily(family))

#sensitivities
S = tc_createMatrix(params.rows, 1)
maxS = 0.0
tc_setColumnName(S,0,"slope")
for i in range(0, params.rows):
    tc_showProgress("Calculating Slopes", int(100 * i / params.rows))
    tc_setRowName(S,i,tc_getRowName(params,i))
    x = tc_getMatrixValue(params, i, 0)
    dx = x * 0.01   #one percent change in x
    
    y0 = Objective(formula)   #original value

    tc_setMatrixValue(params, i, 0, x + dx)
    tc_updateParameters(params)
    y1 = Objective(formula)/y0    # f ( x- dx)

    tc_setMatrixValue(params, i, 0, x - dx)
    tc_updateParameters(params)
    y2 = Objective(formula)   # f( x + dx)

    tc_setMatrixValue(params, i, 0, x)

    q = (y1 - y2)/(2*dx)       #slope = { f(x+dx) - f(x-dx)  } / 2dx
    tc_setMatrixValue(S, i, 0,  q)
    if q > maxS: maxS = q

    tc_updateParameters(params)
tc_showProgress("Calculating Slopes", int(100))
tc_printMatrix(S)

if maxS > 0:
    for i in range(0,S.rows):
        q = tc_getMatrixValue(S, i, 0)
        tc_setMatrixValue(S, i, 0, q/maxS)

tc_scatterplot(S, formula)
