/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class tc_strings {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  public tc_strings(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  public static long getCPtr(tc_strings obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        tinkercellJNI.delete_tc_strings(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setLength(int value) {
    tinkercellJNI.tc_strings_length_set(swigCPtr, this, value);
  }

  public int getLength() {
    return tinkercellJNI.tc_strings_length_get(swigCPtr, this);
  }

  public void setStrings(SWIGTYPE_p_p_char value) {
    tinkercellJNI.tc_strings_strings_set(swigCPtr, this, SWIGTYPE_p_p_char.getCPtr(value));
  }

  public SWIGTYPE_p_p_char getStrings() {
    long cPtr = tinkercellJNI.tc_strings_strings_get(swigCPtr, this);
    return (cPtr == 0) ? null : new SWIGTYPE_p_p_char(cPtr, false);
  }

  public tc_strings() {
    this(tinkercellJNI.new_tc_strings(), true);
  }

}
