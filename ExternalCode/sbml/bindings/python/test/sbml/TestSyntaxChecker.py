#
# @file    TestSyntaxChecker.py
# @brief   SyntaxChecker unit tests
#
# @author  Akiya Jouraku (Python conversion)
# @author  Sarah Keating
#
# $Id$
# $HeadURL$
#
# This test file was converted from src/sbml/test/TestSyntaxChecker.c
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

class TestSyntaxChecker(unittest.TestCase):


  def test_SyntaxChecker_validID(self):
    self.assert_( libsbml.SyntaxChecker.isValidXMLID("cell") == True )
    self.assert_( libsbml.SyntaxChecker.isValidXMLID("1cell") == False )
    self.assert_( libsbml.SyntaxChecker.isValidXMLID("_cell") == True )
    pass  

  def test_SyntaxChecker_validId(self):
    self.assert_( libsbml.SyntaxChecker.isValidSBMLSId("cell") == True )
    self.assert_( libsbml.SyntaxChecker.isValidSBMLSId("1cell") == False )
    pass  

  def test_SyntaxChecker_validUnitId(self):
    self.assert_( libsbml.SyntaxChecker.isValidUnitSId("cell") == True )
    self.assert_( libsbml.SyntaxChecker.isValidUnitSId("1cell") == False )
    pass  

  def test_SyntaxChecker_validXHTML(self):
    triple = libsbml.XMLTriple("p", "", "")
    att = libsbml.XMLAttributes()
    ns = libsbml.XMLNamespaces()
    ns.add( "http://www.w3.org/1999/xhtml", "")
    tt = libsbml.XMLToken("This is my text")
    n1 = libsbml.XMLNode(tt)
    token = libsbml.XMLToken(triple,att,ns)
    node = libsbml.XMLNode(token)
    node.addChild(n1)
    self.assert_( libsbml.SyntaxChecker.hasExpectedXHTMLSyntax(node,None) == False )
    triple = libsbml.XMLTriple("html", "", "")
    ns.clear()
    token = libsbml.XMLToken(triple,att,ns)
    node = libsbml.XMLNode(token)
    node.addChild(n1)
    self.assert_( libsbml.SyntaxChecker.hasExpectedXHTMLSyntax(node,None) == False )
    pass  

def suite():
  suite = unittest.TestSuite()
  suite.addTest(unittest.makeSuite(TestSyntaxChecker))

  return suite

if __name__ == "__main__":
  if unittest.TextTestRunner(verbosity=1).run(suite()).wasSuccessful() :
    sys.exit(0)
  else:
    sys.exit(1)