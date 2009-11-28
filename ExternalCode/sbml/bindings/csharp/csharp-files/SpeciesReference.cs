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

public class SpeciesReference : SimpleSpeciesReference {
	private HandleRef swigCPtr;
	
	internal SpeciesReference(IntPtr cPtr, bool cMemoryOwn) : base(libsbmlPINVOKE.SpeciesReferenceUpcast(cPtr), cMemoryOwn)
	{
		//super(libsbmlPINVOKE.SpeciesReferenceUpcast(cPtr), cMemoryOwn);
		swigCPtr = new HandleRef(this, cPtr);
	}
	
	internal static HandleRef getCPtr(SpeciesReference obj)
	{
		return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
	}
	
	internal static HandleRef getCPtrAndDisown (SpeciesReference obj)
	{
		HandleRef ptr = new HandleRef(null, IntPtr.Zero);
		
		if (obj != null)
		{
			ptr             = obj.swigCPtr;
			obj.swigCMemOwn = false;
		}
		
		return ptr;
	}

  ~SpeciesReference() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          libsbmlPINVOKE.delete_SpeciesReference(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public SpeciesReference(long level, long version) : this(libsbmlPINVOKE.new_SpeciesReference__SWIG_0(level, version), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public SpeciesReference(SBMLNamespaces sbmlns) : this(libsbmlPINVOKE.new_SpeciesReference__SWIG_1(SBMLNamespaces.getCPtr(sbmlns)), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public SpeciesReference(SpeciesReference orig) : this(libsbmlPINVOKE.new_SpeciesReference__SWIG_2(SpeciesReference.getCPtr(orig)), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public new SpeciesReference clone() {
    IntPtr cPtr = libsbmlPINVOKE.SpeciesReference_clone(swigCPtr);
    SpeciesReference ret = (cPtr == IntPtr.Zero) ? null : new SpeciesReference(cPtr, true);
    return ret;
  }

  public void initDefaults() {
    libsbmlPINVOKE.SpeciesReference_initDefaults(swigCPtr);
  }

  public double getStoichiometry() {
    double ret = libsbmlPINVOKE.SpeciesReference_getStoichiometry(swigCPtr);
    return ret;
  }

  public StoichiometryMath getStoichiometryMath() {
    IntPtr cPtr = libsbmlPINVOKE.SpeciesReference_getStoichiometryMath__SWIG_0(swigCPtr);
    StoichiometryMath ret = (cPtr == IntPtr.Zero) ? null : new StoichiometryMath(cPtr, false);
    return ret;
  }

  public int getDenominator() {
    int ret = libsbmlPINVOKE.SpeciesReference_getDenominator(swigCPtr);
    return ret;
  }

  public bool getConstant() {
    bool ret = libsbmlPINVOKE.SpeciesReference_getConstant(swigCPtr);
    return ret;
  }

  public bool isSetStoichiometryMath() {
    bool ret = libsbmlPINVOKE.SpeciesReference_isSetStoichiometryMath(swigCPtr);
    return ret;
  }

  public bool isSetConstant() {
    bool ret = libsbmlPINVOKE.SpeciesReference_isSetConstant(swigCPtr);
    return ret;
  }

  public bool isSetStoichiometry() {
    bool ret = libsbmlPINVOKE.SpeciesReference_isSetStoichiometry(swigCPtr);
    return ret;
  }

  public int setStoichiometry(double value) {
    int ret = libsbmlPINVOKE.SpeciesReference_setStoichiometry(swigCPtr, value);
    return ret;
  }

  public int setStoichiometryMath(StoichiometryMath math) {
    int ret = libsbmlPINVOKE.SpeciesReference_setStoichiometryMath(swigCPtr, StoichiometryMath.getCPtr(math));
    return ret;
  }

  public int setDenominator(int value) {
    int ret = libsbmlPINVOKE.SpeciesReference_setDenominator(swigCPtr, value);
    return ret;
  }

  public int unsetStoichiometryMath() {
    int ret = libsbmlPINVOKE.SpeciesReference_unsetStoichiometryMath(swigCPtr);
    return ret;
  }

  public StoichiometryMath createStoichiometryMath() {
    IntPtr cPtr = libsbmlPINVOKE.SpeciesReference_createStoichiometryMath(swigCPtr);
    StoichiometryMath ret = (cPtr == IntPtr.Zero) ? null : new StoichiometryMath(cPtr, false);
    return ret;
  }

  public override int setAnnotation(XMLNode annotation) {
    int ret = libsbmlPINVOKE.SpeciesReference_setAnnotation__SWIG_0(swigCPtr, XMLNode.getCPtr(annotation));
    return ret;
  }

  public override int setAnnotation(string annotation) {
    int ret = libsbmlPINVOKE.SpeciesReference_setAnnotation__SWIG_1(swigCPtr, annotation);
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override int appendAnnotation(XMLNode annotation) {
    int ret = libsbmlPINVOKE.SpeciesReference_appendAnnotation__SWIG_0(swigCPtr, XMLNode.getCPtr(annotation));
    return ret;
  }

  public override int appendAnnotation(string annotation) {
    int ret = libsbmlPINVOKE.SpeciesReference_appendAnnotation__SWIG_1(swigCPtr, annotation);
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public override int getTypeCode() {
    int ret = libsbmlPINVOKE.SpeciesReference_getTypeCode(swigCPtr);
    return ret;
  }

  public override string getElementName() {
    string ret = libsbmlPINVOKE.SpeciesReference_getElementName(swigCPtr);
    return ret;
  }

  public override bool hasRequiredAttributes() {
    bool ret = libsbmlPINVOKE.SpeciesReference_hasRequiredAttributes(swigCPtr);
    return ret;
  }

}

}
