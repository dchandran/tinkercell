#include "mathfunc.h"

/* finds the eigenvalues of a matrix using CLAPACK
 * @param: square matrix
 * @param: matrix dimension
 * @param: (output) vector of real values
 * @param: (output) vector of imaginary values
 * @return: 0 = failure 1 = success
*/
int eigenvalues(double * A, int N, double ** reals, double ** im)
{
    int ret = 1;
    char jobv_ = 'N';
    doublereal *wi,*wr,*U,*work,work_size,*Ui,*D;
    integer lwork,info;
    integer n = (integer)N;

    wi = (doublereal*) malloc( n*sizeof(doublereal));
    wr = (doublereal*) malloc( n*sizeof(doublereal));
    U  = (doublereal*) calloc( n*n,sizeof(doublereal));
    Ui = (doublereal*) calloc( n*n,sizeof(doublereal));
    lwork = -1;

    dgeev_(&jobv_,&jobv_,&n,A,&n,wr,wi,U,&n,Ui,&n,&work_size,&lwork,&info);

    if (info == 0)
    {
       lwork = (integer)work_size;
       work  = (doublereal*) calloc( lwork , sizeof( doublereal) ); 
       dgeev_(&jobv_,&jobv_,&n,A,&n,wr,wi,U,&n,Ui,&n,work,&lwork,&info);

      if (info == 0)
      {
         (*reals) = (double*)wr;
         (*im) = (double*)wi;
      }
      else ret = 0;
    }
    else ret = 0;

    if (ret == 0) 
    {
       free(wi);
       free(wr);
    }
    free(U);
    free(Ui);
    free(work);
    return (ret);
}


/* calculates correlation between two vectors
 * @param: first vector of doubles
 * @param: second vector of doubles
 * @param: size of both vectors
 * @return: covariance
*/
double correlation(double * X, double * Y, int sz)
{
   int i;
   double d, ans;
   double mXY = 0,mX = 0, mY = 0, mX2 = 0, mY2 = 0;

   if (X == 0 || Y == 0) return (0.0);
   
   for (i = 0; i < sz; ++i)
   {
        mX += X[i];
        mY += Y[i];
        mXY += X[i] * Y[i];
        mX2 += X[i]*X[i];
        mY2 += Y[i]*Y[i];
   }
   mX /= (double)sz;
   mY /= (double)sz;
   mXY /= (double)sz;
   mX2 /= (double)sz;
   mY2 /= (double)sz;

   d = sqrt(mX2 - mX*mX) * sqrt(mY2 - mY*mY);

   if (d == 0.0) return (0.0);

   ans = (mXY - mX*mY)/d;
   return ((double)ans);
}

/* calculates correlation between two columns of two (or the same) matrix
 * @param: first matrix (single array)
 * @param: second matrix (since array)
 * @param: column of first matrix
 * @param: column of second matrix
 * @param: number of columns in first matrix
 * @param: number of columns in second matrix
 * @param: number of rows in both matrices
 * @return: covariance
*/
double colCorrelation(double * M1, double * M2, int colsM1, int colsM2, int iM1, int iM2, int sz)
{
   int i;

   if (M1 == 0 || M2 == 0) return (0.0);
   double mXY = 0,mX = 0, mY = 0, mX2 = 0, mY2 = 0;
   for (i = 0; i < sz; ++i)
   {
        mX += getValue(M1, colsM1, i, iM1);
        mY += getValue(M2, colsM2, i, iM2);
        mXY += getValue(M1, colsM1, i, iM1) * getValue(M2, colsM2, i, iM2);
        mX2 += getValue(M1, colsM1, i, iM1) * getValue(M1, colsM1, i, iM1);
        mY2 += getValue(M2, colsM2, i, iM2) * getValue(M2, colsM2, i, iM2);
   }
   mX /= (double)sz;
   mY /= (double)sz;
   mXY /= (double)sz;
   mX2 /= (double)sz;
   mY2 /= (double)sz;

   double d = sqrt(mX2 - mX*mX) * sqrt(mY2 - mY*mY);
   if (d == 0.0) return (0.0);
   return ((mXY - mX*mY)/d);
}
