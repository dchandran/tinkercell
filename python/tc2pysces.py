def getPyscesModel():
    from tinkercell import *
    import pysces
    import re

    A = tc_allItems();
    N = fromMatrix( tc_getStoichiometry(A), True );
    rates0 = fromStrings( tc_getRates(A) );
    params = fromMatrix( tc_getParameters(A) );
    fixed = fromMatrix( tc_getFixedVariables(A) );
    inits = fromMatrix( tc_getInitialValues(A) );
    funcsNames = fromStrings( tc_getForcingFunctionNames(A) );
    funcsAssign0 = fromStrings( tc_getForcingFunctionAssignments(A) );
    triggers = fromStrings(  tc_getEventTriggers() );
    events0 = fromStrings( tc_getEventResponses() );
    tc_deleteItemsArray(A);
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

    n = len(fixed[0]);
    if n > 0 and len(fixed[0]) == len(fixed[2][0]):
        if not emptyExists:
            fix += "FIX:";
        for i in range(0,n):
            fix += " " + fixed[0][0][i];

    modelString = fix + "\n\n" + modelString;

    modelString += "# Init ext\n";

    #fixed variables
    if n > 0 and len(fixed[0]) == len(fixed[2][0]):
        for i in range(0,n):
            modelString += fixed[0][i] + " = " + str(fixed[2][0][i]) + "\n";

    #initial variables
    hashInits = {};
    n = len(inits[0]);
    if n > 0 and len(inits[0]) == len(inits[2][0]):
        modelString += "\n# Init vars\n";    
        for i in range(0,n):
            hashInits[ inits[0][i] ] = inits[2][0][i];
            modelString += inits[0][i] + " = " + str(inits[2][0][i]) + "\n";

    for j in N[0]:
        if not hashInits.has_key(j):
            modelString += j + " = 0.0\n";

    #parameters -- remove unused parameters
    n = len(params[0]);
    if n > 0 and len(params[0]) == len(params[2][0]):
        modelString += "\n# Init params\n";
        for i in range(0,n):  #for each parameter
            modelString += params[0][i] + " = " + str(params[2][0][i]) + "\n";

    n = len(funcsNames);
    if n > 0 and len(funcsNames) == len(funcsAssign):
        modelString += "\n# Forcing functions\n";
        for i in range(0,n):
            modelString += "!F " + funcsNames[i] + " = " + funcsAssign[i] + "\n";
            
    n = len(triggers);
    if n > 0 and len(triggers) == len(events):
        modelString += "\n# Events\n";
        for i in range(0,n):
            modelString += "Event: event" + str(i) + "," + triggers[i] + " , 0 {" + events[i] + "}\n";

    p = re.compile("\^");
    p.sub("**",modelString);
    #return modelString;

    mod = pysces.model("model",loader="string",fString=modelString);
    mod.doLoad();  
    return mod;

