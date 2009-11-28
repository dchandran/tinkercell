#
# @file    TestUnitDefinition.rb
# @brief   SBML UnitDefinition unit tests
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Ben Bornstein 
#
# $Id: TestUnitDefinition.rb 10124 2009-08-28 12:04:51Z sarahkeating $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/ruby/test/sbml/TestUnitDefinition.rb $
#
# This test file was converted from src/sbml/test/TestUnitDefinition.c
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

class TestUnitDefinition < Test::Unit::TestCase

  def setup
    @@ud = LibSBML::UnitDefinition.new(2,4)
    if (@@ud == nil)
    end
  end

  def teardown
    @@ud = nil
  end

  def test_UnitDefinition_addUnit
    u = LibSBML::Unit.new(2,4)
    u.setKind(LibSBML::UNIT_KIND_MOLE)
    @@ud.addUnit(u)
    assert( @@ud.getNumUnits() == 1 )
    u = nil
  end

  def test_UnitDefinition_create
    assert( @@ud.getTypeCode() == LibSBML::SBML_UNIT_DEFINITION )
    assert( @@ud.getMetaId() == "" )
    assert( @@ud.getNotes() == nil )
    assert( @@ud.getAnnotation() == nil )
    assert( @@ud.getId() == "" )
    assert( @@ud.getName() == "" )
    assert_equal false, @@ud.isSetId()
    assert_equal false, @@ud.isSetName()
    assert( @@ud.getNumUnits() == 0 )
  end

  def test_UnitDefinition_createWithNS
    xmlns = LibSBML::XMLNamespaces.new()
    xmlns.add( "http://www.sbml.org", "testsbml")
    sbmlns = LibSBML::SBMLNamespaces.new(2,1)
    sbmlns.addNamespaces(xmlns)
    object = LibSBML::UnitDefinition.new(sbmlns)
    assert( object.getTypeCode() == LibSBML::SBML_UNIT_DEFINITION )
    assert( object.getMetaId() == "" )
    assert( object.getNotes() == nil )
    assert( object.getAnnotation() == nil )
    assert( object.getLevel() == 2 )
    assert( object.getVersion() == 1 )
    assert( object.getNamespaces() != nil )
    assert( object.getNamespaces().getLength() == 2 )
    object = nil
  end

  def test_UnitDefinition_createWithName
    ud = LibSBML::UnitDefinition.new(2,4)
    ud.setName( "mmol_per_liter_per_sec")
    assert( ud.getTypeCode() == LibSBML::SBML_UNIT_DEFINITION )
    assert( ud.getMetaId() == "" )
    assert( ud.getNotes() == nil )
    assert( ud.getAnnotation() == nil )
    assert( ud.getId() == "" )
    assert ((  "mmol_per_liter_per_sec" == ud.getName() ))
    assert_equal true, ud.isSetName()
    assert( ud.getNumUnits() == 0 )
    ud = nil
  end

  def test_UnitDefinition_free_NULL
  end

  def test_UnitDefinition_getUnit
    mole = LibSBML::Unit.new(2,4)
    litre = LibSBML::Unit.new(2,4)
    second = LibSBML::Unit.new(2,4)
    mole.setKind(LibSBML::UnitKind_forName("mole"))
    litre.setKind(LibSBML::UnitKind_forName("litre"))
    second.setKind(LibSBML::UnitKind_forName("second"))
    mole.setScale(-3)
    litre.setExponent(-1)
    second.setExponent(-1)
    @@ud.addUnit(mole)
    @@ud.addUnit(litre)
    @@ud.addUnit(second)
    mole = nil
    litre = nil
    second = nil
    assert( @@ud.getNumUnits() == 3 )
    mole = @@ud.getUnit(0)
    litre = @@ud.getUnit(1)
    second = @@ud.getUnit(2)
    assert( mole.getKind() == LibSBML::UNIT_KIND_MOLE )
    assert( litre.getKind() == LibSBML::UNIT_KIND_LITRE )
    assert( second.getKind() == LibSBML::UNIT_KIND_SECOND )
    assert( mole.getScale() == -3 )
    assert( litre.getExponent() == -1 )
    assert( second.getExponent() == -1 )
  end

  def test_UnitDefinition_isVariantOfArea
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfArea()
    u.setKind(LibSBML::UNIT_KIND_METRE)
    u.setExponent(2)
    assert_equal true, @@ud.isVariantOfArea()
    u.setScale(-1)
    assert_equal true, @@ud.isVariantOfArea()
    u.setMultiplier(2)
    assert_equal true, @@ud.isVariantOfArea()
    u.setOffset(3)
    assert_equal true, @@ud.isVariantOfArea()
    u.setExponent(3)
    assert_equal false, @@ud.isVariantOfArea()
    u.setExponent(2)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfArea()
  end

  def test_UnitDefinition_isVariantOfLength
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfLength()
    u.setKind(LibSBML::UNIT_KIND_METRE)
    u.setExponent(1)
    assert_equal true, @@ud.isVariantOfLength()
    u.setScale(-1)
    assert_equal true, @@ud.isVariantOfLength()
    u.setMultiplier(2)
    assert_equal true, @@ud.isVariantOfLength()
    u.setOffset(3)
    assert_equal true, @@ud.isVariantOfLength()
    u.setExponent(2)
    assert_equal false, @@ud.isVariantOfLength()
    u.setExponent(1)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfLength()
  end

  def test_UnitDefinition_isVariantOfSubstancePerTime_1
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    perTime = @@ud.createUnit()
    perTime.setKind(LibSBML::UnitKind_forName("second"))
    perTime.setExponent(-1)
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfSubstancePerTime()
    u.setKind(LibSBML::UNIT_KIND_MOLE)
    u.setExponent(1)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    u.setScale(-1)
    perTime.setScale(-1)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    u.setMultiplier(2)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    u.setOffset(3)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    u.setExponent(-3)
    assert_equal false, @@ud.isVariantOfSubstancePerTime()
    u.setExponent(1)
    perTime.setExponent(-3)
    assert_equal false, @@ud.isVariantOfSubstancePerTime()
    perTime.setExponent(-1)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    dim = nil
  end

  def test_UnitDefinition_isVariantOfSubstancePerTime_2
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    perTime = @@ud.createUnit()
    perTime.setKind(LibSBML::UnitKind_forName("second"))
    perTime.setExponent(-1)
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfSubstancePerTime()
    u.setKind(LibSBML::UNIT_KIND_ITEM)
    u.setExponent(1)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    u.setScale(-1)
    perTime.setScale(-1)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    u.setMultiplier(2)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    u.setOffset(3)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    u.setExponent(-3)
    assert_equal false, @@ud.isVariantOfSubstancePerTime()
    u.setExponent(1)
    perTime.setExponent(-3)
    assert_equal false, @@ud.isVariantOfSubstancePerTime()
    perTime.setExponent(-1)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfSubstancePerTime()
    dim = nil
  end

  def test_UnitDefinition_isVariantOfSubstancePerTime_3
    ud = LibSBML::UnitDefinition.new(2,2)
    dim = LibSBML::Unit.new(2,2)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    perTime = ud.createUnit()
    perTime.setKind(LibSBML::UnitKind_forName("second"))
    perTime.setExponent(-1)
    u = ud.createUnit()
    assert_equal false, ud.isVariantOfSubstancePerTime()
    u.setKind(LibSBML::UNIT_KIND_GRAM)
    u.setExponent(1)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    u.setScale(-1)
    perTime.setScale(-1)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    u.setMultiplier(2)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    u.setOffset(3)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    u.setExponent(-3)
    assert_equal false, ud.isVariantOfSubstancePerTime()
    u.setExponent(1)
    perTime.setExponent(-3)
    assert_equal false, ud.isVariantOfSubstancePerTime()
    perTime.setExponent(-1)
    ud.addUnit(dim)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    ud = nil
    dim = nil
  end

  def test_UnitDefinition_isVariantOfSubstancePerTime_4
    ud = LibSBML::UnitDefinition.new(2,2)
    dim = ud.createUnit()
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    perTime = ud.createUnit()
    perTime.setKind(LibSBML::UnitKind_forName("second"))
    perTime.setExponent(-1)
    u = ud.createUnit()
    assert_equal false, ud.isVariantOfSubstancePerTime()
    u.setKind(LibSBML::UNIT_KIND_KILOGRAM)
    u.setExponent(1)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    u.setScale(-1)
    perTime.setScale(-1)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    u.setMultiplier(2)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    u.setOffset(3)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    u.setExponent(-3)
    assert_equal false, ud.isVariantOfSubstancePerTime()
    u.setExponent(1)
    perTime.setExponent(-3)
    assert_equal false, ud.isVariantOfSubstancePerTime()
    perTime.setExponent(-1)
    ud.addUnit(dim)
    assert_equal true, ud.isVariantOfSubstancePerTime()
    ud = nil
  end

  def test_UnitDefinition_isVariantOfSubstance_1
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfSubstance()
    u.setKind(LibSBML::UNIT_KIND_MOLE)
    u.setExponent(1)
    assert_equal true, @@ud.isVariantOfSubstance()
    u.setScale(-1)
    assert_equal true, @@ud.isVariantOfSubstance()
    u.setMultiplier(2)
    assert_equal true, @@ud.isVariantOfSubstance()
    u.setOffset(3)
    assert_equal true, @@ud.isVariantOfSubstance()
    u.setExponent(-3)
    assert_equal false, @@ud.isVariantOfSubstance()
    u.setExponent(1)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfSubstance()
  end

  def test_UnitDefinition_isVariantOfSubstance_2
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfSubstance()
    u.setKind(LibSBML::UNIT_KIND_ITEM)
    u.setExponent(1)
    assert_equal true, @@ud.isVariantOfSubstance()
    u.setScale(-1)
    assert_equal true, @@ud.isVariantOfSubstance()
    u.setMultiplier(2)
    assert_equal true, @@ud.isVariantOfSubstance()
    u.setOffset(3)
    assert_equal true, @@ud.isVariantOfSubstance()
    u.setExponent(-2)
    assert_equal false, @@ud.isVariantOfSubstance()
    u.setExponent(1)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfSubstance()
  end

  def test_UnitDefinition_isVariantOfTime
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfTime()
    u.setKind(LibSBML::UNIT_KIND_SECOND)
    u.setExponent(1)
    assert_equal true, @@ud.isVariantOfTime()
    u.setScale(-10)
    assert_equal true, @@ud.isVariantOfTime()
    u.setMultiplier(10)
    assert_equal true, @@ud.isVariantOfTime()
    u.setOffset(30)
    assert_equal true, @@ud.isVariantOfTime()
    u.setExponent(2)
    assert_equal false, @@ud.isVariantOfTime()
    u.setExponent(1)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfTime()
  end

  def test_UnitDefinition_isVariantOfVolume_1
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfVolume()
    u.setKind(LibSBML::UNIT_KIND_LITRE)
    u.setExponent(1)
    assert_equal true, @@ud.isVariantOfVolume()
    u.setScale(100)
    assert_equal true, @@ud.isVariantOfVolume()
    u.setMultiplier(5)
    assert_equal true, @@ud.isVariantOfVolume()
    u.setOffset(-5)
    assert_equal true, @@ud.isVariantOfVolume()
    u.setExponent(-1)
    assert_equal false, @@ud.isVariantOfVolume()
    u.setExponent(1)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfVolume()
  end

  def test_UnitDefinition_isVariantOfVolume_2
    dim = LibSBML::Unit.new(2,4)
    dim.setKind(LibSBML::UnitKind_forName("dimensionless"))
    u = @@ud.createUnit()
    assert_equal false, @@ud.isVariantOfVolume()
    u.setKind(LibSBML::UNIT_KIND_METRE)
    u.setExponent(3)
    assert_equal true, @@ud.isVariantOfVolume()
    u.setScale(100)
    assert_equal true, @@ud.isVariantOfVolume()
    u.setMultiplier(5)
    assert_equal true, @@ud.isVariantOfVolume()
    u.setOffset(-5)
    assert_equal true, @@ud.isVariantOfVolume()
    u.setExponent(2)
    assert_equal false, @@ud.isVariantOfVolume()
    u.setExponent(3)
    @@ud.addUnit(dim)
    assert_equal true, @@ud.isVariantOfVolume()
  end

  def test_UnitDefinition_printUnits
    ud = LibSBML::UnitDefinition.new(2,4)
    ud.setId( "mmls")
    perTime = ud.createUnit()
    perTime.setKind(LibSBML::UnitKind_forName("second"))
    perTime.setExponent(-1)
    ud_str = LibSBML::UnitDefinition.printUnits(ud,false)
    assert ((                 "second (exponent = -1, multiplier = 1, scale = 0)" == ud_str ))
    ud_str1 = LibSBML::UnitDefinition.printUnits(ud,true)
    assert ((  "(1 second)^-1" == ud_str1 ))
    ud1 = LibSBML::UnitDefinition.new(2,4)
    ud1.setId( "mmls")
    u = ud1.createUnit()
    u.setKind(LibSBML::UNIT_KIND_KILOGRAM)
    u.setExponent(1)
    u.setScale(2)
    u.setMultiplier(3.0)
    ud_str2 = LibSBML::UnitDefinition.printUnits(ud1,false)
    assert ((                 "kilogram (exponent = 1, multiplier = 3, scale = 2)" == ud_str2 ))
    ud_str3 = LibSBML::UnitDefinition.printUnits(ud1,true)
    assert ((  "(300 kilogram)^1" == ud_str3 ))
  end

  def test_UnitDefinition_removeUnit
    o1 = @@ud.createUnit()
    o2 = @@ud.createUnit()
    o3 = @@ud.createUnit()
    assert( @@ud.removeUnit(0) == o1 )
    assert( @@ud.getNumUnits() == 2 )
    assert( @@ud.removeUnit(0) == o2 )
    assert( @@ud.getNumUnits() == 1 )
    assert( @@ud.removeUnit(0) == o3 )
    assert( @@ud.getNumUnits() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_UnitDefinition_setId
    id =  "mmls";
    @@ud.setId(id)
    assert (( id == @@ud.getId() ))
    assert_equal true, @@ud.isSetId()
    if (@@ud.getId() == id)
    end
    @@ud.setId(@@ud.getId())
    assert (( id == @@ud.getId() ))
    @@ud.setId("")
    assert_equal false, @@ud.isSetId()
    if (@@ud.getId() != nil)
    end
  end

  def test_UnitDefinition_setName
    name =  "mmol_per_liter_per_sec";
    @@ud.setName(name)
    assert (( name == @@ud.getName() ))
    assert_equal true, @@ud.isSetName()
    if (@@ud.getName() == name)
    end
    @@ud.setName(@@ud.getName())
    assert (( name == @@ud.getName() ))
    @@ud.setName("")
    assert_equal false, @@ud.isSetName()
    if (@@ud.getName() != nil)
    end
  end

end
