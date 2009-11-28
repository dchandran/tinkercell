#
# @file    TestEventAssignment_newSetters.rb
# @brief   EventAssignment unit tests for new set function API
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Sarah Keating 
#
# $Id$
# $HeadURL$
#
# This test file was converted from src/sbml/test/TestEventAssignment_newSetters.c
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

class TestEventAssignment_newSetters < Test::Unit::TestCase

  def setup
    @@e = LibSBML::EventAssignment.new(2,4)
    if (@@e == nil)
    end
  end

  def teardown
    @@e = nil
  end

  def test_EventAssignment_setMath1
    math = LibSBML::parseFormula("2 * k")
    i = @@e.setMath(math)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( @@e.getMath() != math )
    assert_equal true, @@e.isSetMath()
    i = @@e.setMath(nil)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( @@e.getMath() == nil )
    assert_equal false, @@e.isSetMath()
    math = nil
  end

  def test_EventAssignment_setMath2
    math = LibSBML::ASTNode.new(LibSBML::AST_TIMES)
    i = @@e.setMath(math)
    assert( i == LibSBML::LIBSBML_INVALID_OBJECT )
    assert_equal false, @@e.isSetMath()
    math = nil
  end

  def test_EventAssignment_setVariable1
    id =  "1e1";
    i = @@e.setVariable(id)
    assert( i == LibSBML::LIBSBML_INVALID_ATTRIBUTE_VALUE )
    assert_equal false, @@e.isSetVariable()
  end

  def test_EventAssignment_setVariable2
    id =  "e1";
    i = @@e.setVariable(id)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert (( id == @@e.getVariable() ))
    assert_equal true, @@e.isSetVariable()
    i = @@e.setVariable("")
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert_equal false, @@e.isSetVariable()
  end

end
