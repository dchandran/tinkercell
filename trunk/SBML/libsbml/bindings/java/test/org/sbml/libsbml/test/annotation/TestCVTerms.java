/*
 *
 * @file    TestCVTerms.java
 * @brief   CVTerms unit tests
 *
 * @author  Akiya Jouraku (Java conversion)
 * @author  Sarah Keating 
 *
 * $Id: TestCVTerms.java 10124 2009-08-28 12:04:51Z sarahkeating $
 * $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/java/test/org/sbml/libsbml/test/annotation/TestCVTerms.java $
 *
 * This test file was converted from src/sbml/test/TestCVTerms.c
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


package org.sbml.libsbml.test.annotation;

import org.sbml.libsbml.*;

import java.io.File;
import java.lang.AssertionError;

public class TestCVTerms {

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

  public void test_CVTerm_addResource()
  {
    CVTerm term = new  CVTerm(libsbml.MODEL_QUALIFIER);
    String resource =  "GO6666";;
    XMLAttributes xa;
    assertTrue( term != null );
    assertTrue( term.getQualifierType() == libsbml.MODEL_QUALIFIER );
    term.addResource(resource);
    xa = term.getResources();
    assertTrue( xa.getLength() == 1 );
    assertTrue(xa.getName(0).equals( "rdf:resource"));
    assertTrue(xa.getValue(0).equals( "GO6666"));
    term = null;
  }

  public void test_CVTerm_create()
  {
    CVTerm term = new  CVTerm(libsbml.MODEL_QUALIFIER);
    assertTrue( term != null );
    assertTrue( term.getQualifierType() == libsbml.MODEL_QUALIFIER );
    term = null;
  }

  public void test_CVTerm_createFromNode()
  {
    XMLAttributes xa;
    XMLTriple qual_triple = new  XMLTriple("is", "", "bqbiol");
    XMLTriple bag_triple = new  XMLTriple();
    XMLTriple li_triple = new  XMLTriple();
    XMLAttributes att = new  XMLAttributes();
    att.add( "", "This is my resource");
    XMLAttributes att1 = new  XMLAttributes();
    XMLToken li_token = new  XMLToken(li_triple,att);
    XMLToken bag_token = new  XMLToken(bag_triple,att1);
    XMLToken qual_token = new  XMLToken(qual_triple,att1);
    XMLNode li = new XMLNode(li_token);
    XMLNode bag = new XMLNode(bag_token);
    XMLNode node = new XMLNode(qual_token);
    bag.addChild(li);
    node.addChild(bag);
    CVTerm term = new  CVTerm(node);
    assertTrue( term != null );
    assertTrue( term.getQualifierType() == libsbml.BIOLOGICAL_QUALIFIER );
    assertTrue( term.getBiologicalQualifierType() == libsbml.BQB_IS );
    xa = term.getResources();
    assertTrue( xa.getLength() == 1 );
    assertTrue(xa.getName(0).equals( "rdf:resource"));
    assertTrue(xa.getValue(0).equals( "This is my resource"));
    qual_triple = null;
    bag_triple = null;
    li_triple = null;
    li_token = null;
    bag_token = null;
    qual_token = null;
    att = null;
    att1 = null;
    term = null;
    node = null;
    bag = null;
    li = null;
  }

  public void test_CVTerm_getResources()
  {
    CVTerm term = new  CVTerm(libsbml.MODEL_QUALIFIER);
    String resource =  "GO6666";;
    String resource1 =  "OtherURI";;
    long number;
    term.addResource(resource);
    term.addResource(resource1);
    number = term.getNumResources();
    assertTrue( number == 2 );
    assertTrue(term.getResourceURI(0).equals( "GO6666"));
    assertTrue(term.getResourceURI(1).equals( "OtherURI"));
    term = null;
  }

  public void test_CVTerm_set_get()
  {
    CVTerm term = new  CVTerm(libsbml.MODEL_QUALIFIER);
    assertTrue( term != null );
    assertTrue( term.getQualifierType() == libsbml.MODEL_QUALIFIER );
    term.setModelQualifierType(libsbml.BQM_IS);
    assertTrue( term != null );
    assertTrue( term.getQualifierType() == libsbml.MODEL_QUALIFIER );
    assertTrue( term.getModelQualifierType() == libsbml.BQM_IS );
    term.setQualifierType(libsbml.BIOLOGICAL_QUALIFIER);
    term.setBiologicalQualifierType(libsbml.BQB_IS);
    assertTrue( term.getQualifierType() == libsbml.BIOLOGICAL_QUALIFIER );
    assertTrue( term.getBiologicalQualifierType() == libsbml.BQB_IS );
    term = null;
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
