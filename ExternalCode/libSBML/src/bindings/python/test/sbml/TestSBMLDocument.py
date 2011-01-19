#
# @file    TestSBMLDocument.py
# @brief   SBMLDocument unit tests
#
# @author  Akiya Jouraku (Python conversion)
# @author  Ben Bornstein 
#
# $Id$
# $HeadURL$
#
# ====== WARNING ===== WARNING ===== WARNING ===== WARNING ===== WARNING ======
#
# DO NOT EDIT THIS FILE.
#
# This file was generated automatically by converting the file located at
# src/sbml/test/TestSBMLDocument.c
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


class TestSBMLDocument(unittest.TestCase):


  def test_SBMLDocument_create(self):
    d = libsbml.SBMLDocument()
    self.assert_( d.getTypeCode() == libsbml.SBML_DOCUMENT )
    self.assert_( d.getNotes() == None )
    self.assert_( d.getAnnotation() == None )
    self.assert_( d.getLevel() == 3 )
    self.assert_( d.getVersion() == 1 )
    self.assert_( d.getNumErrors() == 0 )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_createWith(self):
    d = libsbml.SBMLDocument(1,2)
    self.assert_( d.getTypeCode() == libsbml.SBML_DOCUMENT )
    self.assert_( d.getNotes() == None )
    self.assert_( d.getAnnotation() == None )
    self.assert_( d.getLevel() == 1 )
    self.assert_( d.getVersion() == 2 )
    self.assert_( d.getNumErrors() == 0 )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_free_NULL(self):
    _dummyList = [ None ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_setLevelAndVersion(self):
    d = libsbml.SBMLDocument(2,2)
    m1 = libsbml.Model(2,2)
    d.setModel(m1)
    self.assert_( d.setLevelAndVersion(2,3,False) == True )
    self.assert_( d.setLevelAndVersion(2,1,False) == True )
    self.assert_( d.setLevelAndVersion(1,2,False) == True )
    self.assert_( d.setLevelAndVersion(1,1,False) == False )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_setLevelAndVersion_Error(self):
    d = libsbml.SBMLDocument()
    d.setLevelAndVersion(2,1,True)
    m1 = libsbml.Model(2,1)
    u = libsbml.Unit(2,1)
    u.setKind(libsbml.UnitKind_forName("mole"))
    u.setOffset(3.2)
    ud = libsbml.UnitDefinition(2,1)
    ud.setId( "ud")
    ud.addUnit(u)
    m1.addUnitDefinition(ud)
    d.setModel(m1)
    self.assert_( d.setLevelAndVersion(2,2,True) == False )
    self.assert_( d.setLevelAndVersion(2,3,True) == False )
    self.assert_( d.setLevelAndVersion(1,2,True) == False )
    self.assert_( d.setLevelAndVersion(1,1,True) == False )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_setLevelAndVersion_UnitsError(self):
    d = libsbml.SBMLDocument()
    d.setLevelAndVersion(2,4,False)
    m1 = d.createModel()
    c = m1.createCompartment()
    c.setId( "c")
    p = m1.createParameter()
    p.setId( "p")
    p.setUnits( "mole")
    r = m1.createAssignmentRule()
    r.setVariable( "c")
    r.setFormula( "p*p")
    self.assert_( d.setLevelAndVersion(2,2,False) == True )
    self.assert_( d.setLevelAndVersion(2,3,False) == True )
    self.assert_( d.setLevelAndVersion(1,2,False) == True )
    self.assert_( d.setLevelAndVersion(1,1,False) == False )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_setLevelAndVersion_Warning(self):
    d = libsbml.SBMLDocument(2,2)
    m1 = libsbml.Model(2,2)
    (m1).setSBOTerm(2)
    d.setModel(m1)
    self.assert_( d.setLevelAndVersion(2,3,False) == True )
    self.assert_( d.setLevelAndVersion(2,1,False) == True )
    self.assert_( d.setLevelAndVersion(1,2,False) == True )
    self.assert_( d.setLevelAndVersion(1,1,False) == False )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_setModel(self):
    d = libsbml.SBMLDocument(2,4)
    m1 = libsbml.Model(2,4)
    m2 = libsbml.Model(2,4)
    self.assert_( d.getModel() == None )
    i = d.setModel(m1)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    mout = d.getModel()
    self.assert_( mout != None )
    self.assert_( mout != m1 )
    i = d.setModel(d.getModel())
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    mout = d.getModel()
    self.assert_( mout != None )
    self.assert_( mout != m1 )
    i = d.setModel(m2)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    mout = d.getModel()
    self.assert_( mout != None )
    self.assert_( mout != m2 )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_setModel1(self):
    d = libsbml.SBMLDocument(2,2)
    m1 = libsbml.Model(2,1)
    i = d.setModel(m1)
    self.assert_( i == libsbml.LIBSBML_VERSION_MISMATCH )
    self.assert_( d.getModel() == None )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_setModel2(self):
    d = libsbml.SBMLDocument(2,2)
    m1 = libsbml.Model(1,2)
    i = d.setModel(m1)
    self.assert_( i == libsbml.LIBSBML_LEVEL_MISMATCH )
    self.assert_( d.getModel() == None )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

  def test_SBMLDocument_setModel3(self):
    d = libsbml.SBMLDocument(2,2)
    m1 = libsbml.Model(2,2)
    i = d.setModel(m1)
    self.assert_( i == libsbml.LIBSBML_OPERATION_SUCCESS )
    self.assert_( d.getModel() != None )
    _dummyList = [ d ]; _dummyList[:] = []; del _dummyList
    pass  

def suite():
  suite = unittest.TestSuite()
  suite.addTest(unittest.makeSuite(TestSBMLDocument))

  return suite

if __name__ == "__main__":
  if unittest.TextTestRunner(verbosity=1).run(suite()).wasSuccessful() :
    sys.exit(0)
  else:
    sys.exit(1)