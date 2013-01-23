/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace djondb {

using System;
using System.Runtime.InteropServices;

public class BSONArrayObj : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal BSONArrayObj(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(BSONArrayObj obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~BSONArrayObj() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          djonwrapperPINVOKE.delete_BSONArrayObj(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public BSONArrayObj() : this(djonwrapperPINVOKE.new_BSONArrayObj__SWIG_0(), true) {
  }

  public BSONArrayObj(BSONArrayObj orig) : this(djonwrapperPINVOKE.new_BSONArrayObj__SWIG_1(BSONArrayObj.getCPtr(orig)), true) {
    if (djonwrapperPINVOKE.SWIGPendingException.Pending) throw djonwrapperPINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual int length() {
    int ret = djonwrapperPINVOKE.BSONArrayObj_length(swigCPtr);
    return ret;
  }

  public void add(BSONObj obj) {
    djonwrapperPINVOKE.BSONArrayObj_add(swigCPtr, BSONObj.getCPtr(obj));
    if (djonwrapperPINVOKE.SWIGPendingException.Pending) throw djonwrapperPINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual BSONObj get(int index) {
    IntPtr cPtr = djonwrapperPINVOKE.BSONArrayObj_get(swigCPtr, index);
    BSONObj ret = (cPtr == IntPtr.Zero) ? null : new BSONObj(cPtr, false);
    return ret;
  }

  public virtual string toChar() {
    string ret = djonwrapperPINVOKE.BSONArrayObj_toChar(swigCPtr);
    return ret;
  }

  public virtual BSONArrayObj select(string select) {
    IntPtr cPtr = djonwrapperPINVOKE.BSONArrayObj_select(swigCPtr, select);
    BSONArrayObj ret = (cPtr == IntPtr.Zero) ? null : new BSONArrayObj(cPtr, false);
    return ret;
  }

  public virtual SWIGTYPE_p_std__vectorT_BSONObj_p_t__iterator begin() {
    SWIGTYPE_p_std__vectorT_BSONObj_p_t__iterator ret = new SWIGTYPE_p_std__vectorT_BSONObj_p_t__iterator(djonwrapperPINVOKE.BSONArrayObj_begin(swigCPtr), true);
    return ret;
  }

  public virtual SWIGTYPE_p_std__vectorT_BSONObj_p_t__iterator end() {
    SWIGTYPE_p_std__vectorT_BSONObj_p_t__iterator ret = new SWIGTYPE_p_std__vectorT_BSONObj_p_t__iterator(djonwrapperPINVOKE.BSONArrayObj_end(swigCPtr), true);
    return ret;
  }

}

}
