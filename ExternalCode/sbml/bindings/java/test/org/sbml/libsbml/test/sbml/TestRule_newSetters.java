/*
 *
 * @file    TestRule_newSetters.java
 * @brief   Rule unit tests for new set function API
 *
 * @author  Akiya Jouraku (Java conversion)
 * @author  Sarah Keating 
 *
 * $Id$
 * $HeadURL$
 *
 * This test file was converted from src/sbml/test/TestRule_newSetters.c
 * with the help of conversion sciprt (ctest_converter.pl).
 *
 *<!---------------------------------------------------------------------------
 * This file is part of libSBML.  Please visit http://sbml.org for more
 * information about SBML, and the latest version of libSBML.
 *
 * Copyright 2005-2009 California Institute of Technology.
 * Copyright 2002-2005 California Institute of Technology and
 *                     Japan Science and Technology Corporation.
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.  A copy of the license agreement is provided
 * in the file named "LICENSE.txt" included with this software distribution
 * and also available online as http://sbml.org/software/libsbml/license.html
 *--------------------------------------------------------------------------->*/


package org.sbml.libsbml.test.sbml;

import org.sbml.libsbml.*;

import java.io.File;
import java.lang.AssertionError;

public class TestRule_newSetters {

  static void assertTrue(boolean condition) throws AssertionError
  {
    if (condition == true)
    {
      return;
    }
    throw new AssertionError();
  }

  static void assertEquals(Object a, Object b) throws AssertionError
  {
    if ( (a == null) && (b == null) )
    {
      return;
    }
    else if ( (a == null) || (b == null) )
    {
      throw new AssertionError();
    }
    else if (a.equals(b))
    {
      return;
    }

    throw new AssertionError();
  }

  static void assertNotEquals(Object a, Object b) throws AssertionError
  {
    if ( (a == null) && (b == null) )
    {
      throw new AssertionError();
    }
    else if ( (a == null) || (b == null) )
    {
      return;
    }
    else if (a.equals(b))
    {
      throw new AssertionError();
    }
  }

  static void assertEquals(boolean a, boolean b) throws AssertionError
  {
    if ( a == b )
    {
      return;
    }
    throw new AssertionError();
  }

  static void assertNotEquals(boolean a, boolean b) throws AssertionError
  {
    if ( a != b )
    {
      return;
    }
    throw new AssertionError();
  }

  static void assertEquals(int a, int b) throws AssertionError
  {
    if ( a == b )
    {
      return;
    }
    throw new AssertionError();
  }

  static void assertNotEquals(int a, int b) throws AssertionError
  {
    if ( a != b )
    {
      return;
    }
    throw new AssertionError();
  }
  private Rule R;

  protected void setUp() throws Exception
  {
    R = new  AssignmentRule(2,4);
    if (R == null);
    {
    }
  }

  protected void tearDown() throws Exception
  {
    R = null;
  }

  public void test_Rule_setFormula1()
  {
    String formula =  "k1*X0";;
    long i = R.setFormula(formula);
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertTrue(R.getFormula().equals(formula));
    assertEquals( true, R.isSetFormula() );
  }

