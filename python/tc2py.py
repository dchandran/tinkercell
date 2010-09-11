from tinkercell import  *

def to_tc_items(array):
    n = len(array);
    A = tc_createItemsArray(n);
    for i in range(0, n):
        tc_setItem(A, i, array[i]);

    return A;

def from_tc_items(array):
    n = array.length;
    A = range(0,n);
    for i in range(0, n):
        A[i] = tc_getItem(array,i);

    tc_deleteItemsArray(array);
    return A;

def to_tc_strings(array):
    n = len(array);
    A = tc_createStringsArray(n);
    for i in range(0, n):
        tc_setString(A, i, array[i]);

    return A;

def from_tc_strings(array):
    n = array.length;
    A = range(0,n);
    for i in range(0, n):
        A[i] = tc_getString(array,i);

    tc_deleteStringsArray(array);
    return A;

def from_tc_matrix(matrix, row_wise = False):
    n = matrix.rows;
    m = matrix.cols;
    cols = from_tc_strings(matrix.colnames);
    rows = from_tc_strings(matrix.rownames);
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

def to_tc_matrix(lists, row_wise = False , rows = [], cols = []):
    n = len(lists);
    m = len(lists[0]);
    if row_wise:
        A = tc_createMatrix(n,m);
    else:
        A = tc_createMatrix(m,n);
    for i in range(0, n):
        for j in range(0,m):
            if row_wise:
                tc_setMatrixValue(matrix,i,j,lists[i][j]);
            else:
                tc_setMatrixValue(matrix,j,i,lists[i][j]);
    n = len(rows);
    m = len(cols);

    for i in range(0,n):
        tc_setRowName(matrix,i,rows[i]);

    for i in range(0,m):
        tc_setColName(matrix,i,cols[i]);

    return A;

def toHex(r,g,b):
    hexchars = "0123456789ABCDEF0";
    return "#" + hexchars[r / 16] + hexchars[r % 16] + hexchars[g / 16] + hexchars[g % 16] + hexchars[b / 16] + hexchars[b % 16];

def fromTC(x):
    if type(x) == type(tc_createMatrix(0,0)): return from_tc_matrix(x)
    if type(x) == type(tc_createStringsArray(0)): return from_tc_strings(x)
    if type(x) == type(tc_createItemsArray(0)): return from_tc_items(x)
    return null

def toTC(x):
    if type(x) == type([]) and len(x) > 0 and type(x[0]) == type([]): return to_tc_matrix(x)
    if type(x) == type([]) and len(x) > 0 and type(x[0]) == type('hello'): return to_tc_strings(x)
    if type(x) == type([]) and len(x) > 0 and str(type(x[0])) == '<type \'SwigPyObject\'>': return to_tc_items(x)
    return null

