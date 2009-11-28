/// 
///  @file    TestXMLNode_newSetters.cs
///  @brief   XMLNode unit tests
///  @author  Frank Bergmann (Csharp conversion)
///  @author  Akiya Jouraku (Csharp conversion)
///  @author  Sarah Keating 
/// 
///  $Id$
///  $HeadURL$
/// 
///  This test file was converted from src/sbml/test/TestXMLNode_newSetters.c
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

  public class TestXMLNode_newSetters {
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


    public void test_XMLNode_addChild1()
    {
      XMLNode node = new XMLNode();
      XMLNode node2 = new XMLNode();
      long i = node.addChild(node2);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNumChildren() == 1 );
      node = null;
      node2 = null;
    }

    public void test_XMLNode_addChild2()
    {
      XMLTriple triple = new  XMLTriple("test","","");
      XMLAttributes attr = new  XMLAttributes();
      XMLNode node = new XMLNode(triple,attr);
      XMLNode node2 = new XMLNode();
      long i = node.addChild(node2);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNumChildren() == 1 );
      triple = null;
      attr = null;
      node = null;
      node2 = null;
    }

    public void test_XMLNode_addChild3()
    {
      XMLTriple triple = new  XMLTriple("test","","");
      XMLNode node = new XMLNode(triple);
      XMLNode node2 = new XMLNode();
      long i = node.addChild(node2);
      assertTrue( i == libsbml.LIBSBML_INVALID_XML_OPERATION );
      assertTrue( node.getNumChildren() == 0 );
      triple = null;
      node = null;
      node2 = null;
    }

    public void test_XMLNode_clearAttributes()
    {
      XMLTriple triple = new  XMLTriple("test","","");
      XMLAttributes attr = new  XMLAttributes();
      XMLNode node = new XMLNode(triple,attr);
      XMLTriple xt2 = new  XMLTriple("name3", "http://name3.org/", "p3");
      XMLTriple xt1 = new  XMLTriple("name5", "http://name5.org/", "p5");
      long i = node.addAttr( "name1", "val1");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 1 );
      i = node.addAttr( "name2", "val2", "http://name1.org/", "p1");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 2 );
      i = node.addAttr(xt2, "val2");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 3 );
      i = node.addAttr( "name4", "val4");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 4 );
      i = node.clearAttributes();
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 0 );
      xt1 = null;
      xt2 = null;
      triple = null;
      attr = null;
      node = null;
    }

    public void test_XMLNode_clearNamespaces()
    {
      XMLTriple triple = new  XMLTriple("test","","");
      XMLAttributes attr = new  XMLAttributes();
      XMLNode node = new XMLNode(triple,attr);
      long i = node.addNamespace( "http://test1.org/", "test1");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNamespaces().getLength() == 1 );
      i = node.addNamespace( "http://test2.org/", "test2");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNamespaces().getLength() == 2 );
      i = node.clearNamespaces();
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNamespaces().getLength() == 0 );
      triple = null;
      attr = null;
      node = null;
    }

    public void test_XMLNode_removeAttributes()
    {
      XMLTriple triple = new  XMLTriple("test","","");
      XMLAttributes attr = new  XMLAttributes();
      XMLNode node = new XMLNode(triple,attr);
      XMLTriple xt2 = new  XMLTriple("name3", "http://name3.org/", "p3");
      XMLTriple xt1 = new  XMLTriple("name5", "http://name5.org/", "p5");
      long i = node.addAttr( "name1", "val1");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 1 );
      i = node.addAttr( "name2", "val2", "http://name1.org/", "p1");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 2 );
      i = node.addAttr(xt2, "val2");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 3 );
      i = node.addAttr( "name4", "val4");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 4 );
      i = node.removeAttr(7);
      assertTrue( i == libsbml.LIBSBML_INDEX_EXCEEDS_SIZE );
      i = node.removeAttr( "name7");
      assertTrue( i == libsbml.LIBSBML_INDEX_EXCEEDS_SIZE );
      i = node.removeAttr( "name7", "namespaces7");
      assertTrue( i == libsbml.LIBSBML_INDEX_EXCEEDS_SIZE );
      i = node.removeAttr(xt1);
      assertTrue( i == libsbml.LIBSBML_INDEX_EXCEEDS_SIZE );
      assertTrue( node.getAttributes().getLength() == 4 );
      i = node.removeAttr(3);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 3 );
      i = node.removeAttr( "name1");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 2 );
      i = node.removeAttr( "name2", "http://name1.org/");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 1 );
      i = node.removeAttr(xt2);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getAttributes().getLength() == 0 );
      xt1 = null;
      xt2 = null;
      triple = null;
      attr = null;
      node = null;
    }

    public void test_XMLNode_removeChildren()
    {
      XMLNode node = new XMLNode();
      XMLNode node2 = new XMLNode();
      XMLNode node3 = new XMLNode();
      node.addChild(node2);
      node.addChild(node3);
      assertTrue( node.getNumChildren() == 2 );
      long i = node.removeChildren();
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNumChildren() == 0 );
      node = null;
      node2 = null;
      node3 = null;
    }

    public void test_XMLNode_removeNamespaces()
    {
      XMLTriple triple = new  XMLTriple("test","","");
      XMLAttributes attr = new  XMLAttributes();
      XMLNode node = new XMLNode(triple,attr);
      long i = node.addNamespace( "http://test1.org/", "test1");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNamespaces().getLength() == 1 );
      i = node.addNamespace( "http://test2.org/", "test2");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNamespaces().getLength() == 2 );
      i = node.removeNamespace(7);
      assertTrue( i == libsbml.LIBSBML_INDEX_EXCEEDS_SIZE );
      assertTrue( node.getNamespaces().getLength() == 2 );
      i = node.removeNamespace( "name7");
      assertTrue( i == libsbml.LIBSBML_INDEX_EXCEEDS_SIZE );
      assertTrue( node.getNamespaces().getLength() == 2 );
      i = node.removeNamespace(0);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNamespaces().getLength() == 1 );
      i = node.removeNamespace( "test2");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( node.getNamespaces().getLength() == 0 );
      triple = null;
      attr = null;
      node = null;
    }

  }
}
