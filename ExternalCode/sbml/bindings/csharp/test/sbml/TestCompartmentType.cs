/// 
///  @file    TestCompartmentType.cs
///  @brief   CompartmentTypeType unit tests
///  @author  Frank Bergmann (Csharp conversion)
///  @author  Akiya Jouraku (Csharp conversion)
///  @author  Sarah Keating 
/// 
///  $Id: TestCompartmentType.cs 10124 2009-08-28 12:04:51Z sarahkeating $
///  $HeadURL: https://sbml.svn.sourceforge.net/svnroot/sbml/trunk/libsbml/src/bindings/csharp/test/sbml/TestCompartmentType.cs $
/// 
///  This test file was converted from src/sbml/test/TestCompartmentType.c
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

  public class TestCompartmentType {
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

    private CompartmentType CT;

    public void setUp()
    {
      CT = new  CompartmentType(2,4);
      if (CT == null);
      {
      }
    }

    public void tearDown()
    {
      CT = null;
    }

    public void test_CompartmentType_create()
    {
      assertTrue( CT.getTypeCode() == libsbml.SBML_COMPARTMENT_TYPE );
      assertTrue( CT.getMetaId() == "" );
      assertTrue( CT.getNotes() == null );
      assertTrue( CT.getAnnotation() == null );
      assertTrue( CT.getId() == "" );
      assertTrue( CT.getName() == "" );
      assertEquals( false, CT.isSetId() );
      assertEquals( false, CT.isSetName() );
    }

    public void test_CompartmentType_createWithNS()
    {
      XMLNamespaces xmlns = new  XMLNamespaces();
      xmlns.add( "http://www.sbml.org", "testsbml");
      SBMLNamespaces sbmlns = new  SBMLNamespaces(2,2);
      sbmlns.addNamespaces(xmlns);
      CompartmentType object1 = new  CompartmentType(sbmlns);
      assertTrue( object1.getTypeCode() == libsbml.SBML_COMPARTMENT_TYPE );
      assertTrue( object1.getMetaId() == "" );
      assertTrue( object1.getNotes() == null );
      assertTrue( object1.getAnnotation() == null );
      assertTrue( object1.getLevel() == 2 );
      assertTrue( object1.getVersion() == 2 );
      assertTrue( object1.getNamespaces() != null );
      assertTrue( object1.getNamespaces().getLength() == 2 );
      object1 = null;
    }

    public void test_CompartmentType_free_NULL()
    {
    }

    public void test_CompartmentType_setId()
    {
      string id =  "mitochondria";;
      CT.setId(id);
      assertTrue(( id == CT.getId() ));
      assertEquals( true, CT.isSetId() );
      if (CT.getId() == id);
      {
      }
      CT.setId(CT.getId());
      assertTrue(( id == CT.getId() ));
      CT.setId("");
      assertEquals( false, CT.isSetId() );
      if (CT.getId() != null);
      {
      }
    }

    public void test_CompartmentType_setName()
    {
      string name =  "My_Favorite_Factory";;
      CT.setName(name);
      assertTrue(( name == CT.getName() ));
      assertEquals( true, CT.isSetName() );
      if (CT.getName() == name);
      {
      }
      CT.setName(CT.getName());
      assertTrue(( name == CT.getName() ));
      CT.setName("");
      assertEquals( false, CT.isSetName() );
      if (CT.getName() != null);
      {
      }
    }

    public void test_CompartmentType_unsetName()
    {
      CT.setName( "name");
      assertTrue((  "name"      == CT.getName() ));
      assertEquals( true, CT.isSetName() );
      CT.unsetName();
      assertEquals( false, CT.isSetName() );
    }

  }
}