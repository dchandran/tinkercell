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

public class XMLError : IDisposable {
	private HandleRef swigCPtr;
	protected bool swigCMemOwn;
	
	internal XMLError(IntPtr cPtr, bool cMemoryOwn)
	{
		swigCMemOwn = cMemoryOwn;
		swigCPtr    = new HandleRef(this, cPtr);
	}
	
	internal static HandleRef getCPtr(XMLError obj)
	{
		return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
	}
	
	internal static HandleRef getCPtrAndDisown (XMLError obj)
	{
		HandleRef ptr = new HandleRef(null, IntPtr.Zero);
		
		if (obj != null)
		{
			ptr             = obj.swigCPtr;
			obj.swigCMemOwn = false;
		}
		
		return ptr;
	}

  ~XMLError() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          libsbmlPINVOKE.delete_XMLError(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public static bool operator==(XMLError lhs, XMLError rhs)
  {
    if((Object)lhs == (Object)rhs)
    {
      return true;
    }

    if( ((Object)lhs == null) || ((Object)rhs == null) )
    {
      return false;
    }

    return (getCPtr(lhs).Handle.ToString() == getCPtr(rhs).Handle.ToString());
  }

  public static bool operator!=(XMLError lhs, XMLError rhs)
  {
    return !(lhs == rhs);
  }

  public override bool Equals(Object sb)
  {
    if ( ! (sb is XMLError) )
    {
      return false;
    }

    return this == (XMLError)sb;
  }

  public override int GetHashCode()
  {
    return swigCPtr.Handle.ToInt32();
  }

  public XMLError(int errorId, string details, long line, long column, long severity, long category) : this(libsbmlPINVOKE.new_XMLError__SWIG_0(errorId, details, line, column, severity, category), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public XMLError(int errorId, string details, long line, long column, long severity) : this(libsbmlPINVOKE.new_XMLError__SWIG_1(errorId, details, line, column, severity), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public XMLError(int errorId, string details, long line, long column) : this(libsbmlPINVOKE.new_XMLError__SWIG_2(errorId, details, line, column), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public XMLError(int errorId, string details, long line) : this(libsbmlPINVOKE.new_XMLError__SWIG_3(errorId, details, line), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public XMLError(int errorId, string details) : this(libsbmlPINVOKE.new_XMLError__SWIG_4(errorId, details), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public XMLError(int errorId) : this(libsbmlPINVOKE.new_XMLError__SWIG_5(errorId), true) {
  }

  public XMLError() : this(libsbmlPINVOKE.new_XMLError__SWIG_6(), true) {
  }

  public XMLError(XMLError orig) : this(libsbmlPINVOKE.new_XMLError__SWIG_7(XMLError.getCPtr(orig)), true) {
    if (libsbmlPINVOKE.SWIGPendingException.Pending) throw libsbmlPINVOKE.SWIGPendingException.Retrieve();
  }

  public long getErrorId() { return (long)libsbmlPINVOKE.XMLError_getErrorId(swigCPtr); }

  public string getMessage() {
    string ret = libsbmlPINVOKE.XMLError_getMessage(swigCPtr);
    return ret;
  }

  public string getShortMessage() {
    string ret = libsbmlPINVOKE.XMLError_getShortMessage(swigCPtr);
    return ret;
  }

  public long getLine() { return (long)libsbmlPINVOKE.XMLError_getLine(swigCPtr); }

  public long getColumn() { return (long)libsbmlPINVOKE.XMLError_getColumn(swigCPtr); }

  public long getSeverity() { return (long)libsbmlPINVOKE.XMLError_getSeverity(swigCPtr); }

  public string getSeverityAsString() {
    string ret = libsbmlPINVOKE.XMLError_getSeverityAsString(swigCPtr);
    return ret;
  }

  public long getCategory() { return (long)libsbmlPINVOKE.XMLError_getCategory(swigCPtr); }

  public string getCategoryAsString() {
    string ret = libsbmlPINVOKE.XMLError_getCategoryAsString(swigCPtr);
    return ret;
  }

  public bool isInfo() {
    bool ret = libsbmlPINVOKE.XMLError_isInfo(swigCPtr);
    return ret;
  }

  public bool isWarning() {
    bool ret = libsbmlPINVOKE.XMLError_isWarning(swigCPtr);
    return ret;
  }

  public bool isError() {
    bool ret = libsbmlPINVOKE.XMLError_isError(swigCPtr);
    return ret;
  }

  public bool isFatal() {
    bool ret = libsbmlPINVOKE.XMLError_isFatal(swigCPtr);
    return ret;
  }

  public bool isInternal() {
    bool ret = libsbmlPINVOKE.XMLError_isInternal(swigCPtr);
    return ret;
  }

  public bool isSystem() {
    bool ret = libsbmlPINVOKE.XMLError_isSystem(swigCPtr);
    return ret;
  }

  public bool isXML() {
    bool ret = libsbmlPINVOKE.XMLError_isXML(swigCPtr);
    return ret;
  }

  public int setLine(long line) {
    int ret = libsbmlPINVOKE.XMLError_setLine(swigCPtr, line);
    return ret;
  }

  public int setColumn(long column) {
    int ret = libsbmlPINVOKE.XMLError_setColumn(swigCPtr, column);
    return ret;
  }

  public static string getStandardMessage(int code) {
    string ret = libsbmlPINVOKE.XMLError_getStandardMessage(code);
    return ret;
  }

}

}