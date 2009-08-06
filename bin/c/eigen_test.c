#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mathfunc.h"

int main()
{
   // --------------------------- test values
    double A[] =
            {0,1,2,3,
             1,2,3,4,
             1,3,4,5,
             1,1,1,1 };
    int n = 4;
    // ---------------------------------------------
    double * rl = 0, * im = 0;
    int k = eigenvalues(A,n,&rl,&im);
    if (k)
    {
        int i;
        for (i=0; i < n; ++i) printf("%lf + %lf i\n",rl[i],im[i]); 
    }

    double y1[10] = {1,2,3,4,5,6,7,8,9,10};
    double y2[10] = {2,6,6,8,10,16,17,18,23,30};

    printf("corr = %lf \n",correlation(y1,y2,10));

    return(0);
}
