/****************************************************************************
 **
 ** Copyright (C) 2008 Deepak Chandran
 ** Contact: Deepak Chandran (dchandran1@gmail.com)
 **
 ****************************************************************************/
 
 #include "correlation.h"

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
   return ans;
}

double maxCorrelation(double * X, double * Y, int sz, int minSz)
{
   int i, j, k;
   double r, r_best;
   double mXY = 0, mX = 0, mY = 0, mX2 = 0, mY2 = 0;

   if (X == 0 || Y == 0) return (0.0);
   
   r_best = -2.0;
   
   for (i=minSz; i <= sz; ++i)
   {
		r = correlation((X + (sz-i-1)),Y,i);
		//printf("%i %lf\n",i,r);
		
		if (r > r_best)
			r_best = r;
   }
   
   for (i=minSz; i <= sz; ++i)
   {
		r = correlation((Y + (sz-i-1)),X,i);
		//printf("%i %lf\n",i,r);
		
		if (r > r_best)
			r_best = r;
   }
   
   return r_best;
}

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

/*
int main()
{
	double X[] = {1,2,3,4,5,6,7,8,9,10};
	double Y[] = {1,1,1,1,2,3,4,5,6,7};
	
	printf( "%lf\n", maxCorrelation(X,Y,10,4) );
	return 0;
}
*/
