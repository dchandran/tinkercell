#
# @file    TestAnnotationCopyAndClone.rb
# @brief   Test the copy and clone methods for annotation classes
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Sarah Keating 
#
# $Id: TestAnnotationCopyAndClone.rb 10124 2009-08-28 12:04:51Z sarahkeating $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/ruby/test/annotation/TestAnnotationCopyAndClone.rb $
#
# This test file was converted from src/sbml/test/TestCopyAndClone.cpp
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
require 'test/unit'
require 'libSBML'

class TestAnnotationCopyAndClone < Test::Unit::TestCase

  def test_CVTerm_assignmentOperator
    cvterm1 = LibSBML::CVTerm.new(LibSBML::BIOLOGICAL_QUALIFIER)
    cvterm1.addResource("http://www.geneontology.org/#GO:0005892")
    assert( cvterm1.getQualifierType() == LibSBML::BIOLOGICAL_QUALIFIER )
    assert( cvterm1.getResources().getLength() == 1 )
    assert( cvterm1.getResources().getValue(0) ==  "http://www.geneontology.org/#GO:0005892" )
    cvterm2 = LibSBML::CVTerm.new()
    cvterm2 = cvterm1
    assert( cvterm2.getQualifierType() == LibSBML::BIOLOGICAL_QUALIFIER )
    assert( cvterm2.getResources().getLength() == 1 )
    assert( cvterm2.getResources().getValue(0) ==  "http://www.geneontology.org/#GO:0005892" )
    cvterm2 = nil
    cvterm1 = nil
  end

  def test_CVTerm_clone
    cvterm1 = LibSBML::CVTerm.new(LibSBML::BIOLOGICAL_QUALIFIER)
    cvterm1.addResource("http://www.geneontology.org/#GO:0005892")
    assert( cvterm1.getQualifierType() == LibSBML::BIOLOGICAL_QUALIFIER )
    assert( cvterm1.getResources().getLength() == 1 )
    assert( cvterm1.getResources().getValue(0) ==  "http://www.geneontology.org/#GO:0005892" )
    cvterm2 = cvterm1.clone()
    assert( cvterm2.getQualifierType() == LibSBML::BIOLOGICAL_QUALIFIER )
    assert( cvterm2.getResources().getLength() == 1 )
    assert( cvterm2.getResources().getValue(0) ==  "http://www.geneontology.org/#GO:0005892" )
    cvterm2 = nil
    cvterm1 = nil
  end

  def test_CVTerm_copyConstructor
    cvterm1 = LibSBML::CVTerm.new(LibSBML::BIOLOGICAL_QUALIFIER)
    cvterm1.addResource("http://www.geneontology.org/#GO:0005892")
    assert( cvterm1.getQualifierType() == LibSBML::BIOLOGICAL_QUALIFIER )
    assert( cvterm1.getResources().getLength() == 1 )
    assert( cvterm1.getResources().getValue(0) ==  "http://www.geneontology.org/#GO:0005892" )
    cvterm2 = LibSBML::CVTerm.new(cvterm1)
    assert( cvterm2.getQualifierType() == LibSBML::BIOLOGICAL_QUALIFIER )
    assert( cvterm2.getResources().getLength() == 1 )
    assert( cvterm2.getResources().getValue(0) ==  "http://www.geneontology.org/#GO:0005892" )
    cvterm2 = nil
    cvterm1 = nil
  end

  def test_Date_assignmentOperator
    date = LibSBML::Date.new(2005,12,30,12,15,45,1,2,0)
    assert( date.getMonth() == 12 )
    assert( date.getSecond() == 45 )
    date2 = LibSBML::Date.new()
    date2 = date
    assert( date2.getMonth() == 12 )
    assert( date2.getSecond() == 45 )
    date2 = nil
    date = nil
  end

  def test_Date_clone
    date = LibSBML::Date.new(2005,12,30,12,15,45,1,2,0)
    assert( date.getMonth() == 12 )
    assert( date.getSecond() == 45 )
    date2 = date.clone()
    assert( date2.getMonth() == 12 )
    assert( date2.getSecond() == 45 )
    date2 = nil
    date = nil
  end

  def test_Date_copyConstructor
    date = LibSBML::Date.new(2005,12,30,12,15,45,1,2,0)
    assert( date.getMonth() == 12 )
    assert( date.getSecond() == 45 )
    date2 = LibSBML::Date.new(date)
    assert( date2.getMonth() == 12 )
    assert( date2.getSecond() == 45 )
    date2 = nil
    date = nil
  end

  def test_ModelCreator_assignmentOperator
    mc = LibSBML::ModelCreator.new()
    mc.setFamilyName("Keating")
    mc.setEmail("sbml-team@caltech.edu")
    assert( mc.getFamilyName() ==  "Keating" )
    assert( mc.getEmail() ==  "sbml-team@caltech.edu" )
    mc2 = LibSBML::ModelCreator.new()
    mc2 = mc
    assert( mc2.getFamilyName() ==  "Keating" )
    assert( mc2.getEmail() ==  "sbml-team@caltech.edu" )
    mc2 = nil
    mc = nil
  end

  def test_ModelCreator_clone
    mc = LibSBML::ModelCreator.new()
    mc.setFamilyName("Keating")
    mc.setEmail("sbml-team@caltech.edu")
    assert( mc.getFamilyName() ==  "Keating" )
    assert( mc.getEmail() ==  "sbml-team@caltech.edu" )
    mc2 = mc.clone()
    assert( mc2.getFamilyName() ==  "Keating" )
    assert( mc2.getEmail() ==  "sbml-team@caltech.edu" )
    mc2 = nil
    mc = nil
  end

  def test_ModelCreator_copyConstructor
    mc = LibSBML::ModelCreator.new()
    mc.setFamilyName("Keating")
    mc.setEmail("sbml-team@caltech.edu")
    assert( mc.getFamilyName() ==  "Keating" )
    assert( mc.getEmail() ==  "sbml-team@caltech.edu" )
    mc2 = LibSBML::ModelCreator.new(mc)
    assert( mc2.getFamilyName() ==  "Keating" )
    assert( mc2.getEmail() ==  "sbml-team@caltech.edu" )
    mc2 = nil
    mc = nil
  end

  def test_ModelHistory_assignmentOperator
    mh = LibSBML::ModelHistory.new()
    mc = LibSBML::ModelCreator.new()
    mc.setGivenName("Sarah")
    mc.setFamilyName("Keating")
    mc.setEmail("sbml-team@caltech.edu")
    mh.addCreator(mc)
    mc = nil
    date = LibSBML::Date.new(2005,12,30,12,15,45,1,2,0)
    mh.setCreatedDate(date)
    date = nil
    assert( mh.getCreatedDate().getMonth() == 12 )
    assert( mh.getCreatedDate().getSecond() == 45 )
    assert( mh.getCreator(0).getFamilyName() ==  "Keating" )
    mh2 = LibSBML::ModelHistory.new()
    mh2 = mh
    assert( mh2.getCreatedDate().getMonth() == 12 )
    assert( mh2.getCreatedDate().getSecond() == 45 )
    assert( mh2.getCreator(0).getFamilyName() ==  "Keating" )
    mh2 = nil
    mh = nil
  end

  def test_ModelHistory_clone
    mh = LibSBML::ModelHistory.new()
    mc = LibSBML::ModelCreator.new()
    mc.setFamilyName("Keating")
    mc.setGivenName("Sarah")
    mc.setEmail("sbml-team@caltech.edu")
    mh.addCreator(mc)
    mc = nil
    date = LibSBML::Date.new(2005,12,30,12,15,45,1,2,0)
    mh.setCreatedDate(date)
    date = nil
    assert( mh.getCreatedDate().getMonth() == 12 )
    assert( mh.getCreatedDate().getSecond() == 45 )
    assert( mh.getCreator(0).getFamilyName() ==  "Keating" )
    mh2 = mh.clone()
    assert( mh2.getCreatedDate().getMonth() == 12 )
    assert( mh2.getCreatedDate().getSecond() == 45 )
    assert( mh2.getCreator(0).getFamilyName() ==  "Keating" )
    mh2 = nil
    mh = nil
  end

  def test_ModelHistory_copyConstructor
    mh = LibSBML::ModelHistory.new()
    mc = LibSBML::ModelCreator.new()
    mc.setFamilyName("Keating")
    mc.setGivenName("Sarah")
    mc.setEmail("sbml-team@caltech.edu")
    mh.addCreator(mc)
    mc = nil
    date = LibSBML::Date.new(2005,12,30,12,15,45,1,2,0)
    mh.setCreatedDate(date)
    date = nil
    assert( mh.getCreatedDate().getMonth() == 12 )
    assert( mh.getCreatedDate().getSecond() == 45 )
    assert( mh.getCreator(0).getFamilyName() ==  "Keating" )
    mh2 = LibSBML::ModelHistory.new(mh)
    assert( mh2.getCreatedDate().getMonth() == 12 )
    assert( mh2.getCreatedDate().getSecond() == 45 )
    assert( mh2.getCreator(0).getFamilyName() ==  "Keating" )
    mh2 = nil
    mh = nil
  end

end