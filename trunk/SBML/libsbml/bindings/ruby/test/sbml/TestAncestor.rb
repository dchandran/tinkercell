#
# @file    TestAncestor.rb
# @brief   SBML ancestor objects unit tests
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Sarah Keating 
#
# $Id: TestAncestor.rb 10124 2009-08-28 12:04:51Z sarahkeating $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/ruby/test/sbml/TestAncestor.rb $
#
# This test file was converted from src/sbml/test/TestAncestor.cpp
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

class TestAncestor < Test::Unit::TestCase

  def test_AlgebraicRule_ancestor_create
    m = LibSBML::Model.new(2,4)
    r = m.createAlgebraicRule()
    lo = m.getListOfRules()
    assert( r.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( r.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( r.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( r.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getRule(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_AssignmentRule_ancestor_create
    m = LibSBML::Model.new(2,4)
    r = m.createAssignmentRule()
    lo = m.getListOfRules()
    assert( r.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( r.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( r.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( r.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getRule(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_CompartmentType_ancestor_add
    ct = LibSBML::CompartmentType.new(2,4)
    m = LibSBML::Model.new(2,4)
    ct.setId("ct")
    m.addCompartmentType(ct)
    ct = nil
    lo = m.getListOfCompartmentTypes()
    obj = m.getCompartmentType(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_CompartmentType_ancestor_create
    m = LibSBML::Model.new(2,4)
    ct = m.createCompartmentType()
    lo = m.getListOfCompartmentTypes()
    assert( ct.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( ct.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( ct.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ct.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getCompartmentType(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Compartment_ancestor_add
    c = LibSBML::Compartment.new(2,4)
    c.setId("C")
    m = LibSBML::Model.new(2,4)
    m.addCompartment(c)
    c = nil
    lo = m.getListOfCompartments()
    obj = m.getCompartment(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Compartment_ancestor_create
    m = LibSBML::Model.new(2,4)
    c = m.createCompartment()
    lo = m.getListOfCompartments()
    assert( c.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( c.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( c.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( c.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getCompartment(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Constraint_ancestor_add
    ct = LibSBML::Constraint.new(2,4)
    m = LibSBML::Model.new(2,4)
    ct.setMath(LibSBML::parseFormula("k+k"))
    m.addConstraint(ct)
    ct = nil
    lo = m.getListOfConstraints()
    obj = m.getConstraint(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Constraint_ancestor_create
    m = LibSBML::Model.new(2,4)
    ct = m.createConstraint()
    lo = m.getListOfConstraints()
    assert( ct.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( ct.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( ct.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ct.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getConstraint(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Delay_ancestor_add
    d = LibSBML::Delay.new(2,4)
    e = LibSBML::Event.new(2,4)
    e.setDelay(d)
    d = nil
    obj = e.getDelay()
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    e = nil
  end

  def test_Delay_ancestor_create
    e = LibSBML::Event.new(2,4)
    ea = e.createDelay()
    assert( ea.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( ea.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ea.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = e.getDelay()
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_Delay_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    e = m.createEvent()
    ea = m.createDelay()
    assert( ea.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( ea.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( ea.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ea.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = e.getDelay()
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_EventAssignment_ancestor_add
    e = LibSBML::Event.new(2,4)
    ea = LibSBML::EventAssignment.new(2,4)
    ea.setVariable("c")
    ea.setMath(LibSBML::parseFormula("K+L"))
    e.addEventAssignment(ea)
    ea = nil
    lo = e.getListOfEventAssignments()
    obj = e.getEventAssignment(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_EventAssignment_ancestor_create
    e = LibSBML::Event.new(2,4)
    ea = e.createEventAssignment()
    lo = e.getListOfEventAssignments()
    assert( ea.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( ea.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( ea.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ea.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = e.getEventAssignment(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_EventAssignment_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    e = m.createEvent()
    ea = m.createEventAssignment()
    lo = e.getListOfEventAssignments()
    assert( ea.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( ea.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( ea.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( ea.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ea.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = e.getEventAssignment(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_Event_ancestor_add
    e = LibSBML::Event.new(2,4)
    m = LibSBML::Model.new(2,4)
    t = LibSBML::Trigger.new(2,4)
    e.setTrigger(t)
    e.createEventAssignment()
    m.addEvent(e)
    e = nil
    lo = m.getListOfEvents()
    obj = m.getEvent(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_Event_ancestor_create
    m = LibSBML::Model.new(2,4)
    e = m.createEvent()
    lo = m.getListOfEvents()
    assert( e.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( e.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( e.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( e.getAncestorOfType(LibSBML::SBML_PARAMETER) == nil )
    obj = m.getEvent(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_FunctionDefinition_ancestor_add
    fd = LibSBML::FunctionDefinition.new(2,4)
    m = LibSBML::Model.new(2,4)
    fd.setId("fd")
    fd.setMath(LibSBML::parseFormula("l"))
    m.addFunctionDefinition(fd)
    fd = nil
    lo = m.getListOfFunctionDefinitions()
    obj = m.getFunctionDefinition(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_FunctionDefinition_ancestor_create
    m = LibSBML::Model.new(2,4)
    fd = m.createFunctionDefinition()
    lo = m.getListOfFunctionDefinitions()
    assert( fd.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( fd.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( fd.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( fd.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getFunctionDefinition(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_InitialAssignment_ancestor_add
    ia = LibSBML::InitialAssignment.new(2,4)
    m = LibSBML::Model.new(2,4)
    ia.setSymbol("c")
    ia.setMath(LibSBML::parseFormula("9"))
    m.addInitialAssignment(ia)
    ia = nil
    lo = m.getListOfInitialAssignments()
    obj = m.getInitialAssignment(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_InitialAssignment_ancestor_create
    m = LibSBML::Model.new(2,4)
    ia = m.createInitialAssignment()
    lo = m.getListOfInitialAssignments()
    assert( ia.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( ia.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( ia.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ia.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getInitialAssignment(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_KineticLaw_Parameter_ancestor_add
    kl = LibSBML::KineticLaw.new(2,4)
    p = LibSBML::Parameter.new(2,4)
    p.setId("jake")
    kl.addParameter(p)
    p = nil
    lop = kl.getListOfParameters()
    obj = kl.getParameter(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_KINETIC_LAW) == kl )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lop )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    kl = nil
  end

  def test_KineticLaw_Parameter_ancestor_create
    kl = LibSBML::KineticLaw.new(2,4)
    p = kl.createParameter()
    assert( kl.getNumParameters() == 1 )
    lop = kl.getListOfParameters()
    assert( p.getAncestorOfType(LibSBML::SBML_KINETIC_LAW) == kl )
    assert( p.getAncestorOfType(LibSBML::SBML_LIST_OF) == lop )
    assert( p.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( p.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = kl.getParameter(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_KINETIC_LAW) == kl )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lop )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    kl = nil
  end

  def test_KineticLaw_Parameter_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    r = m.createReaction()
    kl = m.createKineticLaw()
    p = m.createKineticLawParameter()
    assert( kl.getNumParameters() == 1 )
    lop = kl.getListOfParameters()
    assert( p.getAncestorOfType(LibSBML::SBML_KINETIC_LAW) == kl )
    assert( p.getAncestorOfType(LibSBML::SBML_LIST_OF) == lop )
    assert( p.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( p.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( p.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( p.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = kl.getParameter(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_KINETIC_LAW) == kl )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lop )
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    kl = nil
  end

  def test_KineticLaw_ancestor_add
    kl = LibSBML::KineticLaw.new(2,4)
    r = LibSBML::Reaction.new(2,4)
    r.setKineticLaw(kl)
    obj = r.getKineticLaw()
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    r = nil
  end

  def test_KineticLaw_ancestor_create
    r = LibSBML::Reaction.new(2,4)
    kl = r.createKineticLaw()
    assert( kl.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( kl.getAncestorOfType(LibSBML::SBML_DELAY) == nil )
    assert( kl.getAncestorOfType(LibSBML::SBML_MODEL) == nil )
    assert( kl.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    obj = r.getKineticLaw()
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_DELAY) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    r = nil
  end

  def test_KineticLaw_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    r = m.createReaction()
    kl = r.createKineticLaw()
    assert( kl.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( kl.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( kl.getAncestorOfType(LibSBML::SBML_DELAY) == nil )
    assert( kl.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    obj = r.getKineticLaw()
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_DELAY) == nil )
    r = nil
  end

  def test_Model_ancestor_add
    d = LibSBML::SBMLDocument.new(2,4)
    m = LibSBML::Model.new(2,4)
    d.setModel(m)
    assert( d == d.getModel().getAncestorOfType(LibSBML::SBML_DOCUMENT) )
    d = nil
  end

  def test_Model_ancestor_create
    d = LibSBML::SBMLDocument.new()
    m = d.createModel()
    assert( m.getAncestorOfType(LibSBML::SBML_DOCUMENT) == d )
    obj = d.getModel()
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == d )
    d = nil
  end

  def test_Parameter_ancestor_add
    ia = LibSBML::Parameter.new(2,4)
    m = LibSBML::Model.new(2,4)
    ia.setId("p")
    m.addParameter(ia)
    ia = nil
    lo = m.getListOfParameters()
    obj = m.getParameter(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Parameter_ancestor_create
    m = LibSBML::Model.new(2,4)
    p = m.createParameter()
    lo = m.getListOfParameters()
    assert( p.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( p.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( p.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( p.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getParameter(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_RateRule_ancestor_create
    m = LibSBML::Model.new(2,4)
    r = m.createRateRule()
    lo = m.getListOfRules()
    assert( r.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( r.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( r.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( r.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getRule(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Reaction_ancestor_add
    ia = LibSBML::Reaction.new(2,4)
    m = LibSBML::Model.new(2,4)
    ia.setId("k")
    m.addReaction(ia)
    ia = nil
    lo = m.getListOfReactions()
    obj = m.getReaction(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Reaction_ancestor_create
    m = LibSBML::Model.new(2,4)
    r = m.createReaction()
    lo = m.getListOfReactions()
    assert( r.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( r.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( r.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( r.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getReaction(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Rule_ancestor_add
    ia = LibSBML::RateRule.new(2,4)
    ia.setVariable("a")
    ia.setMath(LibSBML::parseFormula("9"))
    m = LibSBML::Model.new(2,4)
    m.addRule(ia)
    ia = nil
    lo = m.getListOfRules()
    obj = m.getRule(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_SpeciesReference_Modifier_ancestor_add
    sr = LibSBML::ModifierSpeciesReference.new(2,4)
    sr.setSpecies("s")
    r = LibSBML::Reaction.new(2,4)
    r.addModifier(sr)
    sr = nil
    lo = r.getListOfModifiers()
    obj = r.getModifier(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesReference_Modifier_ancestor_create
    r = LibSBML::Reaction.new(2,4)
    sr = r.createModifier()
    lo = r.getListOfModifiers()
    assert( sr.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( sr.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( sr.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( sr.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = r.getModifier(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesReference_Modifier_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    r = m.createReaction()
    sr = m.createModifier()
    lo = r.getListOfModifiers()
    assert( sr.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( sr.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( sr.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( sr.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( sr.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = r.getModifier(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesReference_Product_ancestor_add
    sr = LibSBML::SpeciesReference.new(2,4)
    r = LibSBML::Reaction.new(2,4)
    sr.setSpecies("p")
    r.addProduct(sr)
    sr = nil
    lo = r.getListOfProducts()
    obj = r.getProduct(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesReference_Product_ancestor_create
    r = LibSBML::Reaction.new(2,4)
    sr = r.createProduct()
    lo = r.getListOfProducts()
    assert( sr.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( sr.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( sr.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( sr.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = r.getProduct(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesReference_Product_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    r = m.createReaction()
    sr = m.createProduct()
    lo = r.getListOfProducts()
    assert( sr.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( sr.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( sr.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( sr.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( sr.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = r.getProduct(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesReference_Reactant_ancestor_add
    sr = LibSBML::SpeciesReference.new(2,4)
    r = LibSBML::Reaction.new(2,4)
    sr.setSpecies("s")
    r.addReactant(sr)
    sr = nil
    lo = r.getListOfReactants()
    obj = r.getReactant(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesReference_Reactant_ancestor_create
    r = LibSBML::Reaction.new(2,4)
    sr = r.createReactant()
    lo = r.getListOfReactants()
    assert( sr.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( sr.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( sr.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( sr.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = r.getReactant(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesReference_Reactant_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    r = m.createReaction()
    sr = m.createReactant()
    lo = r.getListOfReactants()
    assert( sr.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( sr.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( sr.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( sr.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( sr.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = r.getReactant(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_REACTION) == r )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_SpeciesType_ancestor_add
    ia = LibSBML::SpeciesType.new(2,4)
    m = LibSBML::Model.new(2,4)
    ia.setId("s")
    m.addSpeciesType(ia)
    ia = nil
    lo = m.getListOfSpeciesTypes()
    obj = m.getSpeciesType(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_SpeciesType_ancestor_create
    m = LibSBML::Model.new(2,4)
    st = m.createSpeciesType()
    lo = m.getListOfSpeciesTypes()
    assert( st.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( st.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( st.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( st.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getSpeciesType(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Species_ancestor_add
    ia = LibSBML::Species.new(2,4)
    m = LibSBML::Model.new(2,4)
    ia.setId("s")
    ia.setCompartment("c")
    m.addSpecies(ia)
    ia = nil
    lo = m.getListOfSpecies()
    obj = m.getSpecies(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Species_ancestor_create
    m = LibSBML::Model.new(2,4)
    s = m.createSpecies()
    lo = m.getListOfSpecies()
    assert( s.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( s.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( s.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( s.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getSpecies(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_StoichiometryMath_ancestor_add
    m = LibSBML::StoichiometryMath.new(2,4)
    sr = LibSBML::SpeciesReference.new(2,4)
    sr.setStoichiometryMath(m)
    m = nil
    obj = sr.getStoichiometryMath()
    assert( obj.getAncestorOfType(LibSBML::SBML_SPECIES_REFERENCE) == sr )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    sr = nil
  end

  def test_StoichiometryMath_ancestor_create
    sr = LibSBML::SpeciesReference.new(2,4)
    sm = sr.createStoichiometryMath()
    assert( sm.getAncestorOfType(LibSBML::SBML_SPECIES_REFERENCE) == sr )
    assert( sm.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( sm.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = sr.getStoichiometryMath()
    assert( obj.getAncestorOfType(LibSBML::SBML_SPECIES_REFERENCE) == sr )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_Trigger_ancestor_add
    d = LibSBML::Trigger.new(2,4)
    e = LibSBML::Event.new(2,4)
    e.setTrigger(d)
    d = nil
    obj = e.getTrigger()
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    e = nil
  end

  def test_Trigger_ancestor_create
    e = LibSBML::Event.new(2,4)
    ea = e.createTrigger()
    assert( ea.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( ea.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ea.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = e.getTrigger()
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_Trigger_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    e = m.createEvent()
    ea = m.createTrigger()
    assert( ea.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( ea.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( ea.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ea.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = e.getTrigger()
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == e )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
  end

  def test_UnitDefinition_ancestor_add
    ia = LibSBML::UnitDefinition.new(2,4)
    m = LibSBML::Model.new(2,4)
    ia.setId("u")
    ia.createUnit()
    m.addUnitDefinition(ia)
    ia = nil
    lo = m.getListOfUnitDefinitions()
    obj = m.getUnitDefinition(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_UnitDefinition_ancestor_create
    m = LibSBML::Model.new(2,4)
    ud = m.createUnitDefinition()
    lo = m.getListOfUnitDefinitions()
    assert( ud.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( ud.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( ud.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( ud.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
    obj = m.getUnitDefinition(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_EVENT) == nil )
  end

  def test_Unit_ancestor_add
    ud = LibSBML::UnitDefinition.new(2,4)
    u = LibSBML::Unit.new(2,4)
    u.setKind(LibSBML::UNIT_KIND_MOLE)
    ud.addUnit(u)
    u = nil
    assert( ud.getNumUnits() == 1 )
    lo = ud.getListOfUnits()
    obj = ud.getUnit(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_UNIT_DEFINITION) == ud )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    ud = nil
  end

  def test_Unit_ancestor_create
    ud = LibSBML::UnitDefinition.new(2,4)
    u = ud.createUnit()
    assert( ud.getNumUnits() == 1 )
    lo = ud.getListOfUnits()
    assert( u.getAncestorOfType(LibSBML::SBML_UNIT_DEFINITION) == ud )
    assert( u.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( u.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( u.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = ud.getUnit(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_UNIT_DEFINITION) == ud )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    ud = nil
  end

  def test_Unit_ancestor_create_model
    m = LibSBML::Model.new(2,4)
    ud = m.createUnitDefinition()
    u = m.createUnit()
    assert( ud.getNumUnits() == 1 )
    lo = ud.getListOfUnits()
    assert( u.getAncestorOfType(LibSBML::SBML_UNIT_DEFINITION) == ud )
    assert( u.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( u.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( u.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( u.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    obj = ud.getUnit(0)
    assert( obj.getAncestorOfType(LibSBML::SBML_UNIT_DEFINITION) == ud )
    assert( obj.getAncestorOfType(LibSBML::SBML_LIST_OF) == lo )
    assert( obj.getAncestorOfType(LibSBML::SBML_DOCUMENT) == nil )
    assert( obj.getAncestorOfType(LibSBML::SBML_MODEL) == m )
    assert( obj.getAncestorOfType(LibSBML::SBML_COMPARTMENT) == nil )
    ud = nil
  end

end
