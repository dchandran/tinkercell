/*
 *
 * @file    TestCompartmentType_newSetters.java
 * @brief   CompartmentType unit tests for new set function API
 *
 * @author  Akiya Jouraku (Java conversion)
 * @author  Sarah Keating
 *
 * $Id$
 * $HeadURL$
 *
 * This test file was converted from src/sbml/test/TestCompartmentType_newSetters.c
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

public class TestCompartmentType_newSetters {

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
  private CompartmentType C;

  protected void setUp() throws Exception
  {
    C = new  CompartmentType(2,2);
    if (C == null);
    {
    }
  }

  protected void tearDown() throws Exception
  {
    C = null;
  }

  public void test_CompartmentType_setId2()
  {
    long i = C.setId( "1cell");
    assertTrue( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE );
    assertEquals( false, C.isSetId() );
  }

  public void test_CompartmentType_setId3()
  {
    long i = C.setId( "cell");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( true, C.isSetId() );
    assertTrue(C.getId().equals( "cell" ));
  }

  public void test_CompartmentType_setId4()
  {
    long i = C.setId("");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, C.isSetId() );
  }

  public void test_CompartmentType_setName1()
  {
    long i = C.setName( "cell");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( true, C.isSetName() );
    i = C.unsetName();
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, C.isSetName() );
  }

  public void test_CompartmentType_setName2()
  {
    long i = C.setName( "1cell");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( true, C.isSetName() );
    i = C.unsetName();
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, C.isSetName() );
  }

  public void test_CompartmentType_setName3()
  {
    long i = C.setName("");
    assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
    assertEquals( false, C.isSetName() );
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