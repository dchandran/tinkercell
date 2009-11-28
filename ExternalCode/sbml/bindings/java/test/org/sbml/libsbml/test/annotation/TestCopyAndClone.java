/*
 *
 * @file    TestCopyAndClone.java
 * @brief   Test the copy and clone methods for annotation classes
 *
 * @author  Akiya Jouraku (Java conversion)
 * @author  Sarah Keating 
 *
 * $Id: TestCopyAndClone.java 10124 2009-08-28 12:04:51Z sarahkeating $
 * $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/java/test/org/sbml/libsbml/test/annotation/TestCopyAndClone.java $
 *
 * This test file was converted from src/sbml/test/TestCopyAndClone.cpp
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

public class TestCopyAndClone {

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

  public void test_CVTerm_assignmentOperator()
  {
    CVTerm CVTerm1 = new CVTerm(libsbml.BIOLOGICAL_QUALIFIER);
    CVTerm1.addResource("http://www.geneontology.org/#GO:0005892");
    assertTrue( CVTerm1.getQualifierType() == libsbml.BIOLOGICAL_QUALIFIER );
    assertTrue( CVTerm1.getResources().getLength() == 1 );
    assertTrue( CVTerm1.getResources().getValue(0).equals( "http://www.geneontology.org/#GO:0005892") );
    CVTerm CVTerm2 = new CVTerm();
    CVTerm2 = CVTerm1;
    assertTrue( CVTerm2.getQualifierType() == libsbml.BIOLOGICAL_QUALIFIER );
    assertTrue( CVTerm2.getResources().getLength() == 1 );
    assertTrue( CVTerm2.getResources().getValue(0).equals( "http://www.geneontology.org/#GO:0005892") );
    CVTerm2 = null;
    CVTerm1 = null;
  }

  public void test_CVTerm_clone()
  {
    CVTerm CVTerm1 = new CVTerm(libsbml.BIOLOGICAL_QUALIFIER);
    CVTerm1.addResource("http://www.geneontology.org/#GO:0005892");
    assertTrue( CVTerm1.getQualifierType() == libsbml.BIOLOGICAL_QUALIFIER );
    assertTrue( CVTerm1.getResources().getLength() == 1 );
    assertTrue( CVTerm1.getResources().getValue(0).equals( "http://www.geneontology.org/#GO:0005892") );
    CVTerm CVTerm2 = (CVTerm) CVTerm1.cloneObject();
    assertTrue( CVTerm2.getQualifierType() == libsbml.BIOLOGICAL_QUALIFIER );
    assertTrue( CVTerm2.getResources().getLength() == 1 );
    assertTrue( CVTerm2.getResources().getValue(0).equals( "http://www.geneontology.org/#GO:0005892") );
    CVTerm2 = null;
    CVTerm1 = null;
  }

  public void test_CVTerm_copyConstructor()
  {
    CVTerm CVTerm1 = new CVTerm(libsbml.BIOLOGICAL_QUALIFIER);
    CVTerm1.addResource("http://www.geneontology.org/#GO:0005892");
    assertTrue( CVTerm1.getQualifierType() == libsbml.BIOLOGICAL_QUALIFIER );
    assertTrue( CVTerm1.getResources().getLength() == 1 );
    assertTrue( CVTerm1.getResources().getValue(0).equals( "http://www.geneontology.org/#GO:0005892") );
    CVTerm CVTerm2 = new CVTerm(CVTerm1);
    assertTrue( CVTerm2.getQualifierType() == libsbml.BIOLOGICAL_QUALIFIER );
    assertTrue( CVTerm2.getResources().getLength() == 1 );
    assertTrue( CVTerm2.getResources().getValue(0).equals( "http://www.geneontology.org/#GO:0005892") );
    CVTerm2 = null;
    CVTerm1 = null;
  }

  public void test_Date_assignmentOperator()
  {
    Date date = new Date(2005,12,30,12,15,45,1,2,0);
    assertTrue( date.getMonth() == 12 );
    assertTrue( date.getSecond() == 45 );
    Date date2 = new Date();
    date2 = date;
    assertTrue( date2.getMonth() == 12 );
    assertTrue( date2.getSecond() == 45 );
    date2 = null;
    date = null;
  }

  public void test_Date_clone()
  {
    Date date = new Date(2005,12,30,12,15,45,1,2,0);
    assertTrue( date.getMonth() == 12 );
    assertTrue( date.getSecond() == 45 );
    Date date2 = (Date) date.cloneObject();
    assertTrue( date2.getMonth() == 12 );
    assertTrue( date2.getSecond() == 45 );
    date2 = null;
    date = null;
  }

  public void test_Date_copyConstructor()
  {
    Date date = new Date(2005,12,30,12,15,45,1,2,0);
    assertTrue( date.getMonth() == 12 );
    assertTrue( date.getSecond() == 45 );
    Date date2 = new Date(date);
    assertTrue( date2.getMonth() == 12 );
    assertTrue( date2.getSecond() == 45 );
    date2 = null;
    date = null;
  }

  public void test_ModelCreator_assignmentOperator()
  {
    ModelCreator mc = new ModelCreator();
    mc.setFamilyName("Keating");
    mc.setEmail("sbml-team@caltech.edu");
    assertTrue( mc.getFamilyName().equals( "Keating") );
    assertTrue( mc.getEmail().equals( "sbml-team@caltech.edu") );
    ModelCreator mc2 = new ModelCreator();
    mc2 = mc;
    assertTrue( mc2.getFamilyName().equals( "Keating") );
    assertTrue( mc2.getEmail().equals( "sbml-team@caltech.edu") );
    mc2 = null;
    mc = null;
  }

  public void test_ModelCreator_clone()
  {
    ModelCreator mc = new ModelCreator();
    mc.setFamilyName("Keating");
    mc.setEmail("sbml-team@caltech.edu");
    assertTrue( mc.getFamilyName().equals( "Keating") );
    assertTrue( mc.getEmail().equals( "sbml-team@caltech.edu") );
    ModelCreator mc2 = (ModelCreator) mc.cloneObject();
    assertTrue( mc2.getFamilyName().equals( "Keating") );
    assertTrue( mc2.getEmail().equals( "sbml-team@caltech.edu") );
    mc2 = null;
    mc = null;
  }

  public void test_ModelCreator_copyConstructor()
  {
    ModelCreator mc = new ModelCreator();
    mc.setFamilyName("Keating");
    mc.setEmail("sbml-team@caltech.edu");
    assertTrue( mc.getFamilyName().equals( "Keating") );
    assertTrue( mc.getEmail().equals( "sbml-team@caltech.edu") );
    ModelCreator mc2 = new ModelCreator(mc);
    assertTrue( mc2.getFamilyName().equals( "Keating") );
    assertTrue( mc2.getEmail().equals( "sbml-team@caltech.edu") );
    mc2 = null;
    mc = null;
  }

  public void test_ModelHistory_assignmentOperator()
  {
    ModelHistory mh = new ModelHistory();
    ModelCreator mc = new ModelCreator();
    mc.setGivenName("Sarah");
    mc.setFamilyName("Keating");
    mc.setEmail("sbml-team@caltech.edu");
    mh.addCreator(mc);
    mc = null;
    Date date = new Date(2005,12,30,12,15,45,1,2,0);
    mh.setCreatedDate(date);
    date = null;
    assertTrue( mh.getCreatedDate().getMonth() == 12 );
    assertTrue( mh.getCreatedDate().getSecond() == 45 );
    assertTrue( ((ModelCreator) mh.getCreator(0)).getFamilyName().equals( "Keating") );
    ModelHistory mh2 = new ModelHistory();
    mh2 = mh;
    assertTrue( mh2.getCreatedDate().getMonth() == 12 );
    assertTrue( mh2.getCreatedDate().getSecond() == 45 );
    assertTrue( ((ModelCreator) mh2.getCreator(0)).getFamilyName().equals( "Keating") );
    mh2 = null;
    mh = null;
  }

  public void test_ModelHistory_clone()
  {
    ModelHistory mh = new ModelHistory();
    ModelCreator mc = new ModelCreator();
    mc.setFamilyName("Keating");
    mc.setGivenName("Sarah");
    mc.setEmail("sbml-team@caltech.edu");
    mh.addCreator(mc);
    mc = null;
    Date date = new Date(2005,12,30,12,15,45,1,2,0);
    mh.setCreatedDate(date);
    date = null;
    assertTrue( mh.getCreatedDate().getMonth() == 12 );
    assertTrue( mh.getCreatedDate().getSecond() == 45 );
    assertTrue( ((ModelCreator) mh.getCreator(0)).getFamilyName().equals( "Keating") );
    ModelHistory mh2 = (ModelHistory) mh.cloneObject();
    assertTrue( mh2.getCreatedDate().getMonth() == 12 );
    assertTrue( mh2.getCreatedDate().getSecond() == 45 );
    assertTrue( ((ModelCreator) mh2.getCreator(0)).getFamilyName().equals( "Keating") );
    mh2 = null;
    mh = null;
  }

  public void test_ModelHistory_copyConstructor()
  {
    ModelHistory mh = new ModelHistory();
    ModelCreator mc = new ModelCreator();
    mc.setFamilyName("Keating");
    mc.setGivenName("Sarah");
    mc.setEmail("sbml-team@caltech.edu");
    mh.addCreator(mc);
    mc = null;
    Date date = new Date(2005,12,30,12,15,45,1,2,0);
    mh.setCreatedDate(date);
    date = null;
    assertTrue( mh.getCreatedDate().getMonth() == 12 );
    assertTrue( mh.getCreatedDate().getSecond() == 45 );
    assertTrue( ((ModelCreator) mh.getCreator(0)).getFamilyName().equals( "Keating") );
    ModelHistory mh2 = new ModelHistory(mh);
    assertTrue( mh2.getCreatedDate().getMonth() == 12 );
    assertTrue( mh2.getCreatedDate().getSecond() == 45 );
    assertTrue( ((ModelCreator) mh2.getCreator(0)).getFamilyName().equals( "Keating") );
    mh2 = null;
    mh = null;
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
