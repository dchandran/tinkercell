#
# @file    TestCompartmentType_newSetters.rb
# @brief   CompartmentType unit tests for new set function API
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Sarah Keating
#
# $Id$
# $HeadURL$
#
# This test file was converted from src/sbml/test/TestCompartmentType_newSetters.c
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

class TestCompartmentType_newSetters < Test::Unit::TestCase

  def setup
    @@c = LibSBML::CompartmentType.new(2,2)
    if (@@c == nil)
    end
  end

  def teardown
    @@c = nil
  end

  def test_CompartmentType_setId2
    i = @@c.setId( "1cell")
    assert( i == LibSBML::LIBSBML_INVALID_ATTRIBUTE_VALUE )
    assert_equal false, @@c.isSetId()
  end

  def test_CompartmentType_setId3
    i = @@c.setId( "cell")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal true, @@c.isSetId()
    assert ((  "cell"  == @@c.getId() ))
  end

  def test_CompartmentType_setId4
    i = @@c.setId("")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetId()
  end

  def test_CompartmentType_setName1
    i = @@c.setName( "cell")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal true, @@c.isSetName()
    i = @@c.unsetName()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetName()
  end

  def test_CompartmentType_setName2
    i = @@c.setName( "1cell")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal true, @@c.isSetName()
    i = @@c.unsetName()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetName()
  end

  def test_CompartmentType_setName3
    i = @@c.setName("")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetName()
  end

end