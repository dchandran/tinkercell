#
# @file    TestModelHistory.py
# @brief   ModelHistory unit tests
#
# @author  Akiya Jouraku (Python conversion)
# @author  Sarah Keating 
#
# $Id: TestModelHistory.py 9762 2009-07-13 06:51:18Z ajouraku $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/python/test/annotation/TestModelHistory.py $
#
# This test file was converted from src/sbml/test/TestModelHistory.c
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

class TestModelHistory(unittest.TestCase):


  def test_Date_create(self):
    date = libsbml.Date(2005,12,30,12,15,45,1,2,0)
    self.assert_( date != None )
    self.assert_( date.getYear() == 2005 )
    self.assert_( date.getMonth() == 12 )
    self.assert_( date.getDay() == 30 )
    self.assert_( date.getHour() == 12 )
    self.assert_( date.getMinute() == 15 )
    self.assert_( date.getSecond() == 45 )
    self.assert_( date.getSignOffset() == 1 )
    self.assert_( date.getHoursOffset() == 2 )
    self.assert_( date.getMinutesOffset() == 0 )
    date = None
    pass  

  def test_Date_createFromString(self):
    dd =  "2012-12-02T14:56:11Z";
    date = libsbml.Date(dd)
    self.assert_( date != None )
    self.assert_((  "2012-12-02T14:56:11Z" == date.getDateAsString() ))
    self.assert_( date.getYear() == 2012 )
    self.assert_( date.getMonth() == 12 )
    self.assert_( date.getDay() == 2 )
    self.assert_( date.getHour() == 14 )
    self.assert_( date.getMinute() == 56 )
    self.assert_( date.getSecond() == 11 )
    self.assert_( date.getSignOffset() == 0 )
    self.assert_( date.getHoursOffset() == 0 )
    self.assert_( date.getMinutesOffset() == 0 )
    date = None
    pass  

  def test_Date_getDateAsString(self):
    dd =  "2005-02-02T14:56:11Z";
    date = libsbml.Date(dd)
    self.assert_( date != None )
    self.assert_( date.getYear() == 2005 )
    self.assert_( date.getMonth() == 2 )
    self.assert_( date.getDay() == 2 )
    self.assert_( date.getHour() == 14 )
    self.assert_( date.getMinute() == 56 )
    self.assert_( date.getSecond() == 11 )
    self.assert_( date.getSignOffset() == 0 )
    self.assert_( date.getHoursOffset() == 0 )
    self.assert_( date.getMinutesOffset() == 0 )
    date.setYear(2012)
    date.setMonth(3)
    date.setDay(28)
    date.setHour(23)
    date.setMinute(4)
    date.setSecond(32)
    date.setSignOffset(1)
    date.setHoursOffset(2)
    date.setMinutesOffset(32)
    self.assert_((  "2012-03-28T23:04:32+02:32" == date.getDateAsString() ))
    date = None
    pass  

  def test_Date_setters(self):
    date = libsbml.Date(2005,12,30,12,15,45,1,2,0)
    self.assert_( date != None )
    date.setYear(2012)
    date.setMonth(3)
    date.setDay(28)
    date.setHour(23)
    date.setMinute(4)
    date.setSecond(32)
    date.setSignOffset(1)
    date.setHoursOffset(2)
    date.setMinutesOffset(32)
    self.assert_( date.getYear() == 2012 )
    self.assert_( date.getMonth() == 3 )
    self.assert_( date.getDay() == 28 )
    self.assert_( date.getHour() == 23 )
    self.assert_( date.getMinute() == 4 )
    self.assert_( date.getSecond() == 32 )
    self.assert_( date.getSignOffset() == 1 )
    self.assert_( date.getHoursOffset() == 2 )
    self.assert_( date.getMinutesOffset() == 32 )
    self.assert_((  "2012-03-28T23:04:32+02:32" == date.getDateAsString() ))
    date = None
    pass  

  def test_ModelCreator_create(self):
    mc = libsbml.ModelCreator()
    self.assert_( mc != None )
    mc = None
    pass  

  def test_ModelCreator_setters(self):
    mc = libsbml.ModelCreator()
    self.assert_( mc != None )
    self.assert_( mc.isSetFamilyName() == False )
    self.assert_( mc.isSetGivenName() == False )
    self.assert_( mc.isSetEmail() == False )
    self.assert_( mc.isSetOrganisation() == False )
    mc.setFamilyName( "Keating")
    mc.setGivenName( "Sarah")
    mc.setEmail( "sbml-team@caltech.edu")
    mc.setOrganisation( "UH")
    self.assert_((  "Keating" == mc.getFamilyName() ))
    self.assert_((  "Sarah" == mc.getGivenName() ))
    self.assert_((  "sbml-team@caltech.edu" == mc.getEmail() ))
    self.assert_((  "UH" == mc.getOrganisation() ))
    self.assert_( mc.isSetFamilyName() == True )
    self.assert_( mc.isSetGivenName() == True )
    self.assert_( mc.isSetEmail() == True )
    self.assert_( mc.isSetOrganisation() == True )
    mc.unsetFamilyName()
    mc.unsetGivenName()
    mc.unsetEmail()
    mc.unsetOrganisation()
    self.assert_((  "" == mc.getFamilyName() ))
    self.assert_((  "" == mc.getGivenName() ))
    self.assert_((  "" == mc.getEmail() ))
    self.assert_((  "" == mc.getOrganisation() ))
    self.assert_( mc.isSetFamilyName() == False )
    self.assert_( mc.isSetGivenName() == False )
    self.assert_( mc.isSetEmail() == False )
    self.assert_( mc.isSetOrganisation() == False )
    self.assert_( mc.isSetOrganization() == False )
    mc.setOrganization( "UH")
    self.assert_((  "UH" == mc.getOrganization() ))
    self.assert_( mc.isSetOrganization() == True )
    mc.unsetOrganisation()
    self.assert_((  "" == mc.getOrganization() ))
    self.assert_( mc.isSetOrganization() == False )
    mc = None
    pass  

  def test_ModelHistory_addCreator(self):
    history = libsbml.ModelHistory()
    self.assert_( history.getNumCreators() == 0 )
    self.assert_( history != None )
    mc = libsbml.ModelCreator()
    self.assert_( mc != None )
    mc.setFamilyName( "Keating")
    mc.setGivenName( "Sarah")
    mc.setEmail( "sbml-team@caltech.edu")
    mc.setOrganisation( "UH")
    history.addCreator(mc)
    self.assert_( history.getNumCreators() == 1 )
    mc = None
    newMC = history.getListCreators().get(0)
    self.assert_( newMC != None )
    self.assert_((  "Keating" == newMC.getFamilyName() ))
    self.assert_((  "Sarah" == newMC.getGivenName() ))
    self.assert_((  "sbml-team@caltech.edu" == newMC.getEmail() ))
    self.assert_((  "UH" == newMC.getOrganisation() ))
    history = None
    pass  

  def test_ModelHistory_addModifiedDate(self):
    history = libsbml.ModelHistory()
    self.assert_( history != None )
    self.assert_( history.isSetModifiedDate() == False )
    self.assert_( history.getNumModifiedDates() == 0 )
    date = libsbml.Date(2005,12,30,12,15,45,1,2,0)
    history.addModifiedDate(date)
    date = None
    self.assert_( history.getNumModifiedDates() == 1 )
    self.assert_( history.isSetModifiedDate() == True )
    newdate = history.getListModifiedDates().get(0)
    self.assert_( newdate.getYear() == 2005 )
    self.assert_( newdate.getMonth() == 12 )
    self.assert_( newdate.getDay() == 30 )
    self.assert_( newdate.getHour() == 12 )
    self.assert_( newdate.getMinute() == 15 )
    self.assert_( newdate.getSecond() == 45 )
    self.assert_( newdate.getSignOffset() == 1 )
    self.assert_( newdate.getHoursOffset() == 2 )
    self.assert_( newdate.getMinutesOffset() == 0 )
    date1 = libsbml.Date(2008,11,2,16,42,40,1,2,0)
    history.addModifiedDate(date1)
    date1 = None
    self.assert_( history.getNumModifiedDates() == 2 )
    self.assert_( history.isSetModifiedDate() == True )
    newdate1 = history.getModifiedDate(1)
    self.assert_( newdate1.getYear() == 2008 )
    self.assert_( newdate1.getMonth() == 11 )
    self.assert_( newdate1.getDay() == 2 )
    self.assert_( newdate1.getHour() == 16 )
    self.assert_( newdate1.getMinute() == 42 )
    self.assert_( newdate1.getSecond() == 40 )
    self.assert_( newdate1.getSignOffset() == 1 )
    self.assert_( newdate1.getHoursOffset() == 2 )
    self.assert_( newdate1.getMinutesOffset() == 0 )
    history = None
    pass  

  def test_ModelHistory_create(self):
    history = libsbml.ModelHistory()
    self.assert_( history != None )
    self.assert_( history.getListCreators() != None )
    self.assert_( history.getCreatedDate() == None )
    self.assert_( history.getModifiedDate() == None )
    history = None
    pass  

  def test_ModelHistory_setCreatedDate(self):
    history = libsbml.ModelHistory()
    self.assert_( history != None )
    self.assert_( history.isSetCreatedDate() == False )
    date = libsbml.Date(2005,12,30,12,15,45,1,2,0)
    history.setCreatedDate(date)
    self.assert_( history.isSetCreatedDate() == True )
    date = None
    newdate = history.getCreatedDate()
    self.assert_( newdate.getYear() == 2005 )
    self.assert_( newdate.getMonth() == 12 )
    self.assert_( newdate.getDay() == 30 )
    self.assert_( newdate.getHour() == 12 )
    self.assert_( newdate.getMinute() == 15 )
    self.assert_( newdate.getSecond() == 45 )
    self.assert_( newdate.getSignOffset() == 1 )
    self.assert_( newdate.getHoursOffset() == 2 )
    self.assert_( newdate.getMinutesOffset() == 0 )
    history = None
    pass  

  def test_ModelHistory_setModifiedDate(self):
    history = libsbml.ModelHistory()
    self.assert_( history != None )
    self.assert_( history.isSetModifiedDate() == False )
    date = libsbml.Date(2005,12,30,12,15,45,1,2,0)
    history.setModifiedDate(date)
    date = None
    self.assert_( history.isSetModifiedDate() == True )
    newdate = history.getModifiedDate()
    self.assert_( newdate.getYear() == 2005 )
    self.assert_( newdate.getMonth() == 12 )
    self.assert_( newdate.getDay() == 30 )
    self.assert_( newdate.getHour() == 12 )
    self.assert_( newdate.getMinute() == 15 )
    self.assert_( newdate.getSecond() == 45 )
    self.assert_( newdate.getSignOffset() == 1 )
    self.assert_( newdate.getHoursOffset() == 2 )
    self.assert_( newdate.getMinutesOffset() == 0 )
    history = None
    pass  

def suite():
  suite = unittest.TestSuite()
  suite.addTest(unittest.makeSuite(TestModelHistory))

  return suite

if __name__ == "__main__":
  if unittest.TextTestRunner(verbosity=1).run(suite()).wasSuccessful() :
    sys.exit(0)
  else:
    sys.exit(1)