  public void test_Rule_setFormula2()
  {
    long i = R.setFormula("");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, R.isSetFormula() );
  }

  public void test_Rule_setFormula3()
  {
    String formula =  "k1 X0";;
    long i = R.setFormula(formula);
    assertTrue( i == libsbml.LIBSBML_INVALID_OBJECT );
    assertEquals( false, R.isSetFormula() );
  }

  public void test_Rule_setMath1()
  {
    ASTNode math = new  ASTNode(libsbml.AST_TIMES);
    ASTNode a = new  ASTNode();
    ASTNode b = new  ASTNode();
    a.setName( "a");
    b.setName( "b");
    math.addChild(a);
    math.addChild(b);
    String formula;
    ASTNode math1;
    long i = R.setMath(math);
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( true, R.isSetMath() );
    math1 = R.getMath();
    assertTrue( math1 != null );
    formula = libsbml.formulaToString(math1);
    assertTrue( formula != null );
    assertTrue(formula.equals( "a * b"));
    math = null;
  }

  public void test_Rule_setMath2()
  {
    ASTNode math = new  ASTNode(libsbml.AST_TIMES);
    ASTNode a = new  ASTNode();
    a.setName( "a");
    math.addChild(a);
    long i = R.setMath(math);
    assertTrue( i == libsbml.LIBSBML_INVALID_OBJECT );
    assertEquals( false, R.isSetMath() );
    math = null;
  }

  public void test_Rule_setMath3()
  {
    long i = R.setMath(null);
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, R.isSetMath() );
  }

  public void test_Rule_setUnits1()
  {
    long i = R.setUnits( "second");
    assertTrue( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE );
    assertEquals( false, R.isSetUnits() );
  }

  public void test_Rule_setUnits2()
  {
    Rule R1 = new  AssignmentRule(1,2);
    R1.setL1TypeCode(libsbml.SBML_PARAMETER_RULE);
    long i = R1.setUnits( "second");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( true, R1.isSetUnits() );
    i = R1.unsetUnits();
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, R1.isSetUnits() );
    R1 = null;
  }

  public void test_Rule_setUnits3()
  {
    Rule R1 = new  AssignmentRule(1,2);
    R1.setL1TypeCode(libsbml.SBML_PARAMETER_RULE);
    long i = R1.setUnits( "1second");
    assertTrue( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE );
    assertEquals( false, R1.isSetUnits() );
    i = R1.unsetUnits();
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, R1.isSetUnits() );
    R1 = null;
  }

  public void test_Rule_setUnits4()
  {
    Rule R1 = new  AssignmentRule(1,2);
    R1.setL1TypeCode(libsbml.SBML_PARAMETER_RULE);
    long i = R1.setUnits( "second");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( true, R1.isSetUnits() );
    i = R1.setUnits("");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, R1.isSetUnits() );
    R1 = null;
  }

  public void test_Rule_setVariable1()
  {
    long i = R.setVariable( "1mole");
    assertTrue( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE );
    assertEquals( false, R.isSetVariable() );
  }

  public void test_Rule_setVariable2()
  {
    long i = R.setVariable( "mole");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( true, R.isSetVariable() );
    i = R.setVariable( "");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, R.isSetVariable() );
  }

  public void test_Rule_setVariable3()
  {
    Rule R1 = new  AlgebraicRule(1,2);
    long i = R1.setVariable( "r");
    assertTrue( i == libsbml.LIBSBML_UNEXPECTED_ATTRIBUTE );
    assertEquals( false, R1.isSetVariable() );
    R1 = null;
  }

  /**
   * Loads the SWIG-generated libSBML Java module when this class is
   * loaded, or reports a sensible diagnostic message about why it failed.
   */
  static
  {
    String varname;
    String shlibname;

    if (System.getProperty("mrj.version") != null)
    {
      varname = "DYLD_LIBRARY_PATH";    // We're on a Mac.
      shlibname = "libsbmlj.jnilib and/or libsbml.dylib";
    }
    else
    {
      varname = "LD_LIBRARY_PATH";      // We're not on a Mac.
      shlibname = "libsbmlj.so and/or libsbml.so";
    }

    try
    {
      System.loadLibrary("sbmlj");
      // For extra safety, check that the jar file is in the classpath.
      Class.forName("org.sbml.libsbml.libsbml");
    }
    catch (SecurityException e)
    {
      e.printStackTrace();
      System.err.println("Could not load the libSBML library files due to a"+
                         " security exception.\n");
      System.exit(1);
    }
    catch (UnsatisfiedLinkError e)
    {
      e.printStackTrace();
      System.err.println("Error: could not link with the libSBML library files."+
                         " It is likely\nyour " + varname +
                         " environment variable does not include the directories\n"+
                         "containing the " + shlibname + " library files.\n");
      System.exit(1);
    }
    catch (ClassNotFoundException e)
    {
      e.printStackTrace();
      System.err.println("Error: unable to load the file libsbmlj.jar."+
                         " It is likely\nyour -classpath option and CLASSPATH" +
                         " environment variable\n"+
                         "do not include the path to libsbmlj.jar.\n");
      System.exit(1);
    }
  }
}
