#
# @file    TestSpeciesReference_newSetters.py
# @brief   SpeciesReference unit tests for new set function API
#
# @author  Akiya Jouraku (Python conversion)
# @author  Sarah Keating 
#
# $Id$
# $HeadURL$
#
# ====== WARNING ===== WARNING ===== WARNING ===== WARNING ===== WARNING ======
#
# DO NOT EDIT THIS FILE.
#
# This file was generated automatically by converting the file located at
# src/sbml/test/TestSpeciesReference_newSetters.c
# using the conversion program dev/utilities/translateTests/translateTests.pl.
# Any changes made here will be lost the next time the file is regenerated.
#
# -----------------------------------------------------------------------------
# This file is part of libSBML.  Please visit http://sbml.org for more
# information about SBML, and the latest version of libSBML.
#
# Copyright 2005-2010 California Institute of Technology.
# Copyright 2002-2005 California Institute of Technology and
#                     Japan Science and Technology Corporation.
# 
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation.  A copy of the license agreement is provided
# in the file named "LICENSE.txt" included with this software distribution
# and also available online as http://sbml.org/software/libsbml/license.html
# -----------------------------------------------------------------------------

import sys
import unittest
import libsbml


class TestSpeciesReference_newSetters(unittest.TestCase):

  global sr
  sr = None

  def setUp(self):
    self.sr = libsbml.SpeciesReference(2,4)
    if (self.sr == None):
      pass    
    pass  

  def tearDown(self):
    _dummyList = [ self.sr ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setDenominator1(self):
    i = self.sr.setDenominator(2)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( self.sr.getDenominator() == 2 )
    pass  

  def test_SpeciesReference_setDenominator2(self):
    c = libsbml.SpeciesReference(2,2)
    i = c.setDenominator(4)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( c.getDenominator() == 4 )
    _dummyList = [ c ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setId1(self):
    i = self.sr.setId( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetId() )
    self.assert_((  "cell"  == self.sr.getId() ))
    pass  

  def test_SpeciesReference_setId2(self):
    i = self.sr.setId( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, self.sr.isSetId() )
    pass  

  def test_SpeciesReference_setId3(self):
    c = libsbml.SpeciesReference(2,1)
    i = c.setId( "cell")
    _dummyList = [ c ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setId4(self):
    i = self.sr.setId( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetId() )
    self.assert_((  "cell"  == self.sr.getId() ))
    i = self.sr.setId("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetId() )
    pass  

  def test_SpeciesReference_setName1(self):
    i = self.sr.setName( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetName() )
    i = self.sr.unsetName()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetName() )
    pass  

  def test_SpeciesReference_setName2(self):
    i = self.sr.setName( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, self.sr.isSetName() )
    i = self.sr.unsetName()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetName() )
    pass  

  def test_SpeciesReference_setName3(self):
    c = libsbml.SpeciesReference(2,1)
    i = c.setName( "cell")
    self.assert_( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE )
    self.assertEqual( False, c.isSetName() )
    _dummyList = [ c ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setName4(self):
    i = self.sr.setName( "cell")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetName() )
    i = self.sr.setName("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetName() )
    pass  

  def test_SpeciesReference_setSpecies1(self):
    i = self.sr.setSpecies( "mm")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetSpecies() )
    pass  

  def test_SpeciesReference_setSpecies2(self):
    c = libsbml.SpeciesReference(2,2)
    i = c.setSpecies( "1cell")
    self.assert_( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE )
    self.assertEqual( False, c.isSetSpecies() )
    _dummyList = [ c ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setSpecies3(self):
    c = libsbml.SpeciesReference(2,2)
    i = c.setSpecies( "mole")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_((  "mole" == c.getSpecies() ))
    self.assertEqual( True, c.isSetSpecies() )
    _dummyList = [ c ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setSpecies4(self):
    i = self.sr.setSpecies( "mm")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetSpecies() )
    i = self.sr.setSpecies("")
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetSpecies() )
    pass  

  def test_SpeciesReference_setStoichiometry1(self):
    i = self.sr.setStoichiometry(2.0)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( self.sr.getStoichiometry() == 2.0 )
    pass  

  def test_SpeciesReference_setStoichiometry2(self):
    c = libsbml.SpeciesReference(2,2)
    i = c.setStoichiometry(4)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( c.getStoichiometry() == 4.0 )
    _dummyList = [ c ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setStoichiometryMath1(self):
    sm = libsbml.StoichiometryMath(2,4)
    math = libsbml.ASTNode(libsbml.AST_TIMES)
    a = libsbml.ASTNode()
    b = libsbml.ASTNode()
    a.setName( "a")
    b.setName( "b")
    math.addChild(a)
    math.addChild(b)
    sm.setMath(math)
    i = self.sr.setStoichiometryMath(sm)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetStoichiometryMath() )
    self.assert_( self.sr.getStoichiometry() == 1 )
    i = self.sr.unsetStoichiometryMath()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetStoichiometryMath() )
    _dummyList = [ sm ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setStoichiometryMath2(self):
    sm = libsbml.StoichiometryMath(2,4)
    math = libsbml.ASTNode(libsbml.AST_TIMES)
    a = libsbml.ASTNode()
    a.setName( "a")
    math.addChild(a)
    sm.setMath(math)
    i = self.sr.setStoichiometryMath(sm)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetStoichiometryMath() )
    _dummyList = [ sm ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setStoichiometryMath3(self):
    i = self.sr.setStoichiometryMath(None)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetStoichiometryMath() )
    i = self.sr.unsetStoichiometryMath()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetStoichiometryMath() )
    pass  

  def test_SpeciesReference_setStoichiometryMath4(self):
    sm = libsbml.StoichiometryMath(2,4)
    math = None
    sm.setMath(math)
    i = self.sr.setStoichiometryMath(sm)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( True, self.sr.isSetStoichiometryMath() )
    self.assert_( self.sr.getStoichiometry() == 1 )
    i = self.sr.unsetStoichiometryMath()
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assertEqual( False, self.sr.isSetStoichiometryMath() )
    _dummyList = [ sm ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setStoichiometryMath5(self):
    sr1 = libsbml.SpeciesReference(1,2)
    sm = libsbml.StoichiometryMath(2,4)
    math = libsbml.ASTNode(libsbml.AST_TIMES)
    a = libsbml.ASTNode()
    b = libsbml.ASTNode()
    a.setName( "a")
    b.setName( "b")
    math.addChild(a)
    math.addChild(b)
    sm.setMath(math)
    i = sr1.setStoichiometryMath(sm)
    self.assert_( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE )
    self.assertEqual( False, sr1.isSetStoichiometryMath() )
    _dummyList = [ sm ]; _dummyList[:] = []; del _dummyList
    _dummyList = [ sr1 ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SpeciesReference_setStoichiometryMath6(self):
    sm = libsbml.StoichiometryMath(2,1)
    i = self.sr.setStoichiometryMath(sm)
    self.assert_( i == libsbml.LIBSBML_VERSION_MISMATCH )
    self.assertEqual( False, self.sr.isSetStoichiometryMath() )
    _dummyList = [ sm ]; _dummyList[:] = []; del _dummyList
    pass  

def suite():
  suite = unittest.TestSuite()
  suite.addTest(unittest.makeSuite(TestSpeciesReference_newSetters))

  return suite

if __name__ == "__main__":
  if unittest.TextTestRunner(verbosity=1).run(suite()).wasSuccessful() :
    sys.exit(0)
  else:
    sys.exit(1)