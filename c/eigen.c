#include "eigen.h"

/* finds the eigenvalues of a matrix using CLAPACK
 * \param: square matrix
 * \param: matrix dimension
 * \param: (output) vector of real values
 * \param: (output) vector of imaginary values
 * \return: 0 = failure 1 = success
*/
int eigenvalues(double * A, int N, double * wr, double * wi)
{
    int ret = 1;
    char jobv_ = 'N';
    doublereal *U,*work,work_size,*Ui,*D;
    integer lwork,info;
    integer n = (integer)N;

    //wi = (doublereal*) malloc( n*sizeof(doublereal));
    //wr = (doublereal*) malloc( n*sizeof(doublereal));
    U  = (doublereal*) calloc( n*n,sizeof(doublereal));
    Ui = (doublereal*) calloc( n*n,sizeof(doublereal));
    lwork = -1;

    dgeev_(&jobv_,&jobv_,&n,A,&n,wr,wi,U,&n,Ui,&n,&work_size,&lwork,&info);

    if (info == 0)
    {
       lwork = (integer)work_size;
       work  = (doublereal*) calloc( lwork , sizeof( doublereal) ); 
       dgeev_(&jobv_,&jobv_,&n,A,&n,wr,wi,U,&n,Ui,&n,work,&lwork,&info);

      /*if (info == 0)
      {
         (*reals) = (double*)wr;
         (*im) = (double*)wi;
      }
      else ret = 0;*/
    }
    else ret = 0;

    /*if (ret == 0) 
    {
       free(wi);
       free(wr);
    }*/
    free(U);
    free(Ui);
    free(work);
    return (ret);
}
