#
# @file    TestRequiredElements.rb
# @brief   Test hasRequiredElements unit tests
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Sarah Keating 
#
# $Id$
# $HeadURL$
#
# This test file was converted from src/sbml/test/TestRequiredElements.cpp
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

class TestRequiredElements < Test::Unit::TestCase

  def test_AlgebraicRule
    ar = LibSBML::AlgebraicRule.new(2,4)
    assert_equal false, (ar.hasRequiredElements())
    ar.setMath(LibSBML::parseFormula("ar"))
    assert_equal true, ar.hasRequiredElements()
    ar = nil
  end

  def test_AssignmentRule
    r = LibSBML::AssignmentRule.new(2,4)
    assert_equal false, (r.hasRequiredElements())
    r.setMath(LibSBML::parseFormula("ar"))
    assert_equal true, r.hasRequiredElements()
    r = nil
  end

  def test_Compartment
    c = LibSBML::Compartment.new(2,4)
    assert_equal true, c.hasRequiredElements()
    c = nil
  end

  def test_CompartmentType
    ct = LibSBML::CompartmentType.new(2,4)
    assert_equal true, ct.hasRequiredElements()
    ct = nil
  end

  def test_Constraint
    c = LibSBML::Constraint.new(2,4)
    assert_equal false, (c.hasRequiredElements())
    c.setMath(LibSBML::parseFormula("a+b"))
    assert_equal true, c.hasRequiredElements()
    c = nil
  end

  def test_Delay
    d = LibSBML::Delay.new(2,4)
    assert_equal false, (d.hasRequiredElements())
    d.setMath(LibSBML::parseFormula("a+b"))
    assert_equal true, d.hasRequiredElements()
    d = nil
  end

  def test_Event
    e = LibSBML::Event.new(2,4)
    assert_equal false, (e.hasRequiredElements())
    t = LibSBML::Trigger.new(2,4)
    e.setTrigger(t)
    assert_equal false, (e.hasRequiredElements())
    e.createEventAssignment()
    assert_equal true, e.hasRequiredElements()
    e = nil
  end

  def test_EventAssignment
    ea = LibSBML::EventAssignment.new(2,4)
    assert_equal false, (ea.hasRequiredElements())
    ea.setMath(LibSBML::parseFormula("fd"))
    assert_equal true, ea.hasRequiredElements()
    ea = nil
  end

  def test_FunctionDefinition
    fd = LibSBML::FunctionDefinition.new(2,4)
    assert_equal false, (fd.hasRequiredElements())
    fd.setMath(LibSBML::parseFormula("fd"))
    assert_equal true, fd.hasRequiredElements()
    fd = nil
  end

  def test_InitialAssignment
    ia = LibSBML::InitialAssignment.new(2,4)
    assert_equal false, (ia.hasRequiredElements())
    ia.setMath(LibSBML::parseFormula("ia"))
    assert_equal true, ia.hasRequiredElements()
    ia = nil
  end

  def test_KineticLaw
    kl = LibSBML::KineticLaw.new(2,4)
    assert_equal false, (kl.hasRequiredElements())
    kl.setMath(LibSBML::parseFormula("kl"))
    assert_equal true, kl.hasRequiredElements()
    kl = nil
  end

  def test_Model
    m = LibSBML::Model.new(2,4)
    assert_equal true, m.hasRequiredElements()
    m = nil
  end

  def test_Model_L1V1
    m = LibSBML::Model.new(1,1)
    assert_equal false, (m.hasRequiredElements())
    m.createCompartment()
    assert_equal false, (m.hasRequiredElements())
    m.createSpecies()
    assert_equal false, (m.hasRequiredElements())
    m.createReaction()
    assert_equal true, m.hasRequiredElements()
    m = nil
  end

  def test_Model_L1V2
    m = LibSBML::Model.new(1,2)
    assert_equal false, (m.hasRequiredElements())
    m.createCompartment()
    assert_equal true, m.hasRequiredElements()
    m = nil
  end

  def test_ModifierSpeciesReference
    msr = LibSBML::ModifierSpeciesReference.new(2,4)
    assert_equal true, msr.hasRequiredElements()
    msr = nil
  end

  def test_Parameter
    p = LibSBML::Parameter.new(2,4)
    assert_equal true, p.hasRequiredElements()
    p = nil
  end

  def test_RateRule
    r = LibSBML::RateRule.new(2,4)
    assert_equal false, (r.hasRequiredElements())
    r.setMath(LibSBML::parseFormula("ar"))
    assert_equal true, r.hasRequiredElements()
    r = nil
  end

  def test_Reaction
    r = LibSBML::Reaction.new(2,4)
    assert_equal true, r.hasRequiredElements()
    r = nil
  end

  def test_Species
    s = LibSBML::Species.new(2,4)
    assert_equal true, s.hasRequiredElements()
    s = nil
  end

  def test_SpeciesReference
    sr = LibSBML::SpeciesReference.new(2,4)
    assert_equal true, sr.hasRequiredElements()
    sr = nil
  end

  def test_SpeciesType
    st = LibSBML::SpeciesType.new(2,4)
    assert_equal true, st.hasRequiredElements()
    st = nil
  end

  def test_StoichiometryMath
    sm = LibSBML::StoichiometryMath.new(2,4)
    assert_equal false, (sm.hasRequiredElements())
    sm.setMath(LibSBML::parseFormula("ar"))
    assert_equal true, sm.hasRequiredElements()
    sm = nil
  end

  def test_Trigger
    t = LibSBML::Trigger.new(2,4)
    assert_equal false, (t.hasRequiredElements())
    t.setMath(LibSBML::parseFormula("ar"))
    assert_equal true, t.hasRequiredElements()
    t = nil
  end

  def test_Unit
    u = LibSBML::Unit.new(2,4)
    assert_equal true, u.hasRequiredElements()
    u = nil
  end

  def test_UnitDefinition
    ud = LibSBML::UnitDefinition.new(2,4)
    assert_equal false, (ud.hasRequiredElements())
    ud.createUnit()
    assert_equal true, ud.hasRequiredElements()
    ud = nil
  end

  def test_UnitDefinition_L1
    ud = LibSBML::UnitDefinition.new(1,2)
    assert_equal true, ud.hasRequiredElements()
    ud = nil
  end

end
