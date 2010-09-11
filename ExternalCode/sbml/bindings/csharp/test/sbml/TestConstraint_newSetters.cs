/// 
///  @file    TestConstraint_newSetters.cs
///  @brief   Constraint unit tests for new set function API
///  @author  Frank Bergmann (Csharp conversion)
///  @author  Akiya Jouraku (Csharp conversion)
///  @author  Sarah Keating
/// 
///  $Id$
///  $HeadURL$
/// 
///  This test file was converted from src/sbml/test/TestConstraint_newSetters.c
///  with the help of conversion sciprt (ctest_converter.pl).
/// 
/// <!---------------------------------------------------------------------------
///  This file is part of libSBML.  Please visit http://sbml.org for more
///  information about SBML, and the latest version of libSBML.
/// 
///  Copyright 2005-2009 California Institute of Technology.
///  Copyright 2002-2005 California Institute of Technology and
///                      Japan Science and Technology Corporation.
///  
///  This library is free software; you can redistribute it and/or modify it
///  under the terms of the GNU Lesser General Public License as published by
///  the Free Software Foundation.  A copy of the license agreement is provided
///  in the file named "LICENSE.txt" included with this software distribution
///  and also available online as http://sbml.org/software/libsbml/license.html
/// --------------------------------------------------------------------------->*/


namespace LibSBMLCSTest {

  using libsbml;

  using  System.IO;

  public class TestConstraint_newSetters {
    public class AssertionError : System.Exception 
    {
      public AssertionError() : base()
      {
        
      }
    }


    static void assertTrue(bool condition)
    {
      if (condition == true)
      {
        return;
      }
      throw new AssertionError();
    }

    static void assertEquals(object a, object b)
    {
      if ( (a == null) && (b == null) )
      {
        return;
      }
      else if ( (a == null) || (b == null) )
      {
        throw new AssertionError();
      }
      else if (a.Equals(b))
      {
        return;
      }
  
      throw new AssertionError();
    }

    static void assertNotEquals(object a, object b)
    {
      if ( (a == null) && (b == null) )
      {
        throw new AssertionError();
      }
      else if ( (a == null) || (b == null) )
      {
        return;
      }
      else if (a.Equals(b))
      {
        throw new AssertionError();
      }
    }

    static void assertEquals(bool a, bool b)
    {
      if ( a == b )
      {
        return;
      }
      throw new AssertionError();
    }

    static void assertNotEquals(bool a, bool b)
    {
      if ( a != b )
      {
        return;
      }
      throw new AssertionError();
    }

    static void assertEquals(int a, int b)
    {
      if ( a == b )
      {
        return;
      }
      throw new AssertionError();
    }

    static void assertNotEquals(int a, int b)
    {
      if ( a != b )
      {
        return;
      }
      throw new AssertionError();
    }

    private Constraint C;

    public void setUp()
    {
      C = new  Constraint(2,4);
      if (C == null);
      {
      }
    }

    public void tearDown()
    {
      C = null;
    }

    public void test_Constraint_setMath1()
    {
      ASTNode math = libsbml.parseFormula("2 * k");
      long i = C.setMath(math);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( C.getMath() != math );
      assertEquals( true, C.isSetMath() );
      i = C.setMath(null);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( C.getMath() == null );
      assertEquals( false, C.isSetMath() );
      math = null;
    }

    public void test_Constraint_setMath2()
    {
      ASTNode math = new  ASTNode(libsbml.AST_TIMES);
      long i = C.setMath(math);
      assertTrue( i == libsbml.LIBSBML_INVALID_OBJECT );
      assertEquals( false, C.isSetMath() );
      math = null;
    }

    public void test_Constraint_setMessage1()
    {
      XMLNode node = new XMLNode();
      long i = C.setMessage(node);
      assertTrue( i == libsbml.LIBSBML_INVALID_OBJECT );
      assertTrue( C.isSetMessage() == false );
      i = C.unsetMessage();
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertEquals( false, C.isSetMessage() );
      if (C.getMessage() != null);
      {
      }
      node = null;
    }

    public void test_Constraint_setMessage2()
    {
      XMLNode text = XMLNode.convertStringToXMLNode(" Some text ",null);
      XMLTriple triple = new  XMLTriple("p", "http://www.w3.org/1999/xhtml", "");
      XMLAttributes att = new  XMLAttributes();
      XMLNamespaces xmlns = new  XMLNamespaces();
      xmlns.add( "http://www.w3.org/1999/xhtml", "");
      XMLNode p = new XMLNode(triple,att,xmlns);
      p.addChild(text);
      XMLTriple triple1 = new  XMLTriple("message", "", "");
      XMLAttributes att1 = new  XMLAttributes();
      XMLNode node = new XMLNode(triple1,att1);
      node.addChild(p);
      long i = C.setMessage(node);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( C.isSetMessage() == true );
      i = C.unsetMessage();
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertEquals( false, C.isSetMessage() );
      if (C.getMessage() != null);
      {
      }
      node = null;
    }

  }
}