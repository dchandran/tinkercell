#
# @file    TestCompartment_newSetters.rb
# @brief   Compartment unit tests for new set function API
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Sarah Keating
#
# $Id$
# $HeadURL$
#
# This test file was converted from src/sbml/test/TestCompartment_newSetters.c
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

class TestCompartment_newSetters < Test::Unit::TestCase

  def setup
    @@c = LibSBML::Compartment.new(1,2)
    if (@@c == nil)
    end
  end

  def teardown
    @@c = nil
  end

  def test_Compartment_setCompartmentType1
    i = @@c.setCompartmentType( "cell")
    assert( i == LibSBML::LIBSBML_UNEXPECTED_ATTRIBUTE )
    assert_equal false, @@c.isSetCompartmentType()
    i = @@c.unsetCompartmentType()
    assert( i == LibSBML::LIBSBML_UNEXPECTED_ATTRIBUTE )
    assert_equal false, @@c.isSetCompartmentType()
  end

  def test_Compartment_setCompartmentType2
    c = LibSBML::Compartment.new(2,2)
    i = c.setCompartmentType( "1cell")
    assert( i == LibSBML::LIBSBML_INVALID_ATTRIBUTE_VALUE )
    assert_equal false, c.isSetCompartmentType()
    i = c.unsetCompartmentType()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, c.isSetCompartmentType()
    c = nil
  end

  def test_Compartment_setCompartmentType3
    c = LibSBML::Compartment.new(2,2)
    i = c.setCompartmentType( "cell")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal true, c.isSetCompartmentType()
    assert ((  "cell"  == c.getCompartmentType() ))
    i = c.unsetCompartmentType()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, c.isSetCompartmentType()
    c = nil
  end

  def test_Compartment_setCompartmentType4
    c = LibSBML::Compartment.new(2,2)
    i = c.setCompartmentType("")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, c.isSetCompartmentType()
    c = nil
  end

  def test_Compartment_setConstant1
    i = @@c.setConstant(false)
    assert( i == LibSBML::LIBSBML_UNEXPECTED_ATTRIBUTE )
    assert( @@c.getConstant() == false )
  end

  def test_Compartment_setConstant2
    c = LibSBML::Compartment.new(2,2)
    i = c.setConstant(false)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( c.getConstant() == false )
    c = nil
  end

  def test_Compartment_setId2
    c = LibSBML::Compartment.new(2,2)
    i = c.setId( "1cell")
    assert( i == LibSBML::LIBSBML_INVALID_ATTRIBUTE_VALUE )
    assert_equal false, c.isSetId()
    c = nil
  end

  def test_Compartment_setId3
    c = LibSBML::Compartment.new(2,2)
    i = c.setId( "cell")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal true, c.isSetId()
    assert ((  "cell"  == c.getId() ))
    c = nil
  end

  def test_Compartment_setId4
    c = LibSBML::Compartment.new(2,2)
    i = c.setId("")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, c.isSetId()
    c = nil
  end

  def test_Compartment_setName1
    i = @@c.setName( "cell")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal true, @@c.isSetName()
    i = @@c.unsetName()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetName()
  end

  def test_Compartment_setName2
    c = LibSBML::Compartment.new(1,2)
    i = c.setName( "1cell")
    assert( i == LibSBML::LIBSBML_INVALID_ATTRIBUTE_VALUE )
    assert_equal false, c.isSetName()
    i = c.unsetName()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, c.isSetName()
    c = nil
  end

  def test_Compartment_setName3
    i = @@c.setName("")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetName()
  end

  def test_Compartment_setOutside1
    i = @@c.setOutside( "1cell")
    assert( i == LibSBML::LIBSBML_INVALID_ATTRIBUTE_VALUE )
    assert_equal false, @@c.isSetOutside()
    i = @@c.unsetOutside()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetOutside()
  end

  def test_Compartment_setOutside2
    i = @@c.setOutside( "litre")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal true, @@c.isSetOutside()
    i = @@c.unsetOutside()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetOutside()
  end

  def test_Compartment_setOutside3
    i = @@c.setOutside("")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetOutside()
  end

  def test_Compartment_setSize1
    i = @@c.setSize(2.0)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( @@c.getSize() == 2.0 )
    i = @@c.unsetSize()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
  end

  def test_Compartment_setSize2
    c = LibSBML::Compartment.new(2,2)
    i = c.setSize(4)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( c.getSize() == 4 )
    assert_equal true, c.isSetSize()
    i = c.unsetSize()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, c.isSetSize()
    c = nil
  end

  def test_Compartment_setSpatialDimensions1
    i = @@c.setSpatialDimensions(2)
    assert( i == LibSBML::LIBSBML_UNEXPECTED_ATTRIBUTE )
    assert( @@c.getSpatialDimensions() == 3 )
  end

  def test_Compartment_setSpatialDimensions2
    c = LibSBML::Compartment.new(2,2)
    i = c.setSpatialDimensions(4)
    assert( i == LibSBML::LIBSBML_INVALID_ATTRIBUTE_VALUE )
    assert( c.getSpatialDimensions() == 3 )
    c = nil
  end

  def test_Compartment_setSpatialDimensions3
    c = LibSBML::Compartment.new(2,2)
    i = c.setSpatialDimensions(2)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( c.getSpatialDimensions() == 2 )
    c = nil
  end

  def test_Compartment_setUnits1
    i = @@c.setUnits( "1cell")
    assert( i == LibSBML::LIBSBML_INVALID_ATTRIBUTE_VALUE )
    assert_equal false, @@c.isSetUnits()
    i = @@c.unsetUnits()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetUnits()
  end

  def test_Compartment_setUnits2
    i = @@c.setUnits( "litre")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal true, @@c.isSetUnits()
    i = @@c.unsetUnits()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetUnits()
  end

  def test_Compartment_setUnits3
    i = @@c.setUnits("")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@c.isSetUnits()
  end

  def test_Compartment_setVolume1
    i = @@c.setVolume(2.0)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( @@c.getVolume() == 2.0 )
    assert_equal true, @@c.isSetVolume()
    i = @@c.unsetVolume()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( @@c.getVolume() == 1.0 )
    assert_equal true, @@c.isSetVolume()
  end

  def test_Compartment_setVolume2
    c = LibSBML::Compartment.new(2,2)
    i = c.setVolume(4)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( c.getVolume() == 4.0 )
    assert_equal true, c.isSetVolume()
    i = c.unsetVolume()
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, c.isSetVolume()
    c = nil
  end

end