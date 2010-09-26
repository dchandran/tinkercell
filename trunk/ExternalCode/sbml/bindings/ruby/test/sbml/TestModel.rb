#
# @file    TestModel.rb
# @brief   SBML Model unit tests
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Ben Bornstein 
#
# $Id: TestModel.rb 10124 2009-08-28 12:04:51Z sarahkeating $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/ruby/test/sbml/TestModel.rb $
#
# This test file was converted from src/sbml/test/TestModel.c
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

class TestModel < Test::Unit::TestCase

  def setup
    @@m = LibSBML::Model.new(2,4)
    if (@@m == nil)
    end
  end

  def teardown
    @@m = nil
  end

  def test_KineticLaw_getParameterById
    k1 = LibSBML::Parameter.new(2,4)
    k2 = LibSBML::Parameter.new(2,4)
    k1.setId( "k1")
    k2.setId( "k2")
    k1.setValue(3.14)
    k2.setValue(2.72)
    @@m.addParameter(k1)
    @@m.addParameter(k2)
    r1 = LibSBML::Reaction.new(2,4)
    r1.setId( "reaction_1" )
    kl = LibSBML::KineticLaw.new(2,4)
    kl.setFormula( "k1 * X0")
    k3 = LibSBML::Parameter.new(2,4)
    k4 = LibSBML::Parameter.new(2,4)
    k3.setId( "k1")
    k4.setId( "k2")
    k3.setValue(2.72)
    k4.setValue(3.14)
    kl.addParameter(k3)
    kl.addParameter(k4)
    r1.setKineticLaw(kl)
    @@m.addReaction(r1)
    kl1 = @@m.getReaction(0).getKineticLaw()
    assert( kl1.getParameter( "k1" ) != k3 )
    assert( kl1.getParameter( "k1" ) != k1 )
    assert( kl1.getParameter( "k2" ) != k4 )
    assert( kl1.getParameter( "k3" ) == nil )
  end

  def test_Model_addCompartment
    c = LibSBML::Compartment.new(2,4)
    c.setId( "c")
    @@m.addCompartment(c)
    assert( @@m.getNumCompartments() == 1 )
  end

  def test_Model_addParameter
    p = LibSBML::Parameter.new(2,4)
    p.setId( "p")
    @@m.addParameter(p)
    assert( @@m.getNumParameters() == 1 )
  end

  def test_Model_addReaction
    r = LibSBML::Reaction.new(2,4)
    r.setId( "r")
    @@m.addReaction(r)
    assert( @@m.getNumReactions() == 1 )
  end

  def test_Model_addRules
    r1 = LibSBML::AlgebraicRule.new(2,4)
    r2 = LibSBML::AssignmentRule.new(2,4)
    r3 = LibSBML::RateRule.new(2,4)
    r2.setVariable( "r2")
    r3.setVariable( "r3")
    r1.setMath(LibSBML::parseFormula("2"))
    r2.setMath(LibSBML::parseFormula("2"))
    r3.setMath(LibSBML::parseFormula("2"))
    @@m.addRule(r1)
    @@m.addRule(r2)
    @@m.addRule(r3)
    assert( @@m.getNumRules() == 3 )
  end

  def test_Model_addSpecies
    s = LibSBML::Species.new(2,4)
    s.setId( "s")
    s.setCompartment( "c")
    @@m.addSpecies(s)
    assert( @@m.getNumSpecies() == 1 )
  end

  def test_Model_add_get_Event
    e1 = LibSBML::Event.new(2,4)
    e2 = LibSBML::Event.new(2,4)
    t = LibSBML::Trigger.new(2,4)
    e1.setTrigger(t)
    e2.setTrigger(t)
    e1.createEventAssignment()
    e2.createEventAssignment()
    @@m.addEvent(e1)
    @@m.addEvent(e2)
    assert( @@m.getNumEvents() == 2 )
    assert( @@m.getEvent(0) != e1 )
    assert( @@m.getEvent(1) != e2 )
    assert( @@m.getEvent(2) == nil )
  end

  def test_Model_add_get_FunctionDefinitions
    fd1 = LibSBML::FunctionDefinition.new(2,4)
    fd2 = LibSBML::FunctionDefinition.new(2,4)
    fd1.setId( "fd1")
    fd2.setId( "fd2")
    fd1.setMath(LibSBML::parseFormula("2"))
    fd2.setMath(LibSBML::parseFormula("2"))
    @@m.addFunctionDefinition(fd1)
    @@m.addFunctionDefinition(fd2)
    assert( @@m.getNumFunctionDefinitions() == 2 )
    assert( @@m.getFunctionDefinition(0) != fd1 )
    assert( @@m.getFunctionDefinition(1) != fd2 )
    assert( @@m.getFunctionDefinition(2) == nil )
  end

  def test_Model_add_get_UnitDefinitions
    ud1 = LibSBML::UnitDefinition.new(2,4)
    ud2 = LibSBML::UnitDefinition.new(2,4)
    ud1.setId( "ud1")
    ud2.setId( "ud2")
    ud1.createUnit()
    ud2.createUnit()
    @@m.addUnitDefinition(ud1)
    @@m.addUnitDefinition(ud2)
    assert( @@m.getNumUnitDefinitions() == 2 )
    assert( @@m.getUnitDefinition(0) != ud1 )
    assert( @@m.getUnitDefinition(1) != ud2 )
    assert( @@m.getUnitDefinition(2) == nil )
  end

  def test_Model_create
    assert( @@m.getTypeCode() == LibSBML::SBML_MODEL )
    assert( @@m.getMetaId() == "" )
    assert( @@m.getNotes() == nil )
    assert( @@m.getAnnotation() == nil )
    assert( @@m.getId() == "" )
    assert( @@m.getName() == "" )
    assert_equal false, @@m.isSetId()
    assert_equal false, @@m.isSetName()
    assert( @@m.getNumUnitDefinitions() == 0 )
    assert( @@m.getNumCompartments() == 0 )
    assert( @@m.getNumSpecies() == 0 )
    assert( @@m.getNumParameters() == 0 )
    assert( @@m.getNumReactions() == 0 )
  end

  def test_Model_createAlgebraicRule
    ar = @@m.createAlgebraicRule()
    assert( ar != nil )
    assert( @@m.getNumRules() == 1 )
    assert( @@m.getRule(0) == ar )
  end

  def test_Model_createAssignmentRule
    ar = @@m.createAssignmentRule()
    assert( ar != nil )
    assert( @@m.getNumRules() == 1 )
    assert( @@m.getRule(0) == ar )
  end

  def test_Model_createCompartment
    c = @@m.createCompartment()
    assert( c != nil )
    assert( @@m.getNumCompartments() == 1 )
    assert( @@m.getCompartment(0) == c )
  end

  def test_Model_createCompartmentType
    c = @@m.createCompartmentType()
    assert( c != nil )
    assert( @@m.getNumCompartmentTypes() == 1 )
    assert( @@m.getCompartmentType(0) == c )
  end

  def test_Model_createConstraint
    c = @@m.createConstraint()
    assert( c != nil )
    assert( @@m.getNumConstraints() == 1 )
    assert( @@m.getConstraint(0) == c )
  end

  def test_Model_createEvent
    e = @@m.createEvent()
    assert( e != nil )
    assert( @@m.getNumEvents() == 1 )
    assert( @@m.getEvent(0) == e )
  end

  def test_Model_createEventAssignment
    @@m.createEvent()
    @@m.createEvent()
    ea = @@m.createEventAssignment()
    assert( ea != nil )
    assert( @@m.getNumEvents() == 2 )
    e = @@m.getEvent(1)
    assert( e.getNumEventAssignments() == 1 )
    assert( e.getEventAssignment(0) == ea )
  end

  def test_Model_createEventAssignment_noEvent
    assert( @@m.getNumEvents() == 0 )
    assert( @@m.createEventAssignment() == nil )
  end

  def test_Model_createFunctionDefinition
    fd = @@m.createFunctionDefinition()
    assert( fd != nil )
    assert( @@m.getNumFunctionDefinitions() == 1 )
    assert( @@m.getFunctionDefinition(0) == fd )
  end

  def test_Model_createInitialAssignment
    c = @@m.createInitialAssignment()
    assert( c != nil )
    assert( @@m.getNumInitialAssignments() == 1 )
    assert( @@m.getInitialAssignment(0) == c )
  end

  def test_Model_createKineticLaw
    @@m.createReaction()
    @@m.createReaction()
    kl = @@m.createKineticLaw()
    assert( kl != nil )
    assert( @@m.getNumReactions() == 2 )
    r = @@m.getReaction(0)
    assert( r.getKineticLaw() == nil )
    r = @@m.getReaction(1)
    assert( r.getKineticLaw() == kl )
  end

  def test_Model_createKineticLawParameter
    @@m.createReaction()
    @@m.createReaction()
    @@m.createKineticLaw()
    p = @@m.createKineticLawParameter()
    assert( @@m.getNumReactions() == 2 )
    r = @@m.getReaction(0)
    assert( r.getKineticLaw() == nil )
    r = @@m.getReaction(1)
    assert( r.getKineticLaw() != nil )
    kl = r.getKineticLaw()
    assert( kl.getNumParameters() == 1 )
    assert( kl.getParameter(0) == p )
  end

  def test_Model_createKineticLawParameter_noKineticLaw
    r = @@m.createReaction()
    assert( r.getKineticLaw() == nil )
    assert( @@m.createKineticLawParameter() == nil )
  end

  def test_Model_createKineticLawParameter_noReaction
    assert( @@m.getNumReactions() == 0 )
    assert( @@m.createKineticLawParameter() == nil )
  end

  def test_Model_createKineticLaw_alreadyExists
    r = @@m.createReaction()
    kl = @@m.createKineticLaw()
    assert( r.getKineticLaw() == kl )
  end

  def test_Model_createKineticLaw_noReaction
    assert( @@m.getNumReactions() == 0 )
    assert( @@m.createKineticLaw() == nil )
  end

  def test_Model_createModifier
    @@m.createReaction()
    @@m.createReaction()
    msr = @@m.createModifier()
    assert( msr != nil )
    assert( @@m.getNumReactions() == 2 )
    r = @@m.getReaction(1)
    assert( r.getNumModifiers() == 1 )
    assert( r.getModifier(0) == msr )
  end

  def test_Model_createModifier_noReaction
    assert( @@m.getNumReactions() == 0 )
    assert( @@m.createModifier() == nil )
  end

  def test_Model_createParameter
    p = @@m.createParameter()
    assert( p != nil )
    assert( @@m.getNumParameters() == 1 )
    assert( @@m.getParameter(0) == p )
  end

  def test_Model_createProduct
    @@m.createReaction()
    @@m.createReaction()
    sr = @@m.createProduct()
    assert( sr != nil )
    assert( @@m.getNumReactions() == 2 )
    r = @@m.getReaction(1)
    assert( r.getNumProducts() == 1 )
    assert( r.getProduct(0) == sr )
  end

  def test_Model_createProduct_noReaction
    assert( @@m.getNumReactions() == 0 )
    assert( @@m.createProduct() == nil )
  end

  def test_Model_createRateRule
    rr = @@m.createRateRule()
    assert( rr != nil )
    assert( @@m.getNumRules() == 1 )
    assert( @@m.getRule(0) == rr )
  end

  def test_Model_createReactant
    @@m.createReaction()
    @@m.createReaction()
    sr = @@m.createReactant()
    assert( sr != nil )
    assert( @@m.getNumReactions() == 2 )
    r = @@m.getReaction(1)
    assert( r.getNumReactants() == 1 )
    assert( r.getReactant(0) == sr )
  end

  def test_Model_createReactant_noReaction
    assert( @@m.getNumReactions() == 0 )
    assert( @@m.createReactant() == nil )
  end

  def test_Model_createReaction
    r = @@m.createReaction()
    assert( r != nil )
    assert( @@m.getNumReactions() == 1 )
    assert( @@m.getReaction(0) == r )
  end

  def test_Model_createSpecies
    s = @@m.createSpecies()
    assert( s != nil )
    assert( @@m.getNumSpecies() == 1 )
    assert( @@m.getSpecies(0) == s )
  end

  def test_Model_createSpeciesType
    c = @@m.createSpeciesType()
    assert( c != nil )
    assert( @@m.getNumSpeciesTypes() == 1 )
    assert( @@m.getSpeciesType(0) == c )
  end

  def test_Model_createUnit
    @@m.createUnitDefinition()
    @@m.createUnitDefinition()
    u = @@m.createUnit()
    assert( u != nil )
    assert( @@m.getNumUnitDefinitions() == 2 )
    ud = @@m.getUnitDefinition(1)
    assert( ud.getNumUnits() == 1 )
    assert( ud.getUnit(0) == u )
  end

  def test_Model_createUnitDefinition
    ud = @@m.createUnitDefinition()
    assert( ud != nil )
    assert( @@m.getNumUnitDefinitions() == 1 )
    assert( @@m.getUnitDefinition(0) == ud )
  end

  def test_Model_createUnit_noUnitDefinition
    assert( @@m.getNumUnitDefinitions() == 0 )
    assert( @@m.createUnit() == nil )
  end

  def test_Model_createWithNS
    xmlns = LibSBML::XMLNamespaces.new()
    xmlns.add( "http://www.sbml.org", "testsbml")
    sbmlns = LibSBML::SBMLNamespaces.new(2,1)
    sbmlns.addNamespaces(xmlns)
    object = LibSBML::Model.new(sbmlns)
    assert( object.getTypeCode() == LibSBML::SBML_MODEL )
    assert( object.getMetaId() == "" )
    assert( object.getNotes() == nil )
    assert( object.getAnnotation() == nil )
    assert( object.getLevel() == 2 )
    assert( object.getVersion() == 1 )
    assert( object.getNamespaces() != nil )
    assert( object.getNamespaces().getLength() == 2 )
    object = nil
  end

  def test_Model_free_NULL
  end

  def test_Model_getCompartment
    c1 = LibSBML::Compartment.new(2,4)
    c2 = LibSBML::Compartment.new(2,4)
    c1.setId( "A")
    c2.setId( "B")
    @@m.addCompartment(c1)
    @@m.addCompartment(c2)
    assert( @@m.getNumCompartments() == 2 )
    c1 = @@m.getCompartment(0)
    c2 = @@m.getCompartment(1)
    assert ((  "A" == c1.getId() ))
    assert ((  "B" == c2.getId() ))
  end

  def test_Model_getCompartmentById
    c1 = LibSBML::Compartment.new(2,4)
    c2 = LibSBML::Compartment.new(2,4)
    c1.setId( "A" )
    c2.setId( "B" )
    @@m.addCompartment(c1)
    @@m.addCompartment(c2)
    assert( @@m.getNumCompartments() == 2 )
    assert( @@m.getCompartment( "A" ) != c1 )
    assert( @@m.getCompartment( "B" ) != c2 )
    assert( @@m.getCompartment( "C" ) == nil )
  end

  def test_Model_getEventById
    e1 = LibSBML::Event.new(2,4)
    e2 = LibSBML::Event.new(2,4)
    t = LibSBML::Trigger.new(2,4)
    e1.setTrigger(t)
    e2.setTrigger(t)
    e1.createEventAssignment()
    e2.createEventAssignment()
    e1.setId( "e1" )
    e2.setId( "e2" )
    @@m.addEvent(e1)
    @@m.addEvent(e2)
    assert( @@m.getNumEvents() == 2 )
    assert( @@m.getEvent( "e1" ) != e1 )
    assert( @@m.getEvent( "e2" ) != e2 )
    assert( @@m.getEvent( "e3" ) == nil )
  end

  def test_Model_getFunctionDefinitionById
    fd1 = LibSBML::FunctionDefinition.new(2,4)
    fd2 = LibSBML::FunctionDefinition.new(2,4)
    fd1.setId( "sin" )
    fd2.setId( "cos" )
    fd1.setMath(LibSBML::parseFormula("2"))
    fd2.setMath(LibSBML::parseFormula("2"))
    @@m.addFunctionDefinition(fd1)
    @@m.addFunctionDefinition(fd2)
    assert( @@m.getNumFunctionDefinitions() == 2 )
    assert( @@m.getFunctionDefinition( "sin" ) != fd1 )
    assert( @@m.getFunctionDefinition( "cos" ) != fd2 )
    assert( @@m.getFunctionDefinition( "tan" ) == nil )
  end

  def test_Model_getNumSpeciesWithBoundaryCondition
    s1 = LibSBML::Species.new(2,4)
    s2 = LibSBML::Species.new(2,4)
    s3 = LibSBML::Species.new(2,4)
    s1.setId( "s1")
    s2.setId( "s2")
    s3.setId( "s3")
    s1.setCompartment( "c1")
    s2.setCompartment( "c2")
    s3.setCompartment( "c3")
    s1.setBoundaryCondition(true)
    s2.setBoundaryCondition(false)
    s3.setBoundaryCondition(true)
    assert( @@m.getNumSpecies() == 0 )
    assert( @@m.getNumSpeciesWithBoundaryCondition() == 0 )
    @@m.addSpecies(s1)
    assert( @@m.getNumSpecies() == 1 )
    assert( @@m.getNumSpeciesWithBoundaryCondition() == 1 )
    @@m.addSpecies(s2)
    assert( @@m.getNumSpecies() == 2 )
    assert( @@m.getNumSpeciesWithBoundaryCondition() == 1 )
    @@m.addSpecies(s3)
    assert( @@m.getNumSpecies() == 3 )
    assert( @@m.getNumSpeciesWithBoundaryCondition() == 2 )
  end

  def test_Model_getParameter
    p1 = LibSBML::Parameter.new(2,4)
    p2 = LibSBML::Parameter.new(2,4)
    p1.setId( "Km1")
    p2.setId( "Km2")
    @@m.addParameter(p1)
    @@m.addParameter(p2)
    assert( @@m.getNumParameters() == 2 )
    p1 = @@m.getParameter(0)
    p2 = @@m.getParameter(1)
    assert ((  "Km1" == p1.getId() ))
    assert ((  "Km2" == p2.getId() ))
  end

  def test_Model_getParameterById
    p1 = LibSBML::Parameter.new(2,4)
    p2 = LibSBML::Parameter.new(2,4)
    p1.setId( "Km1" )
    p2.setId( "Km2" )
    @@m.addParameter(p1)
    @@m.addParameter(p2)
    assert( @@m.getNumParameters() == 2 )
    assert( @@m.getParameter( "Km1" ) != p1 )
    assert( @@m.getParameter( "Km2" ) != p2 )
    assert( @@m.getParameter( "Km3" ) == nil )
  end

  def test_Model_getReaction
    r1 = LibSBML::Reaction.new(2,4)
    r2 = LibSBML::Reaction.new(2,4)
    r1.setId( "reaction_1")
    r2.setId( "reaction_2")
    @@m.addReaction(r1)
    @@m.addReaction(r2)
    assert( @@m.getNumReactions() == 2 )
    r1 = @@m.getReaction(0)
    r2 = @@m.getReaction(1)
    assert ((  "reaction_1" == r1.getId() ))
    assert ((  "reaction_2" == r2.getId() ))
  end

  def test_Model_getReactionById
    r1 = LibSBML::Reaction.new(2,4)
    r2 = LibSBML::Reaction.new(2,4)
    r1.setId( "reaction_1" )
    r2.setId( "reaction_2" )
    @@m.addReaction(r1)
    @@m.addReaction(r2)
    assert( @@m.getNumReactions() == 2 )
    assert( @@m.getReaction( "reaction_1" ) != r1 )
    assert( @@m.getReaction( "reaction_2" ) != r2 )
    assert( @@m.getReaction( "reaction_3" ) == nil )
  end

  def test_Model_getRules
    ar = LibSBML::AlgebraicRule.new(2,4)
    scr = LibSBML::AssignmentRule.new(2,4)
    cvr = LibSBML::AssignmentRule.new(2,4)
    pr = LibSBML::AssignmentRule.new(2,4)
    scr.setVariable( "r2")
    cvr.setVariable( "r3")
    pr.setVariable( "r4")
    ar.setFormula( "x + 1"         )
    scr.setFormula( "k * t/(1 + k)" )
    cvr.setFormula( "0.10 * t"      )
    pr.setFormula( "k3/k2"         )
    @@m.addRule(ar)
    @@m.addRule(scr)
    @@m.addRule(cvr)
    @@m.addRule(pr)
    assert( @@m.getNumRules() == 4 )
    ar = @@m.getRule(0)
    scr = @@m.getRule(1)
    cvr = @@m.getRule(2)
    pr = @@m.getRule(3)
    assert ((  "x + 1"         == ar.getFormula() ))
    assert ((  "k * t/(1 + k)" == scr.getFormula() ))
    assert ((  "0.10 * t"      == cvr.getFormula() ))
    assert ((  "k3/k2"         == pr.getFormula() ))
  end

  def test_Model_getSpecies
    s1 = LibSBML::Species.new(2,4)
    s2 = LibSBML::Species.new(2,4)
    s1.setId( "Glucose"     )
    s2.setId( "Glucose_6_P" )
    s1.setCompartment( "c")
    s2.setCompartment( "c")
    @@m.addSpecies(s1)
    @@m.addSpecies(s2)
    assert( @@m.getNumSpecies() == 2 )
    s1 = @@m.getSpecies(0)
    s2 = @@m.getSpecies(1)
    assert ((  "Glucose"      == s1.getId() ))
    assert ((  "Glucose_6_P"  == s2.getId() ))
  end

  def test_Model_getSpeciesById
    s1 = LibSBML::Species.new(2,4)
    s2 = LibSBML::Species.new(2,4)
    s1.setId( "Glucose"     )
    s2.setId( "Glucose_6_P" )
    s1.setCompartment( "c")
    s2.setCompartment( "c")
    @@m.addSpecies(s1)
    @@m.addSpecies(s2)
    assert( @@m.getNumSpecies() == 2 )
    assert( @@m.getSpecies( "Glucose"    ) != s1 )
    assert( @@m.getSpecies( "Glucose_6_P") != s2 )
    assert( @@m.getSpecies( "Glucose2"   ) == nil )
  end

  def test_Model_getUnitDefinition
    ud1 = LibSBML::UnitDefinition.new(2,4)
    ud2 = LibSBML::UnitDefinition.new(2,4)
    ud1.setId( "mmls"   )
    ud2.setId( "volume" )
    ud1.createUnit()
    ud2.createUnit()
    @@m.addUnitDefinition(ud1)
    @@m.addUnitDefinition(ud2)
    assert( @@m.getNumUnitDefinitions() == 2 )
    ud1 = @@m.getUnitDefinition(0)
    ud2 = @@m.getUnitDefinition(1)
    assert ((  "mmls"    == ud1.getId() ))
    assert ((  "volume"  == ud2.getId() ))
  end

  def test_Model_getUnitDefinitionById
    ud1 = LibSBML::UnitDefinition.new(2,4)
    ud2 = LibSBML::UnitDefinition.new(2,4)
    ud1.setId( "mmls"   )
    ud2.setId( "volume" )
    ud1.createUnit()
    ud2.createUnit()
    @@m.addUnitDefinition(ud1)
    @@m.addUnitDefinition(ud2)
    assert( @@m.getNumUnitDefinitions() == 2 )
    assert( @@m.getUnitDefinition( "mmls"       ) != ud1 )
    assert( @@m.getUnitDefinition( "volume"     ) != ud2 )
    assert( @@m.getUnitDefinition( "rototillers") == nil )
  end

  def test_Model_removeCompartment
    o1 = @@m.createCompartment()
    o2 = @@m.createCompartment()
    o3 = @@m.createCompartment()
    o3.setId("test")
    assert( @@m.removeCompartment(0) == o1 )
    assert( @@m.getNumCompartments() == 2 )
    assert( @@m.removeCompartment(0) == o2 )
    assert( @@m.getNumCompartments() == 1 )
    assert( @@m.removeCompartment("test") == o3 )
    assert( @@m.getNumCompartments() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeCompartmentType
    o1 = @@m.createCompartmentType()
    o2 = @@m.createCompartmentType()
    o3 = @@m.createCompartmentType()
    o3.setId("test")
    assert( @@m.removeCompartmentType(0) == o1 )
    assert( @@m.getNumCompartmentTypes() == 2 )
    assert( @@m.removeCompartmentType(0) == o2 )
    assert( @@m.getNumCompartmentTypes() == 1 )
    assert( @@m.removeCompartmentType("test") == o3 )
    assert( @@m.getNumCompartmentTypes() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeConstraint
    o1 = @@m.createConstraint()
    o2 = @@m.createConstraint()
    o3 = @@m.createConstraint()
    assert( @@m.removeConstraint(0) == o1 )
    assert( @@m.getNumConstraints() == 2 )
    assert( @@m.removeConstraint(0) == o2 )
    assert( @@m.getNumConstraints() == 1 )
    assert( @@m.removeConstraint(0) == o3 )
    assert( @@m.getNumConstraints() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeEvent
    o1 = @@m.createEvent()
    o2 = @@m.createEvent()
    o3 = @@m.createEvent()
    o3.setId("test")
    assert( @@m.removeEvent(0) == o1 )
    assert( @@m.getNumEvents() == 2 )
    assert( @@m.removeEvent(0) == o2 )
    assert( @@m.getNumEvents() == 1 )
    assert( @@m.removeEvent("test") == o3 )
    assert( @@m.getNumEvents() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeFunctionDefinition
    o1 = @@m.createFunctionDefinition()
    o2 = @@m.createFunctionDefinition()
    o3 = @@m.createFunctionDefinition()
    o3.setId("test")
    assert( @@m.removeFunctionDefinition(0) == o1 )
    assert( @@m.getNumFunctionDefinitions() == 2 )
    assert( @@m.removeFunctionDefinition(0) == o2 )
    assert( @@m.getNumFunctionDefinitions() == 1 )
    assert( @@m.removeFunctionDefinition("test") == o3 )
    assert( @@m.getNumFunctionDefinitions() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeInitialAssignment
    o1 = @@m.createInitialAssignment()
    o2 = @@m.createInitialAssignment()
    o3 = @@m.createInitialAssignment()
    o3.setSymbol("test")
    assert( @@m.removeInitialAssignment(0) == o1 )
    assert( @@m.getNumInitialAssignments() == 2 )
    assert( @@m.removeInitialAssignment(0) == o2 )
    assert( @@m.getNumInitialAssignments() == 1 )
    assert( @@m.removeInitialAssignment("test") == o3 )
    assert( @@m.getNumInitialAssignments() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeParameter
    o1 = @@m.createParameter()
    o2 = @@m.createParameter()
    o3 = @@m.createParameter()
    o3.setId("test")
    assert( @@m.removeParameter(0) == o1 )
    assert( @@m.getNumParameters() == 2 )
    assert( @@m.removeParameter(0) == o2 )
    assert( @@m.getNumParameters() == 1 )
    assert( @@m.removeParameter("test") == o3 )
    assert( @@m.getNumParameters() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeReaction
    o1 = @@m.createReaction()
    o2 = @@m.createReaction()
    o3 = @@m.createReaction()
    o3.setId("test")
    assert( @@m.removeReaction(0) == o1 )
    assert( @@m.getNumReactions() == 2 )
    assert( @@m.removeReaction(0) == o2 )
    assert( @@m.getNumReactions() == 1 )
    assert( @@m.removeReaction("test") == o3 )
    assert( @@m.getNumReactions() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeRule
    o1 = @@m.createAssignmentRule()
    o2 = @@m.createAlgebraicRule()
    o3 = @@m.createRateRule()
    o3.setVariable("test")
    assert( @@m.removeRule(0) == o1 )
    assert( @@m.getNumRules() == 2 )
    assert( @@m.removeRule(0) == o2 )
    assert( @@m.getNumRules() == 1 )
    assert( @@m.removeRule("test") == o3 )
    assert( @@m.getNumRules() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeSpecies
    o1 = @@m.createSpecies()
    o2 = @@m.createSpecies()
    o3 = @@m.createSpecies()
    o3.setId("test")
    assert( @@m.removeSpecies(0) == o1 )
    assert( @@m.getNumSpecies() == 2 )
    assert( @@m.removeSpecies(0) == o2 )
    assert( @@m.getNumSpecies() == 1 )
    assert( @@m.removeSpecies("test") == o3 )
    assert( @@m.getNumSpecies() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeSpeciesType
    o1 = @@m.createSpeciesType()
    o2 = @@m.createSpeciesType()
    o3 = @@m.createSpeciesType()
    o3.setId("test")
    assert( @@m.removeSpeciesType(0) == o1 )
    assert( @@m.getNumSpeciesTypes() == 2 )
    assert( @@m.removeSpeciesType(0) == o2 )
    assert( @@m.getNumSpeciesTypes() == 1 )
    assert( @@m.removeSpeciesType("test") == o3 )
    assert( @@m.getNumSpeciesTypes() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_removeUnitDefinition
    o1 = @@m.createUnitDefinition()
    o2 = @@m.createUnitDefinition()
    o3 = @@m.createUnitDefinition()
    o3.setId("test")
    assert( @@m.removeUnitDefinition(0) == o1 )
    assert( @@m.getNumUnitDefinitions() == 2 )
    assert( @@m.removeUnitDefinition(0) == o2 )
    assert( @@m.getNumUnitDefinitions() == 1 )
    assert( @@m.removeUnitDefinition("test") == o3 )
    assert( @@m.getNumUnitDefinitions() == 0 )
    o1 = nil
    o2 = nil
    o3 = nil
  end

  def test_Model_setId
    id =  "Branch";
    @@m.setId(id)
    assert (( id == @@m.getId() ))
    assert_equal true, @@m.isSetId()
    if (@@m.getId() == id)
    end
    @@m.setId(@@m.getId())
    assert (( id == @@m.getId() ))
    @@m.setId("")
    assert_equal false, @@m.isSetId()
    if (@@m.getId() != nil)
    end
    @@m.setId(id)
    @@m.unsetId()
    assert_equal false, @@m.isSetId()
  end

  def test_Model_setName
    name =  "My_Branch_Model";
    @@m.setName(name)
    assert (( name == @@m.getName() ))
    assert_equal true, @@m.isSetName()
    if (@@m.getName() == name)
    end
    @@m.setName(@@m.getName())
    assert (( name == @@m.getName() ))
    @@m.setName("")
    assert_equal false, @@m.isSetName()
    if (@@m.getName() != nil)
    end
  end

  def test_Model_setgetModelHistory
    history = LibSBML::ModelHistory.new()
    mc = LibSBML::ModelCreator.new()
    date = LibSBML::Date.new(2005,12,30,12,15,45,1,2,0)
    mc.setFamilyName( "Keating")
    mc.setGivenName( "Sarah")
    mc.setEmail( "sbml-team@caltech.edu")
    mc.setOrganisation( "UH")
    history.addCreator(mc)
    history.setCreatedDate(date)
    history.setModifiedDate(date)
    assert( @@m.isSetModelHistory() == false )
    @@m.setModelHistory(history)
    assert( @@m.isSetModelHistory() == true )
    newMC = history.getCreator(0)
    assert( newMC != nil )
    assert ((  "Keating" == newMC.getFamilyName() ))
    assert ((  "Sarah" == newMC.getGivenName() ))
    assert ((  "sbml-team@caltech.edu" == newMC.getEmail() ))
    assert ((  "UH" == newMC.getOrganisation() ))
    @@m.unsetModelHistory()
    assert( @@m.isSetModelHistory() == false )
    history = nil
    mc = nil
  end

end