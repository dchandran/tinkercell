def toArrayOfItems(array):
    n = len(array);
    A = newArrayOfItems(n);
    for i in range(0, n):
        setNthItem(A, i, array[i]);

    return A;

def fromArrayOfItems(array):
    n = array.length;
    A = range(0,n);
    for i in range(0, n):
        A[i] = nthItem(array,i);

    deleteArrayOfItems(array);
    return A;

def toArrayOfStrings(array):
    n = len(array);
    A = newArrayOfStrings(n);
    for i in range(0, n):
        setNthString(A, i, array[i]);

    return A;

def fromArrayOfStrings(array):
    n = array.length;
    A = range(0,n);
    for i in range(0, n):
        A[i] = nthString(array,i);

    deleteArrayOfStrings(array);
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
                A[i][j] = getValue(matrix,i,j);
    else:
        A = range(0,m);
        for i in range(0, m):
            A[i] = range(0,n);
            for j in range(0,n):
                A[i][j] = getValue(matrix,j,i);

    deleteMatrix(matrix);
    return [rows, cols, A];

def toMatrix(lists, row_wise = False , rows = [], cols = []):
    n = len(lists);
    m = len(lists[0]);
    if row_wise:
        A = newMatrix(n,m);
    else:
        A = newMatrix(m,n);
    for i in range(0, n):
        for j in range(0,m):
            if row_wise:
                setNthValue(matrix,i,j,lists[i][j]);
            else:
                setNthValue(matrix,j,i,lists[i][j]);
    n = len(rows);
    m = len(cols);

    for i in range(0,n):
        setRowName(matrix,i,rows[i]);

    for i in range(0,m):
        setColName(matrix,i,cols[i]);

    return A;

def toHex(r,g,b):
    hexchars = "0123456789ABCDEF0";
    return "#" + hexchars[r / 16] + hexchars[r % 16] + hexchars[g / 16] + hexchars[g % 16] + hexchars[b / 16] + hexchars[b % 16];

def fromTC(x)
    if type(x) == type(newMatrix(0,0)): return fromMatrix(x)
    if type(x) == type(newArrayOfStrings(0)): return fromArrayOfStrings(x)
    if type(x) == type(newArrayOfItems(0)): return fromArrayOfItems(x)
    return null

def toTC(x)
    if type(x) == type([]) and len(x) > 0 and type(x[0]) == type([]): return toMatrix(x)
    if type(x) == type([]) and len(x) > 0 and type(x[0]) == type('hello'): return toArrayOfStrings(x)
    if type(x) == type([]) and len(x) > 0 and type(x[0]) == type(10): return toArrayOfItems(x)
    return null

