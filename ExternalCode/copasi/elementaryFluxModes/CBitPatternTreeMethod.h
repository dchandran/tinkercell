// Begin CVS Header
//   $Source: /fs/turing/cvs/copasi_dev/copasi/elementaryFluxModes/CBitPatternTreeMethod.h,v $
//   $Revision: 1.7 $
//   $Name: Build-33 $
//   $Author: shoops $
//   $Date: 2010/02/02 18:09:36 $
// End CVS Header

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#ifndef COPASI_CBitPatternTreeMethod
#define COPASI_CBitPatternTreeMethod

#include <vector>

#include "copasi/elementaryFluxModes/CEFMMethod.h"
#include "copasi/elementaryFluxModes/CStepMatrix.h"
#include "copasi/utilities/CMatrix.h"

class CStepMatrix;
class CStepMatrixColumn;
class CReaction;
class CBitPatternTree;
class CBitPatternTreeNode;
class CZeroSet;

class CBitPatternTreeMethod: public CEFMMethod
{
  friend CEFMMethod * CEFMMethod::createMethod(CCopasiMethod::SubType subType);

public:
  /**
   * A static method that calculates the kernel of a full column rank matrix.
   * Note, the input matrix is used as work area and will be modified during the calculation.
   * @param const CMatrix< C_INT64 > & matrix
   * @param CMatrix< C_INT64 > & kernel
   * @param CVector< size_t > & rowPivot
   * @return bool success
   */
  static bool CalculateKernel(CMatrix< C_INT64 > & matrix,
                              CMatrix< C_INT64 > & kernel,
                              CVector< size_t > & rowPivot);

  /**
   * Calculate the greatest common divisor (GCD) of 2 positive integers. On return
   * m and n contain the GCD
   * @param C_INT64 & m
   * @param C_INT64 & n
   */
  static inline void GCD(C_INT64 & m, C_INT64 & n)
  {
    assert(m > 0 && n > 0);

    while (m != n)
      {
        if (m > n)
          {
            m %= n;

            if (m == 0)
              {
                m = n;
              }
          }
        else
          {
            n %= m;

            if (n == 0)
              {
                n = m;
              }
          }
      }
  }

protected:
  /**
   * Default constructor
   * @param const CCopasiContainer * pParent (Default: NULL)
   */
  CBitPatternTreeMethod(const CCopasiContainer * pParent = NULL);

  /**
   * Constructor to be called by derived methods
   * @param const CCopasiMethod::SubType subType
   * @param const CCopasiContainer * pParent (Default: NULL)
   */
  CBitPatternTreeMethod(const CCopasiMethod::SubType subType,
                        const CCopasiContainer * pParent = NULL);

public:
  /**
   * Copy Constructor
   * @param const CBitPatternTreeMethod & src
   */
  CBitPatternTreeMethod(const CBitPatternTreeMethod & src,
                        const CCopasiContainer * pParent = NULL);

  /**
  *  Destructor
  */
  ~CBitPatternTreeMethod();

  /**
   * Execute the optimization algorithm calling simulation routine
   * when needed. It is noted that this procedure can give feedback
   * of its progress by the callback function set with SetCallback.
   * @ return success;
   */
  virtual bool calculate();

  /**
   * Initialize arrays and pointer.
   * @return bool success
   */
  virtual bool initialize();

private:
  /**
   * Initialize the needed CCopasiObjects.
   */
  void initObjects();

  /**
   * Construct the kernel matrix
   * @param CMatrix< C_FLOAT64> & kernel
   */
  void buildKernelMatrix(CMatrix< C_INT64 > & kernel);

  /**
   * Create all possible linear combinations of the bit pattern nodes pPositive
   * and pNegative and all their child nodes.
   * @param const CBitPatternTreeNode * pPositive
   * @param const CBitPatternTreeNode * pNegative
   */
  void combine(const CBitPatternTreeNode * pPositive,
               const CBitPatternTreeNode * pNegative);

  /**
   * Remove the invalid columns from the step matrix
   * @param const std::vector< CStepMatrix::iterator > & nullColumns
   */
  void findRemoveInvalidColumns(const std::vector< CStepMatrixColumn * > & nullColumns);

  /**
   * Postprocess the step matrix to construct the flux modes.
   */
  void buildFluxModes();

  /**
   * Add the flux mode to the list only if the reversed does not exist
   * @param const CFluxMode & mode
   */
  void addMode(const CFluxMode & mode);

  /**
   * Multiply values so that values contains only integers.
   */
  // void convertToIntegers(CMatrix< C_FLOAT64 > & values);

  /**
   * Get the index of the unset bits of the flux mode.
   */
  void getUnsetBitIndexes(const CStepMatrixColumn * pColumn,
                          CVector< size_t > & indexes) const;

  // Attributes
protected:
  /**
   * A pointer to the model which is analyzed.
   */
  CModel * mpModel;

  /**
   * The current step used for process report.
   */
  unsigned C_INT32 mProgressCounter;

  /**
   * The max step used for process report.
   */
  unsigned C_INT32 mProgressCounterMax;

  /**
   * Handle to the process report item "Current Step"
   */
  unsigned C_INT32 mhProgressCounter;

  /**
   * The current combination used for process report.
   */
  unsigned C_INT32 mProgressCounter2;

  /**
   * The max combination used for process report.
   */
  unsigned C_INT32 mProgressCounter2Max;

  /**
   * Handle to the process report item "Combination"
   */
  unsigned C_INT32 mhProgressCounter2;

  /**
   * A vector to recording the expansion of the stoichiometry matrix.
   */
  std::vector< std::pair< size_t, bool > > mReactionForward;

  /**
   * A vector recording the pivots for the QR factorization
   */
  CVector< size_t > mReactionPivot;

  /**
   * The transpose of the expanded stoichiometry matrix.
   */
  CMatrix< C_INT64 > mExpandedStoiTranspose;

  /**
   * A pointer to the step matrix for creating the flux modes
   */
  CStepMatrix * mpStepMatrix;

  /**
   * The bit pattern tree for the current step
   */
  CBitPatternTree * mpNullTree;

  /**
   * A list of invalid columns currently in the step matrix
   */
  std::vector< CStepMatrixColumn * > mNewColumns;

  /**
   * The minimum set size use to determine whether a linear combination is allowed.
   */
  size_t mMinimumSetSize;

  /**
   * The currently process step
   */
  size_t mStep;

  /**
   * Boolean value indicating whether combination should continue.
   */
  bool mContinueCombination;
};

#endif // COPASI_CBitPatternTreeMethod
