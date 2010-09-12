/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace libsbml {

using System;
using System.Runtime.InteropServices;

public class Rule : SBase {
	private HandleRef swigCPtr;
	
	internal Rule(IntPtr cPtr, bool cMemoryOwn) : base(libsbmlPINVOKE.RuleUpcast(cPtr), cMemoryOwn)
	{
		//super(libsbmlPINVOKE.RuleUpcast(cPtr), cMemoryOwn);
		swigCPtr = new HandleRef(this, cPtr);
	}
	
	internal static HandleRef getCPtr(Rule obj)
	{
		return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
	}
	
	internal static HandleRef getCPtrAndDisown (Rule obj)
	{
		HandleRef ptr = new HandleRef(null, IntPtr.Zero);
		
		if (obj != null)
		{
			ptr             = obj.swigCPtr;
			obj.swigCMemOwn = false;
		}
		
		return ptr;
	}

  ~Rule() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          libsbmlPINVOKE.delete_Rule(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public Rule(Rule orig) : this(libsbmlPINVOKE.new_Rule(Rule.getCPtr(orig)), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public new Rule clone() {
	return (Rule) libsbml.DowncastSBase(libsbmlPINVOKE.Rule_clone(swigCPtr), true);
}

  public string getFormula() {
    string ret = libsbmlPINVOKE.Rule_getFormula(swigCPtr);
    return ret;
  }

  public ASTNode getMath() {
    IntPtr cPtr = libsbmlPINVOKE.Rule_getMath(swigCPtr);
    ASTNode ret = (cPtr == IntPtr.Zero) ? null : new ASTNode(cPtr, false);
    return ret;
  }

  public string getVariable() {
    string ret = libsbmlPINVOKE.Rule_getVariable(swigCPtr);
    return ret;
  }

  public string getUnits() {
    string ret = libsbmlPINVOKE.Rule_getUnits(swigCPtr);
    return ret;
  }

  public bool isSetFormula() {
    bool ret = libsbmlPINVOKE.Rule_isSetFormula(swigCPtr);
    return ret;
  }

  public bool isSetMath() {
    bool ret = libsbmlPINVOKE.Rule_isSetMath(swigCPtr);
    return ret;
  }

  public bool isSetVariable() {
    bool ret = libsbmlPINVOKE.Rule_isSetVariable(swigCPtr);
    return ret;
  }

  public bool isSetUnits() {
    bool ret = libsbmlPINVOKE.Rule_isSetUnits(swigCPtr);
    return ret;
  }

  public int setFormula(string formula) {
    int ret = libsbmlPINVOKE.Rule_setFormula(swigCPtr, formula);
    return ret;
  }

  public int setMath(ASTNode math) {
    int ret = libsbmlPINVOKE.Rule_setMath(swigCPtr, ASTNode.getCPtr(math));
    return ret;
  }

  public int setVariable(string sid) {
    int ret = libsbmlPINVOKE.Rule_setVariable(swigCPtr, sid);
    return ret;
  }

  public int setUnits(string sname) {
    int ret = libsbmlPINVOKE.Rule_setUnits(swigCPtr, sname);
    return ret;
  }

  public int unsetUnits() {
    int ret = libsbmlPINVOKE.Rule_unsetUnits(swigCPtr);
    return ret;
  }

  public UnitDefinition getDerivedUnitDefinition() {
    IntPtr cPtr = libsbmlPINVOKE.Rule_getDerivedUnitDefinition__SWIG_0(swigCPtr);
    UnitDefinition ret = (cPtr == IntPtr.Zero) ? null : new UnitDefinition(cPtr, false);
    return ret;
  }

  public bool containsUndeclaredUnits() {
    bool ret = libsbmlPINVOKE.Rule_containsUndeclaredUnits__SWIG_0(swigCPtr);
    return ret;
  }

  public int getType() {
    int ret = libsbmlPINVOKE.Rule_getType(swigCPtr);
    return ret;
  }

  public bool isAlgebraic() {
    bool ret = libsbmlPINVOKE.Rule_isAlgebraic(swigCPtr);
    return ret;
  }

  public bool isAssignment() {
    bool ret = libsbmlPINVOKE.Rule_isAssignment(swigCPtr);
    return ret;
  }

  public bool isCompartmentVolume() {
    bool ret = libsbmlPINVOKE.Rule_isCompartmentVolume(swigCPtr);
    return ret;
  }

  public bool isParameter() {
    bool ret = libsbmlPINVOKE.Rule_isParameter(swigCPtr);
    return ret;
  }

  public bool isRate() {
    bool ret = libsbmlPINVOKE.Rule_isRate(swigCPtr);
    return ret;
  }

  public bool isScalar() {
    bool ret = libsbmlPINVOKE.Rule_isScalar(swigCPtr);
    return ret;
  }

  public bool isSpeciesConcentration() {
    bool ret = libsbmlPINVOKE.Rule_isSpeciesConcentration(swigCPtr);
    return ret;
  }

  public override int getTypeCode() {
    int ret = libsbmlPINVOKE.Rule_getTypeCode(swigCPtr);
    return ret;
  }

  public int getL1TypeCode() {
    int ret = libsbmlPINVOKE.Rule_getL1TypeCode(swigCPtr);
    return ret;
  }

  public override string getElementName() {
    string ret = libsbmlPINVOKE.Rule_getElementName(swigCPtr);
    return ret;
  }

  public int setL1TypeCode(int type) {
    int ret = libsbmlPINVOKE.Rule_setL1TypeCode(swigCPtr, type);
    return ret;
  }

  public override bool hasRequiredElements() {
    bool ret = libsbmlPINVOKE.Rule_hasRequiredElements(swigCPtr);
    return ret;
  }

  public override bool hasRequiredAttributes() {
    bool ret = libsbmlPINVOKE.Rule_hasRequiredAttributes(swigCPtr);
    return ret;
  }

  public string getId() {
    string ret = libsbmlPINVOKE.Rule_getId(swigCPtr);
    return ret;
  }

}

}