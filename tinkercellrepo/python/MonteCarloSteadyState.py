"""
category: Model Variations
name: Steady State Distrubtion
descr: Run repeated steady state calculations using random parameters (based on min/max parameter values)
menu: yes
icon: module.png
tool: no
"""
items = tc_allItems()
params = tc_getParameters(items)
params2 = fromTC(params)
avgvalues = params2[2][0]
minvalues = params2[2][1]
maxvalues = params2[2][2]
#check if all min/max values ok

s = ""

for j in range(0,params.rows):
	if minvalues[j] == maxvalues[j]: #not ok
		s += tc_getRowName(params, j) + " "
	#show warning and then continue
ok = 1
if len(s) > 0:
	tc_clear() #clear console
	ok = tc_askQuestion("warning: the following parameters have invalid ranges :- \n" + s + "\n\nContinue with analysis?")
if ok == 1:
	n = int( tc_getNumber("Number of samples?") )
	tc_holdPlot(1)
	colnames = []
	output = []
	for i in range(0,n):
		tc_showProgress("Sampling parameters", 100 * i/n)
		for j in range(0,params.rows):
			x = random.uniform(log(minvalues[j]), log(maxvalues[j]))
			tc_setMatrixValue(params, j, 0, exp(x))
			tc_updateParameters(params)
		tc_printMatrix(params)
		ss = tc_getSteadyState()
		if len(colnames) == 0:
			colnames = fromTC(ss.rownames)
			for k in range(0,ss.rows):
				output.append( range(0,n) );
		for k in range(0,ss.rows):
			output[k][i] = tc_getMatrixValue(ss,k,0)
	tc_showProgress("Sampling parameters", 100 )
	tc_scatterplot(toTC(output,[],colnames), "Steady state distribution")

