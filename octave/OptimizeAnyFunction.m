#category: Optimization
#name: Optimize any Octave function
#description: Write a function and get the distribution of parameters that satisfies that function
#icon: octave.png
#menu: yes
#tool: no

s = "#Edit the Objective function and run the code.\n\
tinkercell\n\
PSO\n\
\n\
function y = Objective()\n\
    %ss = tc_getSteadyState()\n\
     y = randn(1);\n\
endfunction\n\
numpoints = 50;\n\
maxiter = 10;\n\
minimize = 0;\n\
title = \"Nonmonotic test\";\n\
runs = 5;\n\
g = ParticleSwarm(Objective,runs);\n\
";
tc_displayCode(s);

