function A = fromTC(arg)
	A = [];
	try  %items array
	    n = arg.length;
		A = zeros(1,n);
		for i=1:n
		    A(i) = tc_getItem(arg,i-1);
		endfor
		tc_deleteItemsArray(arg);
    catch
	    try %strings array
		    n = arg.length;
			A = 1:n;
			for i=1:n
				A(i) = tc_getString(arg,i-1);
			endfor
			tc_deleteStringsArray(arg);
		catch
			try %matrix
				m = M.rows;
				n = M.cols;
				A = zeros(m,n);
				for i=1:m
					for j=1:n
						A(i,j) = tc_getMatrixValue(M,i-1,j-1);
					endfor
				endfor
				tc_deleteMatrix(M);
			catch
			end_try_catch %matrix
		end_try_catch %strings array
	end_try_catch %items array
endfunction

