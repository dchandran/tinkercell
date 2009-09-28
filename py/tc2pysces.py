def load():
    import pytc
    import pysces
    import re
  
    A = pytc.allItems();
    N = pytc.stoichiometry(A);
    rates0 = pytc.rates(A);
    params = pytc.parameters(A);
    fixed = pytc.fixedVariables(A);
    inits = pytc.initialValues(A);
    funcsNames = pytc.forcingFunctionNames(A);
    funcsAssign0 = pytc.forcingFunctionAssignments(A);
    triggers = pytc.eventTriggers(A);
    events0 = pytc.eventResponses(A);
    emptyExists = False;
    modelString = '';
    rates = [];
    funcsAssign = [];
    events = [];
  
    p = re.compile('\^');
    for i in rates0:
        rates.append(p.sub('**',i));

    for i in funcsAssign0:
        funcsAssign.append(p.sub('**',i));

    for i in events0:
        events.append(p.sub('**',i));

    reacs = len(rates);
    species = len(N[0]);
 
    for i in range(0,reacs):   #for each reaction
        lhs = [];
        rhs = [];
        for j in range(0,species):  #get reactants and products
            n = N[2][j][i];
            if n > 0:
                if n != 1.0:
                    rhs.append("{" + str(n) + "}" + str(N[0][j])); #product
                else:
                    rhs.append(str(N[0][j]));
            elif n < 0:
                n = -n;
                if n != 1.0:
                    lhs.append("{" + str(n) + "}" + str(N[0][j])); #reactants
                else:
                    lhs.append(str(N[0][j]));
    #full reaction and its rate
        if len(lhs) > 0 or len(rhs) > 0:
            modelString += N[1][i] + ":\n"; #print its name
            if len(lhs) == 0:
                lhs.append("EMPTY");
                emptyExists = True;
            if len(rhs) == 0:
                rhs.append("EMPTY");
                emptyExists = True;
            modelString += "  " + "+".join(lhs) + " > " + "+".join(rhs) + "\n"; 
            modelString += "  " + rates[i] + "\n\n";

    #we are done with reactions. moving on to params, events, functions, etc.
    fix = '';
  
    if emptyExists:
        fix = "FIX: EMPTY";

    if len(fixed) == 2 and len(fixed[0]) == len(fixed[1]):
        n = len(fixed[0]);
        if n > 0:
            if not emptyExists:
                fix += "FIX:";
            for i in range(0,n):
                fix += " " + fixed[0][i];
   
        modelString = fix + "\n\n" + modelString;
 
    modelString += "# Init ext\n";
  
    #fixed variables
    if len(fixed) == 2 and len(fixed[0]) == len(fixed[1]):
        n = len(fixed[0]);
        for i in range(0,n):
            modelString += fixed[0][i] + " = " + str(fixed[1][i]) + "\n";
  
    #initial variables
    hashInits = {};
    if len(inits) == 2 and len(inits[0]) == len(inits[1]):
        modelString += "\n# Init vars\n";
        n = len(inits[0]);
        for i in range(0,n):
            hashInits[ inits[0][i] ] = inits[1][i];
            modelString += inits[0][i] + " = " + str(inits[1][i]) + "\n";
    
    for j in N[0]:
        if not hashInits.has_key(j):
            modelString += j + " = 0.0\n";

  #parameters -- remove unused parameters
    if len(params) == 2 and len(params[0]) == len(params[1]):
        modelString += "\n# Init params\n";
        n = len(params[0]);
        for i in range(0,n):  #for each parameter
            modelString += params[0][i] + " = " + str(params[1][i]) + "\n";

    if len(funcsNames) > 0 and len(funcsNames) == len(funcsAssign):
        modelString += "\n# Forcing functions\n";
        n = len(funcsNames);
        for i in range(0,n):
            modelString += "!F " + funcsNames[i] + " = " + funcsAssign[i] + "\n";

    if len(triggers) > 0 and len(triggers) == len(events):
        modelString += "\n# Events\n";
        n = len(triggers);
        for i in range(0,n):
            modelString += "Event: event" + str(i) + "," + triggers[i] + " , 0 {" + events[i] + "}\n";

    p = re.compile("\^");
    p.sub("**",modelString);
    #return modelString;
  
    mod = pysces.model("model",loader="string",fString=modelString);
    mod.doLoad();  
    return mod;

