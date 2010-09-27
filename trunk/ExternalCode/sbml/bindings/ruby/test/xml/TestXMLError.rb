#
# @file    TestXMLError.rb
# @brief   XMLError unit tests, C++ version
#
# @author  Akiya Jouraku (Ruby conversion)
# @author  Michael Hucka 
#
# $Id: TestXMLError.rb 10124 2009-08-28 12:04:51Z sarahkeating $
# $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/ruby/test/xml/TestXMLError.rb $
#
# This test file was converted from src/sbml/test/TestXMLError.cpp
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

class TestXMLError < Test::Unit::TestCase

  def test_XMLError_create
    error = LibSBML::XMLError.new()
    assert( error != 0 )
    error = nil
    error = LibSBML::XMLError.new(LibSBML::DuplicateXMLAttribute)
    assert( error.getErrorId() == LibSBML::DuplicateXMLAttribute )
    assert( error.getSeverity() == LibSBML::LIBSBML_SEV_ERROR )
    assert( error.getSeverityAsString() ==  "Error"  )
    assert( error.getCategory() == LibSBML::LIBSBML_CAT_XML )
    assert( error.getCategoryAsString() ==  "XML content" )
    assert( error.getMessage() ==  "Duplicate XML attribute."  )
    assert( error.getShortMessage() ==  "Duplicate attribute"  )
    error = nil
    error = LibSBML::XMLError.new(12345, "My message")
    assert( error.getErrorId() == 12345 )
    assert( error.getMessage() ==  "My message"  )
    assert( error.getSeverity() == LibSBML::LIBSBML_SEV_FATAL )
    assert( error.getSeverityAsString() ==  "Fatal"  )
    assert( error.getCategory() == LibSBML::LIBSBML_CAT_INTERNAL )
    assert( error.getCategoryAsString() ==  "Internal" )
    error = nil
    error = LibSBML::XMLError.new(12345, "My message",0,0,LibSBML::LIBSBML_SEV_INFO,LibSBML::LIBSBML_CAT_SYSTEM)
    assert( error.getErrorId() == 12345 )
    assert( error.getMessage() ==  "My message"  )
    assert( error.getSeverity() == LibSBML::LIBSBML_SEV_INFO )
    assert( error.getSeverityAsString() ==  "Informational"  )
    assert( error.getCategory() == LibSBML::LIBSBML_CAT_SYSTEM )
    assert( error.getCategoryAsString() ==  "Operating system" )
    assert_equal true, error.isInfo()
    assert_equal true, error.isSystem()
    error = nil
    error = LibSBML::XMLError.new(10000, "Another message",0,0,LibSBML::LIBSBML_SEV_FATAL,LibSBML::LIBSBML_CAT_XML)
    assert( error.getErrorId() == 10000 )
    assert( error.getMessage() ==  "Another message"  )
    assert( error.getSeverity() == LibSBML::LIBSBML_SEV_FATAL )
    assert( error.getSeverityAsString() ==  "Fatal"  )
    assert( error.getCategory() == LibSBML::LIBSBML_CAT_XML )
    assert( error.getCategoryAsString() ==  "XML content" )
    assert_equal true, error.isFatal()
    assert_equal true, error.isXML()
    error = nil
  end

  def test_XMLError_setters
    error = LibSBML::XMLError.new()
    assert( error != 0 )
    i = error.setLine(23)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( error.getLine() == 23 )
    i = error.setColumn(45)
    assert( i == LibSBML::LIBSBML_OPERATION_SUCCESS )
    assert( error.getColumn() == 45 )
    error = nil
  end

end