#
# @file    TestSpecies_newSetters.py
# @brief   Species unit tests for new set function API
#
# @author  Akiya Jouraku (Python conversion)
# @author  Sarah Keating
#
# $Id$
# $HeadURL$
#
# This test file was converted from src/sbml/test/TestSpecies_newSetters.c
# with the help of conversion sciprt (ctest_converter.pl).
#
#<!---------------------------------------------------------------------------
# This file is part of libSBML.  Please visit http://sbml.org for more
# information about SBML, and the latest version of libSBML.
#
# Copyright 2005-2009 California Institute of Technology.
# Copyright 2002-2005 California Institute of Technology and
#                     Japan Science and Technology Corporation.
# 
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation.  A copy of the license agreement is provided
# in the file named "LICENSE.txt" included with this software distribution
# and also available online as http://sbml.org/software/libsbml/license.html
#--------------------------------------------------------------------------->*/
import sys
import unittest
import libsbml

class TestSpecies_newSetters(unittest.TestCase):

  C = None

  def setUp(self):
    self.C = libsbml.Species(1,2)
    if (self.C == None):
      pass    
    pass  

  def tearDown(self):
    self.C = None
    pass  

  def test_Species_setBoundaryCondition1(self):
    i = self.C.setBoundaryCondition(False)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( self.C.getBoundaryCondition() == False )
    i = self.C.setBoundaryCondition(True)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( self.C.getBoundaryCondition() == True )
    pass  

  def test_Species_setCharge1(self):
    i = self.C.setCharge(2)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.C.isSetCharge() )
    self.assert_( self.C.getCharge() == 2 )
    i = self.C.unsetCharge()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetCharge() )
    pass  

  def test_Species_setCharge2(self):
    c = libsbml.Species(2,2)
    i = c.setCharge(4)
    self.assert_( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE )
    self.assertEqual( False, c.isSetCharge() )
    c = None
    pass  

  def test_Species_setCharge3(self):
    c = libsbml.Species(2,1)
    i = c.unsetCharge()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetCharge() )
    c = None
    pass  

  def test_Species_setCompartment1(self):
    i = self.C.setCompartment( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, self.C.isSetCompartment() )
    i = self.C.setCompartment( "")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetCompartment() )
    pass  

  def test_Species_setCompartment2(self):
    i = self.C.setCompartment( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.C.isSetCompartment() )
    i = self.C.setCompartment( "")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetCompartment() )
    pass  

  def test_Species_setConstant1(self):
    i = self.C.setConstant(False)
    self.assert_( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE )
    self.assert_( self.C.getConstant() == False )
    pass  

  def test_Species_setConstant2(self):
    c = libsbml.Species(2,2)
    i = c.setConstant(True)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( c.getConstant() == True )
    c = None
    pass  

  def test_Species_setHasOnlySubstanceUnits1(self):
    i = self.C.setHasOnlySubstanceUnits(False)
    self.assert_( self.C.getHasOnlySubstanceUnits() == False )
    self.assert_( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE )
    pass  

  def test_Species_setHasOnlySubstanceUnits2(self):
    c = libsbml.Species(2,2)
    i = c.setHasOnlySubstanceUnits(False)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( c.getHasOnlySubstanceUnits() == False )
    i = c.setHasOnlySubstanceUnits(True)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( c.getHasOnlySubstanceUnits() == True )
    c = None
    pass  

  def test_Species_setId2(self):
    c = libsbml.Species(2,2)
    i = c.setId( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, c.isSetId() )
    c = None
    pass  

  def test_Species_setId3(self):
    c = libsbml.Species(2,2)
    i = c.setId( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, c.isSetId() )
    self.assert_((  "cell"  == c.getId() ))
    c = None
    pass  

  def test_Species_setId4(self):
    c = libsbml.Species(2,2)
    i = c.setId( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, c.isSetId() )
    self.assert_((  "cell"  == c.getId() ))
    i = c.setId("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetId() )
    c = None
    pass  

  def test_Species_setInitialAmount1(self):
    i = self.C.setInitialAmount(2.0)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( self.C.getInitialAmount() == 2.0 )
    self.assertEqual( True, self.C.isSetInitialAmount() )
    i = self.C.unsetInitialAmount()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetInitialAmount() )
    pass  

  def test_Species_setInitialAmount2(self):
    c = libsbml.Species(2,2)
    i = c.setInitialAmount(4)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( c.getInitialAmount() == 4.0 )
    self.assertEqual( True, c.isSetInitialAmount() )
    i = c.unsetInitialAmount()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetInitialAmount() )
    c = None
    pass  

  def test_Species_setInitialConcentration1(self):
    i = self.C.setInitialConcentration(2.0)
    self.assert_( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE )
    self.assertEqual( False, self.C.isSetInitialConcentration() )
    pass  

  def test_Species_setInitialConcentration2(self):
    c = libsbml.Species(2,2)
    i = c.setInitialConcentration(4)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( c.getInitialConcentration() == 4 )
    self.assertEqual( True, c.isSetInitialConcentration() )
    i = c.unsetInitialConcentration()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetInitialConcentration() )
    c = None
    pass  

  def test_Species_setName1(self):
    i = self.C.setName( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.C.isSetName() )
    i = self.C.unsetName()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetName() )
    pass  

  def test_Species_setName2(self):
    c = libsbml.Species(2,2)
    i = c.setName( "1cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, c.isSetName() )
    i = c.unsetName()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetName() )
    c = None
    pass  

  def test_Species_setName3(self):
    i = self.C.setName( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.C.isSetName() )
    i = self.C.setName("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetName() )
    pass  

  def test_Species_setSpatialSizeUnits1(self):
    i = self.C.setSpatialSizeUnits( "mm")
    self.assert_( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE )
    self.assertEqual( False, self.C.isSetSpatialSizeUnits() )
    pass  

  def test_Species_setSpatialSizeUnits2(self):
    c = libsbml.Species(2,2)
    i = c.setSpatialSizeUnits( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, c.isSetSpatialSizeUnits() )
    c = None
    pass  

  def test_Species_setSpatialSizeUnits3(self):
    c = libsbml.Species(2,2)
    i = c.setSpatialSizeUnits( "mole")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_((  "mole" == c.getSpatialSizeUnits() ))
    self.assertEqual( True, c.isSetSpatialSizeUnits() )
    c = None
    pass  

  def test_Species_setSpatialSizeUnits4(self):
    c = libsbml.Species(2,2)
    i = c.setSpatialSizeUnits("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetSpatialSizeUnits() )
    c = None
    pass  

  def test_Species_setSpeciesType1(self):
    i = self.C.setSpeciesType( "cell")
    self.assert_( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE )
    self.assertEqual( False, self.C.isSetSpeciesType() )
    i = self.C.unsetSpeciesType()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetSpeciesType() )
    pass  

  def test_Species_setSpeciesType2(self):
    c = libsbml.Species(2,2)
    i = c.setSpeciesType( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, c.isSetSpeciesType() )
    i = c.unsetSpeciesType()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetSpeciesType() )
    c = None
    pass  

  def test_Species_setSpeciesType3(self):
    c = libsbml.Species(2,2)
    i = c.setSpeciesType( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, c.isSetSpeciesType() )
    self.assert_((  "cell"  == c.getSpeciesType() ))
    i = c.unsetSpeciesType()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetSpeciesType() )
    c = None
    pass  

  def test_Species_setSpeciesType4(self):
    c = libsbml.Species(2,2)
    i = c.setSpeciesType( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, c.isSetSpeciesType() )
    self.assert_((  "cell"  == c.getSpeciesType() ))
    i = c.setSpeciesType("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetSpeciesType() )
    c = None
    pass  

  def test_Species_setSubstanceUnits1(self):
    i = self.C.setSubstanceUnits( "mm")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.C.isSetSubstanceUnits() )
    pass  

  def test_Species_setSubstanceUnits2(self):
    c = libsbml.Species(2,2)
    i = c.setSubstanceUnits( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, c.isSetSubstanceUnits() )
    c = None
    pass  

  def test_Species_setSubstanceUnits3(self):
    c = libsbml.Species(2,2)
    i = c.setSubstanceUnits( "mole")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_((  "mole" == c.getSubstanceUnits() ))
    self.assertEqual( True, c.isSetSubstanceUnits() )
    c = None
    pass  

  def test_Species_setSubstanceUnits4(self):
    c = libsbml.Species(2,2)
    i = c.setSubstanceUnits( "mole")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_((  "mole" == c.getSubstanceUnits() ))
    self.assertEqual( True, c.isSetSubstanceUnits() )
    i = c.setSubstanceUnits("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, c.isSetSubstanceUnits() )
    c = None
    pass  

  def test_Species_setUnits1(self):
    i = self.C.setUnits( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, self.C.isSetUnits() )
    i = self.C.unsetUnits()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetUnits() )
    pass  

  def test_Species_setUnits2(self):
    i = self.C.setUnits( "litre")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.C.isSetUnits() )
    i = self.C.unsetUnits()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetUnits() )
    pass  

  def test_Species_setUnits3(self):
    i = self.C.setUnits( "litre")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.C.isSetUnits() )
    i = self.C.setUnits("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.C.isSetUnits() )
    pass  

def suite():
  suite = unittest.TestSuite()
  suite.addTest(unittest.makeSuite(TestSpecies_newSetters))

  return suite

if __name__ == "__main__":
  if unittest.TextTestRunner(verbosity=1).run(suite()).wasSuccessful() :
    sys.exit(0)
  else:
    sys.exit(1)