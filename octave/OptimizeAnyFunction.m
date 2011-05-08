#category: Optimization
#name: Optimize any Octave function
#description: Write a function and get the distribution of parameters that satisfies that function
#icon: octave.png
#menu: yes
#tool: no

fout = fopen("example.m","w");
fprintf(fout,"#Copy this example code into the Coding Window in TinkerCell.\n\
tinkercell\n\
CrossEntropy\n\
#Edit the Objective function and run the code.\n\
#objective function for CrossEntropy\n\
function y = Objective()\n\
    ss = tinkercell.tc_getSteadyState();\n\
    y = randn(0,1);\n\
end\n\
#minimize or maximize?\n\
minimize = 0;\n\
res = OptimizeParameters(@Objective, \"name of function\", 100, 100, minimize);\n\
#get the results and analyze results\n\
mu = res(0);\n\
sigma2 = res(1);\n\
paramnames = res(2);\n\
DoPCA(mu, sigma2, paramnames);\n\
#now we set the parameters back in the model (option)\n\
n = size(mu,1);\n\
params = tinkercell.tc_createMatrix(n, 1);\n\
for i = 1:n\n\
    tinkercell.tc_setMatrixValue(params, i, 0, mu(i));\n\
    tinkercell.tc_setRowName(params, i, paramnames(i));\n\
end\n\
tinkercell.tc_setParameters(params,1);\n\
");
fclose(fout);
tinkercell.tc_openUrl("example.m")

