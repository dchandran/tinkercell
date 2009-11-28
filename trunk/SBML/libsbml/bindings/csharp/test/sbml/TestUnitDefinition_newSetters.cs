/// 
///  @file    TestUnitDefinition_newSetters.cs
///  @brief   SBML UnitDefinition unit tests for new API
///  @author  Frank Bergmann (Csharp conversion)
///  @author  Akiya Jouraku (Csharp conversion)
///  @author  sarah Keating 
/// 
///  $Id$
///  $HeadURL$
/// 
///  This test file was converted from src/sbml/test/TestUnitDefinition_newSetters.c
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

  public class TestUnitDefinition_newSetters {
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

    private UnitDefinition UD;

    public void setUp()
    {
      UD = new  UnitDefinition(2,4);
      if (UD == null);
      {
      }
    }

    public void tearDown()
    {
      UD = null;
    }

    public void test_UnitDefinition_addUnit1()
    {
      UnitDefinition m = new  UnitDefinition(2,2);
      Unit p = new  Unit(2,2);
      long i = m.addUnit(p);
      assertTrue( i == libsbml.LIBSBML_INVALID_OBJECT );
      p.setKind(libsbml.UNIT_KIND_MOLE);
      i = m.addUnit(p);
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue( m.getNumUnits() == 1 );
      p = null;
      m = null;
    }

    public void test_UnitDefinition_addUnit2()
    {
      UnitDefinition m = new  UnitDefinition(2,2);
      Unit p = new  Unit(2,1);
      p.setKind(libsbml.UNIT_KIND_MOLE);
      long i = m.addUnit(p);
      assertTrue( i == libsbml.LIBSBML_VERSION_MISMATCH );
      assertTrue( m.getNumUnits() == 0 );
      p = null;
      m = null;
    }

    public void test_UnitDefinition_addUnit3()
    {
      UnitDefinition m = new  UnitDefinition(2,2);
      Unit p = new  Unit(1,2);
      p.setKind(libsbml.UNIT_KIND_MOLE);
      long i = m.addUnit(p);
      assertTrue( i == libsbml.LIBSBML_LEVEL_MISMATCH );
      assertTrue( m.getNumUnits() == 0 );
      p = null;
      m = null;
    }

    public void test_UnitDefinition_addUnit4()
    {
      UnitDefinition m = new  UnitDefinition(2,2);
      Unit p = null;
      long i = m.addUnit(p);
      assertTrue( i == libsbml.LIBSBML_OPERATION_FAILED );
      assertTrue( m.getNumUnits() == 0 );
      m = null;
    }

    public void test_UnitDefinition_createUnit()
    {
      UnitDefinition m = new  UnitDefinition(2,2);
      Unit p = m.createUnit();
      assertTrue( m.getNumUnits() == 1 );
      assertTrue( (p).getLevel() == 2 );
      assertTrue( (p).getVersion() == 2 );
      m = null;
    }

    public void test_UnitDefinition_setId1()
    {
      long i = UD.setId( "mmls");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue((  "mmls" == UD.getId() ));
      assertEquals( true, UD.isSetId() );
      i = UD.setId("");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertEquals( false, UD.isSetId() );
      i = UD.setId( "123");
      assertTrue( i == libsbml.LIBSBML_INVALID_ATTRIBUTE_VALUE );
      assertEquals( false, UD.isSetId() );
    }

    public void test_UnitDefinition_setName1()
    {
      long i = UD.setName( "mmls");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue((  "mmls" == UD.getName() ));
      assertEquals( true, UD.isSetName() );
      i = UD.setName("");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertEquals( false, UD.isSetName() );
      i = UD.setName( "123");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertEquals( true, UD.isSetName() );
    }

    public void test_UnitDefinition_setName2()
    {
      long i = UD.setName( "mmls");
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertTrue((  "mmls" == UD.getName() ));
      assertEquals( true, UD.isSetName() );
      i = UD.unsetName();
      assertTrue( i == libsbml.LIBSBML_OPERATION_SUCCESS );
      assertEquals( false, UD.isSetName() );
    }

  }
}
