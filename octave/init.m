function A = toItems(array)
    n = length(array);
    A = tc_createItemsArray(n);
    for i =1:n
        tc_setItem(A, i-1, array(i));
    endfor
    return A;
endfunction

function A = toItems(array)
    n = array.length;
    A = 1:n;
    for i=1:n
        A(i) = tc_getItem(array,i-1);
    endfor
    tc_deleteItemsArray(array);
    return A;
endfunction

function A = toStrings(array)
    n = length(array);
    A = tc_createStringsArray(n);
    for i=1:n
        tc_setString(A, i-1, array(i));
    endfor
    return A;
endfunction

function A = fromStrings(array)
    n = array.length;
    A = 1:n;
    for i=1:n
        A(i) = tc_getString(array,i-1);
    endfor
    tc_deleteStringsArray(array);
    return A;
endfunction

function A = fromMatrix(M)
    m = M.rows;
    n = M.cols;
    A = zeros(m,n);
    for i=1:m
        for j=1:n
            A(i,j) = tc_getMatrixValue(M,i-1,j-1);
        endfor
    endfor
    tc_deleteMatrix(M);
    return A;
endfunction

function A = toMatrix(M)
    m = size(M,1);
    n = size(M,2);
    A = tc_createMatrix(m,n);
    for i=1:m
        for j=1:n
            tc_setMatrixValue(matrix,i-1,j-1,M(i,j));
        endfor
    endfor
    return A;
endfunction

