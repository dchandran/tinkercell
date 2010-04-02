try:
    from tinkercell import *

    def toItems(array):
        n = len(array);
        A = newArrayOfItems(n);
        for i in range(0, n):
            setNthItem(A, array[i]);

        return A;
    
    def fromItems(array):
        n = array.length;
        array = range(0,n);
        for i in range(0, n):
            A[i] = nthItem(array,i);

        delete(array);
        return A;

    def toStrings(array):
        n = len(array);
        A = newArrayOfStrings(n);
        for i in range(0, n):
            setNthString(A, array[i]);

        return A;
    
    def fromStrings(array):
        n = array.length;
        array = range(0,n);
        for i in range(0, n):
            A[i] = nthString(array,i);

        delete(array);
        return A;

    def fromMatrix(matrix, row_wise = False):
        n = matrix.rows;
        m = matrix.cols;
        cols = fromStrings(m.colnames);
        rows = fromStrings(m.rownames);
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
except ImportError:
    print 'TinkerCell Python module not found'

try:
    from numpy import *
    from scipy import *
    print 'Python modules loaded: Numpy and Scipy'
except ImportError:
    print 'Numpy and Scipy were not found'

try:
    from pysces import *
    import networkx as nx
    from nxAutoLayout import *
    from tc2pysces import *
    print 'Python modules loaded: PySCeS, NetworkX. Edit init.py to add more'
except ImportError:
    print 'PySCeS and/or NetworkX were not found'

