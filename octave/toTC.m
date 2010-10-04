function A = toTC(arg)
	try  %items array
	    n = length(arg);
    	A = tc_createItemsArray(n);
    	for i =1:n
    	    tc_setItem(A, i-1, arg(i));
    	endfor
    catch
	    try %strings array
		    n = length(arg);
		    A = tc_createStringsArray(n);
		    for i=1:n
		        tc_setString(A, i-1, arg(i));
    		endfor
		catch
			try %matrix
				m = size(M,1);
			    n = size(M,2);
			    A = tc_createMatrix(m,n);
			    for i=1:m
			        for j=1:n
            			tc_setMatrixValue(matrix,i-1,j-1,M(i,j));
			        endfor
			    endfor
			catch
				A = arg;
			end_try_catch %matrix
		end_try_catch %strings array
	end_try_catch %items array
endfunction

