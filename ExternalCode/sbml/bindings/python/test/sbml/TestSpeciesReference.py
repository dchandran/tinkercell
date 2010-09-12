#
# @file    TestSpeciesReference.py
# @brief   SpeciesReference unit tests
#
# @author  Akiya Jouraku (Python conversion)
# @author  Ben Bornstein 
#
# $Id: TestSpeciesReference.py 10124 2009-08-28 12:04:51Z sarahkeating $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/python/test/sbml/TestSpeciesReference.py $
#
# This test file was converted from src/sbml/test/TestSpeciesReference.c
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

class TestSpeciesReference(unittest.TestCase):

  SR = None

  def setUp(self):
    self.SR = libsbml.SpeciesReference(2,4)
    if (self.SR == None):
      pass    
    pass  

  def tearDown(self):
    self.SR = None
    pass  

  def test_SpeciesReference_create(self):
    self.assert_( self.SR.getTypeCode() == libsbml.SBML_SPECIES_REFERENCE )
    self.assert_( self.SR.getMetaId() == "" )
    self.assert_( self.SR.getNotes() == None )
    self.assert_( self.SR.getAnnotation() == None )
    self.assert_( self.SR.getSpecies() == "" )
    self.assert_( self.SR.getStoichiometry() == 1 )
    self.assert_( self.SR.getStoichiometryMath() == None )
    self.assert_( self.SR.getDenominator() == 1 )
    self.assertEqual( False, self.SR.isSetSpecies() )
    self.assertEqual( False, self.SR.isSetStoichiometryMath() )
    pass  

  def test_SpeciesReference_createModifier(self):
    sr = libsbml.ModifierSpeciesReference(2,4)
    self.assert_( sr.getTypeCode() == libsbml.SBML_MODIFIER_SPECIES_REFERENCE )
    self.assert_( sr.getMetaId() == "" )
    self.assert_( sr.getNotes() == None )
    self.assert_( sr.getAnnotation() == None )
    self.assertEqual( True, sr.isModifier() )
    sr = None
    pass  

  def test_SpeciesReference_createWithNS(self):
    xmlns = libsbml.XMLNamespaces()
    xmlns.add( "http://www.sbml.org", "testsbml")
    sbmlns = libsbml.SBMLNamespaces(2,1)
    sbmlns.addNamespaces(xmlns)
    object = libsbml.SpeciesReference(sbmlns)
    self.assert_( object.getTypeCode() == libsbml.SBML_SPECIES_REFERENCE )
    self.assert_( object.getMetaId() == "" )
    self.assert_( object.getNotes() == None )
    self.assert_( object.getAnnotation() == None )
    self.assert_( object.getLevel() == 2 )
    self.assert_( object.getVersion() == 1 )
    self.assert_( object.getNamespaces() != None )
    self.assert_( object.getNamespaces().getLength() == 2 )
    object = None
    pass  

  def test_SpeciesReference_free_NULL(self):
    pass  

  def test_SpeciesReference_setId(self):
    species =  "X0";
    self.SR.setId(species)
    self.assert_(( species == self.SR.getId() ))
    self.assertEqual( True, self.SR.isSetId() )
    if (self.SR.getId() == species):
      pass    
    self.SR.setId(self.SR.getId())
    self.assert_(( species == self.SR.getId() ))
    self.SR.setId("")
    self.assertEqual( False, self.SR.isSetId() )
    if (self.SR.getId() != None):
      pass    
    pass  

  def test_SpeciesReference_setSpecies(self):
    species =  "X0";
    self.SR.setSpecies(species)
    self.assert_(( species == self.SR.getSpecies() ))
    self.assertEqual( True, self.SR.isSetSpecies() )
    if (self.SR.getSpecies() == species):
      pass    
    self.SR.setSpecies(self.SR.getSpecies())
    self.assert_(( species == self.SR.getSpecies() ))
    self.SR.setSpecies("")
    self.assertEqual( False, self.SR.isSetSpecies() )
    if (self.SR.getSpecies() != None):
      pass    
    pass  

  def test_SpeciesReference_setStoichiometryMath(self):
    math = libsbml.parseFormula("k3 / k2")
    stoich = libsbml.StoichiometryMath(2,4)
    stoich.setMath(math)
    self.SR.setStoichiometryMath(stoich)
    math1 = self.SR.getStoichiometryMath()
    self.assert_( math1 != None )
    formula = libsbml.formulaToString(math1.getMath())
    self.assert_( formula != None )
    self.assert_((  "k3 / k2" == formula ))
    self.assertEqual( True, self.SR.isSetStoichiometryMath() )
    pass  

def suite():
  suite = unittest.TestSuite()
  suite.addTest(unittest.makeSuite(TestSpeciesReference))

  return suite

if __name__ == "__main__":
  if unittest.TextTestRunner(verbosity=1).run(suite()).wasSuccessful() :
    sys.exit(0)
  else:
    sys.exit(1)