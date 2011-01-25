///
/// This file contains several useful utility functions
///
///

#ifndef SBW_CLAPACK_UTIL_H
#define SBW_CLAPACK_UTIL_H

#include "libla.h"

#define DELETE_2D_ARRAY(oArray, nLength)\
	{\
		for (int i=0; i<nLength; i++) {\
			delete[] oArray[i];\
		}\
		delete[] oArray;\
		oArray = NULL;\
	}

#define FREE_2D_ARRAY(oArray, nLength)\
	{\
		for (int i=0; i<nLength; i++) {\
			free(oArray[i]);\
		}\
		free(oArray);\
		oArray = NULL;\
	}

#define CREATE_ARRAY(variableName,type,length)\
	if(variableName) { delete[] variableName; variableName = NULL;}\
	variableName = new type[length]; memset(variableName, 0, sizeof(type)*length);//

#define DELETE_ARRAY_IF_NON_NULL(target)\
	if(target) { delete[] target; target = NULL;}
#define DELETE_IF_NON_NULL(target)\
	if(target) { delete target; target = NULL;}


#include <string>

//#include "f2c.h"

namespace LIB_LA
{

	class ApplicationException
	{

	private:
		std::string _Message;
		std::string _DetailedMessage;

	public: 
		ApplicationException() : _Message(""), _DetailedMessage("") {}
		ApplicationException(const std::string &message) : _Message(message), _DetailedMessage("") {}
		ApplicationException(const std::string &message, const std::string &detailedMessage) : 
		_Message(message), _DetailedMessage(detailedMessage) {}

		std::string getMessage() { return _Message; }
		std::string getDetailedMessage() { return _DetailedMessage; } 

	};
	

	class Util
	{
	public:

		static int** matMult(int mA, int nA, int **A, int **B, int nB);
		static double** matMult(int mA, int nA, double **A, double **B, int nB);
		static void checkTolerance(int nrows, double *A, double dTolerance);
		static void checkTolerance(int nrows, int ncols, double **A, double dTolerance);
		static DoubleMatrix* getSubMatrix(int Mb, int Nb, int ms, int ns, int mi, int nj, DoubleMatrix& A);
		static DoubleMatrix* matMult(unsigned int mA, unsigned int nA, DoubleMatrix &A, DoubleMatrix &B, unsigned int nB);
		static DoubleMatrix* matMult(DoubleMatrix &A, DoubleMatrix &B);
		static DoubleMatrix* matMult(IntMatrix &A, DoubleMatrix &B);
		static IntMatrix* matMult(IntMatrix &A, IntMatrix &B);		

		static void CopyMatrix(DoubleMatrix& oMatrix, double** &outMatrix, int &outNumRows, int &outNumCols);
		static void CopyMatrix(IntMatrix& oMatrix, int** &outMatrix, int &outNumRows, int &outNumCols);
		static void CopyMatrix(ComplexMatrix& oMatrix, double** &outMatrixReal,double** &outMatrixImag, int &outNumRows, int &outNumCols);
		static void CopyStringVector(const std::vector< std::string > &vector, char** &outVector, int &outLength);
		static void CopyDoubleVector(const std::vector< double > &vector, double* &outVector, int &outLength);
		static void CopyIntVector(const std::vector< int > &vector, int* &outVector, int &outLength);
		static void CopyComplexVector(const std::vector< Complex> &vector, double* &outVectorReal, double* &outVectorImag, int &outLength);

		static void RoundMatrixToTolerance(DoubleMatrix& oMatrix, double dTolerance);
		static double RoundToTolerance(double dValue, double dTolerance);

		static std::vector<int> GaussJordan(DoubleMatrix &oMatrix, double dTolerance);
		static void FullyPivotedGaussJordan(DoubleMatrix &oMatrix, double dTolerance, std::vector< int > &rowPivots, std::vector< int > &colPivots);

		static void gaussJordan(DoubleMatrix &oMatrix, double dTolerance);
		static int findRank(DoubleMatrix &oMatrix, double dTolerance);

		static void print(int mr, int nc, int *A);
		//static void print(int mr, int nc, integer *A);
		static void print(int mr, int nc, int **A);
		static void print(int mr, int nc, double *A);
		static void print(int mr, int nc, double **A);
		static void print(IntMatrix& A);
		static void print(DoubleMatrix& A);
		//static void print(int mr, int nc, doublecomplex *A);
		//static void print(int mr, int nc, doublecomplex **A);
		static void print(int mr, int nc, LIB_LA::Complex *A);
		static void print(int mr, int nc, LIB_LA::Complex **A);
		static void print(int mr, int nc, int *A, int *B);
		static void print(int mr, int nc, int **A, int **B);
		static void print(int mr, int nc, double **A, double **B);
	};
}

#endif
