function A = fromTC(arg)
	argtyp = swig_type(arg);
	if (length(argtyp) == 8)
		if (argtyp(1:8) == 'tc_items')
			n = arg.length;
			A = zeros(1,n);
			for i=1:n
				A(i) = tinkercell.tc_getItem(arg,i-1);
			endfor
			tinkercell.tc_deleteItemsArray(arg);
		endif
    else
	    if (length(argtyp) == 10)
	    	if (argtyp(1:10) == 'tc_strings')
				n = arg.length;
				A = 1:n;
				for i=1:n
					A(i) = tinkercell.tc_getString(arg,i-1);
				endfor
				tinkercell.tc_deleteStringsArray(arg);
			endif
		else
			if (length(argtyp) == 9)
				if (argtyp == 'tc_matrix')
					m = arg.rows;
					n = arg.cols;
					A = zeros(m,n);
					for i=1:m
						for j=1:n
							A(i,j) = tinkercell.tc_getMatrixValue(arg,i-1,j-1);
						endfor
					endfor
					tinkercell.tc_deleteMatrix(arg);
				endif
			else
				A = arg;
			endif %matrix
		endif %strings array
	endif %items array
endfunction

