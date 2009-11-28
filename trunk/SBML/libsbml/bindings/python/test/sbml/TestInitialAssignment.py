#
# @file    TestInitialAssignment.py
# @brief   SBML InitialAssignment unit tests
#
# @author  Akiya Jouraku (Python conversion)
# @author  Sarah Keating 
#
# $Id: TestInitialAssignment.py 10124 2009-08-28 12:04:51Z sarahkeating $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/python/test/sbml/TestInitialAssignment.py $
#
# This test file was converted from src/sbml/test/TestInitialAssignment.c
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

class TestInitialAssignment(unittest.TestCase):

  IA = None

  def setUp(self):
    self.IA = libsbml.InitialAssignment(2,4)
    if (self.IA == None):
      pass    
    pass  

  def tearDown(self):
    self.IA = None
    pass  

  def test_InitialAssignment_create(self):
    self.assert_( self.IA.getTypeCode() == libsbml.SBML_INITIAL_ASSIGNMENT )
    self.assert_( self.IA.getMetaId() == "" )
    self.assert_( self.IA.getNotes() == None )
    self.assert_( self.IA.getAnnotation() == None )
    self.assert_( self.IA.getSymbol() == "" )
    self.assert_( self.IA.getMath() == None )
    pass  

  def test_InitialAssignment_createWithNS(self):
    xmlns = libsbml.XMLNamespaces()
    xmlns.add( "http://www.sbml.org", "testsbml")
    sbmlns = libsbml.SBMLNamespaces(2,3)
    sbmlns.addNamespaces(xmlns)
    object = libsbml.InitialAssignment(sbmlns)
    self.assert_( object.getTypeCode() == libsbml.SBML_INITIAL_ASSIGNMENT )
    self.assert_( object.getMetaId() == "" )
    self.assert_( object.getNotes() == None )
    self.assert_( object.getAnnotation() == None )
    self.assert_( object.getLevel() == 2 )
    self.assert_( object.getVersion() == 3 )
    self.assert_( object.getNamespaces() != None )
    self.assert_( object.getNamespaces().getLength() == 2 )
    object = None
    pass  

  def test_InitialAssignment_free_NULL(self):
    pass  

  def test_InitialAssignment_setMath(self):
    math = libsbml.parseFormula("2 * k")
    self.IA.setMath(math)
    math1 = self.IA.getMath()
    self.assert_( math1 != None )
    formula = libsbml.formulaToString(math1)
    self.assert_( formula != None )
    self.assert_((  "2 * k" == formula ))
    self.assert_( self.IA.getMath() != math )
    self.assertEqual( True, self.IA.isSetMath() )
    self.IA.setMath(self.IA.getMath())
    math1 = self.IA.getMath()
    self.assert_( math1 != None )
    formula = libsbml.formulaToString(math1)
    self.assert_( formula != None )
    self.assert_((  "2 * k" == formula ))
    self.assert_( self.IA.getMath() != math )
    self.IA.setMath(None)
    self.assertEqual( False, self.IA.isSetMath() )
    if (self.IA.getMath() != None):
      pass    
    math = None
    pass  

  def test_InitialAssignment_setSymbol(self):
    Symbol =  "k2";
    self.IA.setSymbol(Symbol)
    self.assert_(( Symbol == self.IA.getSymbol() ))
    self.assertEqual( True, self.IA.isSetSymbol() )
    if (self.IA.getSymbol() == Symbol):
      pass    
    self.IA.setSymbol(self.IA.getSymbol())
    self.assert_(( Symbol == self.IA.getSymbol() ))
    self.IA.setSymbol("")
    self.assertEqual( False, self.IA.isSetSymbol() )
    if (self.IA.getSymbol() != None):
      pass    
    pass  

def suite():
  suite = unittest.TestSuite()
  suite.addTest(unittest.makeSuite(TestInitialAssignment))

  return suite

if __name__ == "__main__":
  if unittest.TextTestRunner(verbosity=1).run(suite()).wasSuccessful() :
    sys.exit(0)
  else:
    sys.exit(1)
