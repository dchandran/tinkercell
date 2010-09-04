def totc_items(array):
    n = len(array);
    A = tc_createItemsArray(n);
    for i in range(0, n):
        tc_setItem(A, i, array[i]);

    return A;

def fromtc_items(array):
    n = array.length;
    A = range(0,n);
    for i in range(0, n):
        A[i] = tc_getItem(array,i);

    tc_deleteItemsArray(array);
    return A;

def totc_strings(array):
    n = len(array);
    A = tc_createStringsArray(n);
    for i in range(0, n):
        tc_setString(A, i, array[i]);

    return A;

def fromtc_strings(array):
    n = array.length;
    A = range(0,n);
    for i in range(0, n):
        A[i] = tc_getString(array,i);

    tc_deleteStringsArray(array);
    return A;

def fromMatrix(matrix, row_wise = False):
    n = matrix.rows;
    m = matrix.cols;
    cols = fromStrings(matrix.colnames);
    rows = fromStrings(matrix.rownames);
    if row_wise:
        A = range(0,n);
        for i in range(0, n):
            A[i] = range(0,m);
            for j in range(0,m):
                A[i][j] = tc_getMatrixValue(matrix,i,j);
    else:
        A = range(0,m);
        for i in range(0, m):
            A[i] = range(0,n);
            for j in range(0,n):
                A[i][j] = tc_getMatrixValue(matrix,j,i);

    tc_deleteMatrix(matrix);
    return [rows, cols, A];

def toMatrix(lists, row_wise = False , rows = [], cols = []):
    n = len(lists);
    m = len(lists[0]);
    if row_wise:
        A = tc_createMatrix(n,m);
    else:
        A = tc_createMatrix(m,n);
    for i in range(0, n):
        for j in range(0,m):
            if row_wise:
                setNthValue(matrix,i,j,lists[i][j]);
            else:
                setNthValue(matrix,j,i,lists[i][j]);
    n = len(rows);
    m = len(cols);

    for i in range(0,n):
        tc_setRowName(matrix,i,rows[i]);

    for i in range(0,m):
        setColName(matrix,i,cols[i]);

    return A;

def toHex(r,g,b):
    hexchars = "0123456789ABCDEF0";
    return "#" + hexchars[r / 16] + hexchars[r % 16] + hexchars[g / 16] + hexchars[g % 16] + hexchars[b / 16] + hexchars[b % 16];

def fromTC(x):
    if type(x) == type(tc_createMatrix(0,0)): return fromMatrix(x)
    if type(x) == type(tc_createStringsArray(0)): return fromtc_strings(x)
    if type(x) == type(tc_createItemsArray(0)): return fromtc_items(x)
    return null

def toTC(x):
    if type(x) == type([]) and len(x) > 0 and type(x[0]) == type([]): return toMatrix(x)
    if type(x) == type([]) and len(x) > 0 and type(x[0]) == type('hello'): return totc_strings(x)
    if type(x) == type([]) and len(x) > 0 and str(type(x[0])) == '<type \'SwigPyObject\'>': return totc_items(x)
    return null

