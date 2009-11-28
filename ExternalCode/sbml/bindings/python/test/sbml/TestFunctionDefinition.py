#
# @file    TestFunctionDefinition.py
# @brief   SBML FunctionDefinition unit tests
#
# @author  Akiya Jouraku (Python conversion)
# @author  Ben Bornstein 
#
# $Id: TestFunctionDefinition.py 10124 2009-08-28 12:04:51Z sarahkeating $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/python/test/sbml/TestFunctionDefinition.py $
#
# This test file was converted from src/sbml/test/TestFunctionDefinition.c
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

class TestFunctionDefinition(unittest.TestCase):

  FD = None

  def setUp(self):
    self.FD = libsbml.FunctionDefinition(2,4)
    if (self.FD == None):
      pass    
    pass  

  def tearDown(self):
    self.FD = None
    pass  

  def test_FunctionDefinition_create(self):
    self.assert_( self.FD.getTypeCode() == libsbml.SBML_FUNCTION_DEFINITION )
    self.assert_( self.FD.getMetaId() == "" )
    self.assert_( self.FD.getNotes() == None )
    self.assert_( self.FD.getAnnotation() == None )
    self.assert_( self.FD.getId() == "" )
    self.assert_( self.FD.getName() == "" )
    self.assert_( self.FD.getMath() == None )
    pass  

  def test_FunctionDefinition_createWith(self):
    math = libsbml.parseFormula("lambda(x, x^3)")
    fd = libsbml.FunctionDefinition(2,4)
    fd.setId( "pow3")
    fd.setMath(math)
    self.assert_( fd.getTypeCode() == libsbml.SBML_FUNCTION_DEFINITION )
    self.assert_( fd.getMetaId() == "" )
    self.assert_( fd.getNotes() == None )
    self.assert_( fd.getAnnotation() == None )
    self.assert_( fd.getName() == "" )
    math1 = fd.getMath()
    self.assert_( math1 != None )
    formula = libsbml.formulaToString(math1)
    self.assert_( formula != None )
    self.assert_((  "lambda(x, x^3)" == formula ))
    self.assert_( fd.getMath() != math )
    self.assertEqual( True, fd.isSetMath() )
    self.assert_((  "pow3" == fd.getId() ))
    self.assertEqual( True, fd.isSetId() )
    math = None
    fd = None
    pass  

  def test_FunctionDefinition_createWithNS(self):
    xmlns = libsbml.XMLNamespaces()
    xmlns.add( "http://www.sbml.org", "testsbml")
    sbmlns = libsbml.SBMLNamespaces(2,1)
    sbmlns.addNamespaces(xmlns)
    object = libsbml.FunctionDefinition(sbmlns)
    self.assert_( object.getTypeCode() == libsbml.SBML_FUNCTION_DEFINITION )
    self.assert_( object.getMetaId() == "" )
    self.assert_( object.getNotes() == None )
    self.assert_( object.getAnnotation() == None )
    self.assert_( object.getLevel() == 2 )
    self.assert_( object.getVersion() == 1 )
    self.assert_( object.getNamespaces() != None )
    self.assert_( object.getNamespaces().getLength() == 2 )
    object = None
    pass  

  def test_FunctionDefinition_free_NULL(self):
    pass  

  def test_FunctionDefinition_getArguments(self):
    self.FD.setMath(libsbml.parseFormula("lambda(x, y, x^y)"))
    self.assert_( self.FD.getNumArguments() == 2 )
    math = self.FD.getArgument(0)
    self.assert_( math != None )
    self.assertEqual( True, math.isName() )
    self.assert_((  "x" == math.getName() ))
    self.assert_( math.getNumChildren() == 0 )
    math = self.FD.getArgument(1)
    self.assert_( math != None )
    self.assertEqual( True, math.isName() )
    self.assert_((  "y" == math.getName() ))
    self.assert_( math.getNumChildren() == 0 )
    self.assert_( self.FD.getArgument(0) == self.FD.getArgument( "x") )
    self.assert_( self.FD.getArgument(1) == self.FD.getArgument( "y") )
    pass  

  def test_FunctionDefinition_getBody(self):
    math1 = libsbml.parseFormula("lambda(x, x)")
    self.FD.setMath(math1)
    math = self.FD.getBody()
    self.assert_( math != None )
    self.assertEqual( True, math.isName() )
    self.assert_((  "x" == math.getName() ))
    self.assert_( math.getNumChildren() == 0 )
    math1 = None
    pass  

  def test_FunctionDefinition_setId(self):
    id =  "pow3";
    self.FD.setId(id)
    self.assert_(( id == self.FD.getId() ))
    self.assertEqual( True, self.FD.isSetId() )
    if (self.FD.getId() == id):
      pass    
    self.FD.setId(self.FD.getId())
    self.assert_(( id == self.FD.getId() ))
    self.FD.setId("")
    self.assertEqual( False, self.FD.isSetId() )
    if (self.FD.getId() != None):
      pass    
    pass  

  def test_FunctionDefinition_setMath(self):
    math = libsbml.parseFormula("lambda(x, x^3)")
    self.FD.setMath(math)
    math1 = self.FD.getMath()
    self.assert_( math1 != None )
    formula = libsbml.formulaToString(math1)
    self.assert_( formula != None )
    self.assert_((  "lambda(x, x^3)" == formula ))
    self.assert_( self.FD.getMath() != math )
    self.assertEqual( True, self.FD.isSetMath() )
    self.FD.setMath(self.FD.getMath())
    math1 = self.FD.getMath()
    self.assert_( math1 != None )
    formula = libsbml.formulaToString(math1)
    self.assert_( formula != None )
    self.assert_((  "lambda(x, x^3)" == formula ))
    self.assert_( self.FD.getMath() != math )
    self.FD.setMath(None)
    self.assertEqual( False, self.FD.isSetMath() )
    if (self.FD.getMath() != None):
      pass    
    pass  

  def test_FunctionDefinition_setName(self):
    name =  "Cube_Me";
    self.FD.setName(name)
    self.assert_(( name == self.FD.getName() ))
    self.assertEqual( True, self.FD.isSetName() )
    if (self.FD.getName() == name):
      pass    
    self.FD.setName(self.FD.getName())
    self.assert_(( name == self.FD.getName() ))
    self.FD.setName("")
    self.assertEqual( False, self.FD.isSetName() )
    if (self.FD.getName() != None):
      pass    
    pass  

def suite():
  suite = unittest.TestSuite()
  suite.addTest(unittest.makeSuite(TestFunctionDefinition))

  return suite

if __name__ == "__main__":
  if unittest.TextTestRunner(verbosity=1).run(suite()).wasSuccessful() :
    sys.exit(0)
  else:
    sys.exit(1)
