/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.4
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package djondb;

public class djonwrapperJNI {
  public final static native long new_BSONObjVectorPtr__SWIG_0();
  public final static native long new_BSONObjVectorPtr__SWIG_1(long jarg1);
  public final static native long BSONObjVectorPtr_size(long jarg1, BSONObjVectorPtr jarg1_);
  public final static native long BSONObjVectorPtr_capacity(long jarg1, BSONObjVectorPtr jarg1_);
  public final static native void BSONObjVectorPtr_reserve(long jarg1, BSONObjVectorPtr jarg1_, long jarg2);
  public final static native boolean BSONObjVectorPtr_isEmpty(long jarg1, BSONObjVectorPtr jarg1_);
  public final static native void BSONObjVectorPtr_clear(long jarg1, BSONObjVectorPtr jarg1_);
  public final static native void BSONObjVectorPtr_add(long jarg1, BSONObjVectorPtr jarg1_, long jarg2, BSONObj jarg2_);
  public final static native long BSONObjVectorPtr_get(long jarg1, BSONObjVectorPtr jarg1_, int jarg2);
  public final static native void BSONObjVectorPtr_set(long jarg1, BSONObjVectorPtr jarg1_, int jarg2, long jarg3, BSONObj jarg3_);
  public final static native void delete_BSONObjVectorPtr(long jarg1);
  public final static native long new_StringVector__SWIG_0();
  public final static native long new_StringVector__SWIG_1(long jarg1);
  public final static native long StringVector_size(long jarg1, StringVector jarg1_);
  public final static native long StringVector_capacity(long jarg1, StringVector jarg1_);
  public final static native void StringVector_reserve(long jarg1, StringVector jarg1_, long jarg2);
  public final static native boolean StringVector_isEmpty(long jarg1, StringVector jarg1_);
  public final static native void StringVector_clear(long jarg1, StringVector jarg1_);
  public final static native void StringVector_add(long jarg1, StringVector jarg1_, String jarg2);
  public final static native String StringVector_get(long jarg1, StringVector jarg1_, int jarg2);
  public final static native void StringVector_set(long jarg1, StringVector jarg1_, int jarg2, String jarg3);
  public final static native void delete_StringVector(long jarg1);
  public final static native long new_BSONArrayObj__SWIG_0();
  public final static native void delete_BSONArrayObj(long jarg1);
  public final static native long new_BSONArrayObj__SWIG_1(long jarg1, BSONArrayObj jarg1_);
  public final static native int BSONArrayObj_length(long jarg1, BSONArrayObj jarg1_);
  public final static native void BSONArrayObj_add(long jarg1, BSONArrayObj jarg1_, long jarg2, BSONObj jarg2_);
  public final static native long BSONArrayObj_get(long jarg1, BSONArrayObj jarg1_, int jarg2);
  public final static native String BSONArrayObj_toChar(long jarg1, BSONArrayObj jarg1_);
  public final static native long BSONArrayObj_select(long jarg1, BSONArrayObj jarg1_, String jarg2);
  public final static native long BSONArrayObj_begin(long jarg1, BSONArrayObj jarg1_);
  public final static native long BSONArrayObj_end(long jarg1, BSONArrayObj jarg1_);
  public final static native long new_BSONObj__SWIG_0();
  public final static native long new_BSONObj__SWIG_1(long jarg1, BSONObj jarg1_);
  public final static native void delete_BSONObj(long jarg1);
  public final static native void BSONObj_add__SWIG_0(long jarg1, BSONObj jarg1_, String jarg2, int jarg3);
  public final static native void BSONObj_add__SWIG_1(long jarg1, BSONObj jarg1_, String jarg2, double jarg3);
  public final static native void BSONObj_add__SWIG_2(long jarg1, BSONObj jarg1_, String jarg2, long jarg3);
  public final static native void BSONObj_add__SWIG_3(long jarg1, BSONObj jarg1_, String jarg2, String jarg3);
  public final static native void BSONObj_add__SWIG_4(long jarg1, BSONObj jarg1_, String jarg2, long jarg3, BSONObj jarg3_);
  public final static native void BSONObj_add__SWIG_5(long jarg1, BSONObj jarg1_, String jarg2, long jarg3, BSONArrayObj jarg3_);
  public final static native boolean BSONObj_has(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native int BSONObj_getInt(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native double BSONObj_getDouble(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native long BSONObj_getLong(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native String BSONObj_getString(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native long BSONObj_getBSON(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native long BSONObj_getBSONArray(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native long BSONObj_get(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native long BSONObj_getContent__SWIG_0(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native long BSONObj_getContent__SWIG_1(long jarg1, BSONObj jarg1_, String jarg2, long jarg3);
  public final static native long BSONObj_getXpath(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native long BSONObj_select(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native long BSONObj_type(long jarg1, BSONObj jarg1_, String jarg2);
  public final static native String BSONObj_toChar(long jarg1, BSONObj jarg1_);
  public final static native long BSONObj_begin(long jarg1, BSONObj jarg1_);
  public final static native long BSONObj_end(long jarg1, BSONObj jarg1_);
  public final static native int BSONObj_length(long jarg1, BSONObj jarg1_);
  public final static native long new_BSONParser();
  public final static native void delete_BSONParser(long jarg1);
  public final static native long BSONParser_parse(String jarg1);
  public final static native long BSONParser_parseArray(String jarg1);
  public final static native int parseFilterOperator(String jarg1);
  public final static native long new_ParseException__SWIG_0(int jarg1, String jarg2);
  public final static native long new_ParseException__SWIG_1(long jarg1, ParseException jarg1_);
  public final static native String ParseException_what(long jarg1, ParseException jarg1_);
  public final static native int ParseException_errorCode(long jarg1, ParseException jarg1_);
  public final static native void delete_ParseException(long jarg1);
  public final static native long bson_splitSelect(String jarg1);
  public final static native String bson_subselect(String jarg1, String jarg2);
  public final static native int SERVER_PORT_get();
  public final static native long new_DjondbConnection__SWIG_0(String jarg1);
  public final static native long new_DjondbConnection__SWIG_1(String jarg1, int jarg2);
  public final static native long new_DjondbConnection__SWIG_2(long jarg1, DjondbConnection jarg1_);
  public final static native void delete_DjondbConnection(long jarg1);
  public final static native boolean DjondbConnection_open(long jarg1, DjondbConnection jarg1_);
  public final static native void DjondbConnection_close(long jarg1, DjondbConnection jarg1_);
  public final static native void DjondbConnection_internalClose(long jarg1, DjondbConnection jarg1_);
  public final static native boolean DjondbConnection_isOpen(long jarg1, DjondbConnection jarg1_);
  public final static native boolean DjondbConnection_shutdown(long jarg1, DjondbConnection jarg1_);
  public final static native boolean DjondbConnection_insert__SWIG_0(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native boolean DjondbConnection_insert__SWIG_1(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, long jarg4, BSONObj jarg4_);
  public final static native long DjondbConnection_findByKey__SWIG_0(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, String jarg4, String jarg5);
  public final static native long DjondbConnection_findByKey__SWIG_1(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native long DjondbConnection_find__SWIG_0(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, String jarg4, String jarg5);
  public final static native long DjondbConnection_find__SWIG_1(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native boolean DjondbConnection_update__SWIG_0(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, String jarg4);
  public final static native boolean DjondbConnection_update__SWIG_1(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, long jarg4, BSONObj jarg4_);
  public final static native boolean DjondbConnection_remove(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3, String jarg4, String jarg5);
  public final static native boolean DjondbConnection_dropNamespace(long jarg1, DjondbConnection jarg1_, String jarg2, String jarg3);
  public final static native long DjondbConnection_dbs(long jarg1, DjondbConnection jarg1_);
  public final static native long DjondbConnection_namespaces(long jarg1, DjondbConnection jarg1_, String jarg2);
  public final static native String DjondbConnection_host(long jarg1, DjondbConnection jarg1_);
  public final static native void ConnectionReference__connection_set(long jarg1, ConnectionReference jarg1_, long jarg2, DjondbConnection jarg2_);
  public final static native long ConnectionReference__connection_get(long jarg1, ConnectionReference jarg1_);
  public final static native void ConnectionReference__references_set(long jarg1, ConnectionReference jarg1_, int jarg2);
  public final static native int ConnectionReference__references_get(long jarg1, ConnectionReference jarg1_);
  public final static native long new_ConnectionReference();
  public final static native void delete_ConnectionReference(long jarg1);
  public final static native long new_DjondbConnectionManager();
  public final static native void delete_DjondbConnectionManager(long jarg1);
  public final static native long DjondbConnectionManager_getConnection__SWIG_0(String jarg1);
  public final static native long DjondbConnectionManager_getConnection__SWIG_1(String jarg1, int jarg2);
  public final static native void DjondbConnectionManager_releaseConnection(long jarg1, DjondbConnection jarg1_);
}
