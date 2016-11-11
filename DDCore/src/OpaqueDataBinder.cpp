//==========================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include "DD4hep/OpaqueDataBinder.h"
#include "DD4hep/objects/OpaqueData_inl.h"
#include "DD4hep/objects/ConditionsInterna.h"
#include "DD4hep/Conditions.h"

// C/C++ include files
#include <set>
#include <map>
#include <list>
#include <vector>

using namespace std;

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Helper class to bind string values to C++ data objects (primitive or complex)
  template <typename T, typename Q> bool ValueBinder::bind(T& object, const string& val, const Q*) const
  {  object.bind<Q>(val);            return true;  }

  /// Helper class to bind string values to a STL vector of data objects (primitive or complex)
  template <typename T, typename Q> bool VectorBinder::bind(T& object, const string& val, const Q*) const
  {  object.bind<vector<Q> >(val);   return true;  }

  /// Helper class to bind string values to a STL list of data objects (primitive or complex)
  template <typename T, typename Q> bool ListBinder::bind(T& object, const string& val, const Q*) const
  {  object.bind<list<Q> >(val);     return true;  }

  /// Helper class to bind string values to a STL set of data objects (primitive or complex)
  template <typename T, typename Q> bool SetBinder::bind(T& object, const string& val, const Q*) const
  {  object.bind<set<Q> >(val);      return true;  }

  /// Helper class to bind STL map objects
  template <typename T, typename Q> bool MapBinder::bind(T& object, const Q*) const
  {  object.bind((const Q*)0);       return true;  }

  /// Binding function for scalar items. See the implementation function for the concrete instantiations
  template <typename BINDER, typename T> 
  bool OpaqueDataBinder::bind(const BINDER& b, T& object, const string& typ, const string& val)  {
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    if ( typ.substr(0,4) == "char" )
      return b.bind(object,val,_char());
    else if ( typ.substr(0,13) == "unsigned char" )
      return b.bind(object,val,_uchar());
    else if ( typ.substr(0,5) == "short" )
      return b.bind(object,val,_short());
    else if ( typ.substr(0,14) == "unsigned short" )
      return b.bind(object,val,_ushort());
    else if ( typ.substr(0,12) == "unsigned int" )
      return b.bind(object,val,_uint());
    else if ( typ.substr(0,13) == "unsigned long" )
      return b.bind(object,val,_ulong());
#else
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    if ( typ.substr(0,4) == "char" )
      return b.bind(object,val,_int());
    else if ( typ.substr(0,5) == "short" )
      return b.bind(object,val,_int());
#endif
    else if ( typ.substr(0,3) == "int" )
      return b.bind(object,val,_int());
    else if ( typ.substr(0,4) == "long" ) 
      return b.bind(object,val,_long());
    else if ( typ.substr(0,5) == "float" )
      return b.bind(object,val,_float());
    else if ( typ.substr(0,6) == "double" )
      return b.bind(object,val,_double());
    else if ( typ.substr(0,6) == "string" )
      return b.bind(object,val,_string());
    else if ( typ == "std::string" )
      return b.bind(object,val,_string());
    else if ( typ == "Histo1D" )
      return b.bind(object,val,_string());
    else if ( typ == "Histo2D" )
      return b.bind(object,val,_string());
    else
      printout(INFO,"OpaqueDataBinder","++ Unknown conditions parameter type:%s val:%s",typ.c_str(),val.c_str());
    return b.bind(object,val,_string());
  }
  
  /// Binding function for sequences (unmapped STL containers)
  template <typename T> 
  bool OpaqueDataBinder::bind_sequence(T& object, const string& typ, const string& val)
  {
    size_t idx = typ.find('[');
    size_t idq = typ.find(']');
    string value_type = typ.substr(idx+1,idq-idx-1);
    if ( typ.substr(0,6) == "vector" )
      return bind(VectorBinder(), object, value_type, val);
    else if ( typ.substr(0,6) == "list" )
      return bind(ListBinder(), object, value_type, val);
    else if ( typ.substr(0,6) == "set" )
      return bind(SetBinder(), object, value_type, val);
    else if ( idx == string::npos && idq == string::npos )
      return bind(ValueBinder(), object, value_type, val);
    return false;
  }
  
  template<typename BINDER, typename OBJECT, typename KEY, typename VAL>
  static void insert_map_item(const BINDER&,
                              OBJECT& object,
                              const KEY& k,
                              const string& val,
                              const VAL*)
  {
    typedef map<KEY,VAL> map_t;
    map_t& m = object.get<map_t>();
    VAL v;
    if ( !BasicGrammar::instance<VAL>().fromString(&v, val) )  {
      except("OpaqueDataBinder","++ Failed to convert conditions map entry.");
    }
    m.insert(make_pair(k,v));
  }

  template<typename BINDER, typename OBJECT, typename KEY> 
  static void insert_map_key(const BINDER& b,
                             OBJECT& object,
                             const string& key_val,
                             const string& val_type,
                             const string& val,
                             const KEY*)
  {
    KEY key;
    BasicGrammar::instance<KEY>().fromString(&key, key_val);
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    if ( val_type.substr(0,4) == "char" )
      insert_map_item(b, object, key, val, (int*)0);
    else if ( val_type.substr(0,5) == "short" )
      insert_map_item(b, object, key, val, (int*)0);
    else if ( val_type.substr(0,3) == "int" )
      insert_map_item(b, object, key, val, (int*)0);
    else if ( val_type.substr(0,4) == "long" )
      insert_map_item(b, object, key, val, (long*)0);
    else if ( val_type.substr(0,5) == "float" )
      insert_map_item(b, object, key, val, (float*)0);
    else if ( val_type.substr(0,6) == "double" )
      insert_map_item(b, object, key, val, (double*)0);
    else if ( val_type.substr(0,6) == "string" )
      insert_map_item(b, object, key, val, (string*)0);
    else if ( val_type == "std::string" )
      insert_map_item(b, object, key, val, (string*)0);
    else {
      printout(INFO,"Param","++ Unknown conditions parameter type:%s data:%s",
               val_type.c_str(),val.c_str());
      insert_map_item(b, object, key, val, (string*)0);
    }
  }

  template<typename BINDER, typename OBJECT, typename KEY, typename VAL>
  static void insert_map_pair(const BINDER&,
                              OBJECT& object,
                              const string& data,
                              const KEY*,
                              const VAL*)
  {
    typedef map<KEY,VAL> map_t;
    pair<KEY,VAL> entry;
    map_t& m = object.get<map_t>();
    if ( !BasicGrammar::instance<pair<KEY,VAL> >().fromString(&entry,data) )  {
      except("OpaqueDataBinder","++ Failed to convert conditions map entry.");
    }
    m.insert(entry);
  }

  template<typename BINDER, typename OBJECT, typename KEY> 
  static void insert_map_data(const BINDER& b,
                              OBJECT& object,
                              const string& val_type,
                              const string& pair_data,
                              const KEY*)
  {
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    if ( val_type.substr(0,4) == "char" )
      insert_map_pair(b, object, pair_data, (KEY*)0, (int*)0);
    else if ( val_type.substr(0,5) == "short" )
      insert_map_pair(b, object, pair_data, (KEY*)0, (int*)0);
    else if ( val_type.substr(0,3) == "int" )
      insert_map_pair(b, object, pair_data, (KEY*)0, (int*)0);
    else if ( val_type.substr(0,4) == "long" )
      insert_map_pair(b, object, pair_data, (KEY*)0, (long*)0);
    else if ( val_type.substr(0,5) == "float" )
      insert_map_pair(b, object, pair_data, (KEY*)0, (float*)0);
    else if ( val_type.substr(0,6) == "double" )
      insert_map_pair(b, object, pair_data, (KEY*)0, (double*)0);
    else if ( val_type.substr(0,6) == "string" )
      insert_map_pair(b, object, pair_data, (KEY*)0, (string*)0);
    else if ( val_type == "std::string" )
      insert_map_pair(b, object, pair_data, (KEY*)0, (string*)0);
    else {
      printout(INFO,"Param","++ Unknown conditions parameter type:%s data:%s",
               val_type.c_str(),pair_data.c_str());
      insert_map_pair(b, object, pair_data, (KEY*)0, (string*)0);
    }
  }

  template<typename BINDER, typename OBJECT, typename KEY> 
  static void bind_mapping(const BINDER& b, const string& val_type, OBJECT& object, const KEY*)   {
    if ( val_type.substr(0,3) == "int" )
      b.bind(object, (map<KEY,int>*)0);
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    else if ( val_type.substr(0,12) == "unsigned int" )
      b.bind(object, (map<KEY,unsigned int>*)0);
    else if ( val_type.substr(0,4) == "char" )
      b.bind(object, (map<KEY,char>*)0);
    else if ( val_type.substr(0,13) == "unsigned char" )
      b.bind(object, (map<KEY,unsigned char>*)0);
    else if ( val_type.substr(0,5) == "short" )
      b.bind(object, (map<KEY,short>*)0);
    else if ( val_type.substr(0,14) == "unsigned short" )
      b.bind(object, (map<KEY,unsigned short>*)0);
    else if ( val_type.substr(0,13) == "unsigned long" )
      b.bind(object, (map<KEY,unsigned long>*)0);
#else
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    else if ( val_type.substr(0,4) == "char" )
      b.bind(object, (map<KEY,int>*)0);
    else if ( val_type.substr(0,5) == "short" )
      b.bind(object, (map<KEY,int>*)0);
#endif
    else if ( val_type.substr(0,4) == "long" )
      b.bind(object, (map<KEY,long>*)0);
    else if ( val_type.substr(0,5) == "float" )
      b.bind(object, (map<KEY,float>*)0);
    else if ( val_type.substr(0,6) == "double" )
      b.bind(object, (map<KEY,double>*)0);
    else if ( val_type.substr(0,6) == "string" )
      b.bind(object, (map<KEY,string>*)0);
    else if ( val_type == "std::string" )
      b.bind(object, (map<KEY,string>*)0);
    else {
      b.bind(object, (map<KEY,string>*)0);
    }
  }
  
  /// Binding function for STL maps
  template <typename BINDER, typename OBJECT> 
  bool OpaqueDataBinder::bind_map(const BINDER& b, OBJECT& object,
                                  const string& key_type, const string& val_type)   {
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    if ( key_type.substr(0,3) == "int" )
      bind_mapping(b, val_type, object, _int());
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    else if ( key_type.substr(0,4) == "char" )
      bind_mapping(b, val_type, object, _int());
    else if ( key_type.substr(0,5) == "short" )
      bind_mapping(b, val_type, object, _int());
    else if ( key_type.substr(0,4) == "long" )
      bind_mapping(b, val_type, object, _long());
    else if ( key_type.substr(0,5) == "float" )
      bind_mapping(b, val_type, object, _float());
    else if ( key_type.substr(0,6) == "double" )
      bind_mapping(b, val_type, object, _double());
#endif
    else if ( key_type.substr(0,6) == "string" )
      bind_mapping(b, val_type, object, _string());
    else if ( key_type == "std::string" )
      bind_mapping(b, val_type, object, _string());
    else {
      printout(INFO,"OpaqueDataBinder","++ Unknown MAP-conditions key-type:%s",key_type.c_str());
      bind_mapping(b, val_type, object, _string());
    }
    return true;
  }

  /// Filling function for STL maps.
  template <typename BINDER, typename OBJECT>
  bool OpaqueDataBinder::insert_map(const BINDER& b, OBJECT& object,
                                    const string& key_type, const string& key,
                                    const string& val_type, const string& val)
  {
    if ( key_type.substr(0,3) == "int" )
      insert_map_key(b, object, key, val_type, val, _int());
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    else if ( key_type.substr(0,4) == "char" )
      insert_map_key(b, object, key, val_type, val, _int());
    else if ( key_type.substr(0,5) == "short" )
      insert_map_key(b, object, key, val_type, val, _int());
    else if ( key_type.substr(0,4) == "long" )
      insert_map_key(b, object, key, val_type, val, _long());
    else if ( key_type.substr(0,5) == "float" )
      insert_map_key(b, object, key, val_type, val, _float());
    else if ( key_type.substr(0,6) == "double" )
      insert_map_key(b, object, key, val_type, val, _double());
#endif
    else if ( key_type.substr(0,6) == "string" )
      insert_map_key(b, object, key, val_type, val, _string());
    else if ( key_type == "std::string" )
      insert_map_key(b, object, key, val_type, val, _string());
    else {
      printout(INFO,"OpaqueDataBinder","++ Unknown MAP-conditions key-type:%s",key_type.c_str());
      insert_map_key(b, object, key, val_type, val, _string());
    }
    return true;
  }

  /// Filling function for STL maps.
  template <typename BINDER, typename OBJECT> 
  bool OpaqueDataBinder::insert_map(const BINDER& b, OBJECT& object,
                                    const std::string& key_type, const std::string& val_type,
                                    const std::string& pair_data)
  {
    if ( key_type.substr(0,3) == "int" )
      insert_map_data(b, object, val_type, pair_data, _int());
#if defined(DD4HEP_HAVE_ALL_PARSERS)
    // Short and char is not part of the standard dictionaries. Fall back to 'int'.
    else if ( key_type.substr(0,4) == "char" )
      insert_map_data(b, object, val_type, pair_data, _int());
    else if ( key_type.substr(0,5) == "short" )
      insert_map_data(b, object, val_type, pair_data, _int());
    else if ( key_type.substr(0,4) == "long" )
      insert_map_data(b, object, val_type, pair_data, _long());
    else if ( key_type.substr(0,5) == "float" )
      insert_map_data(b, object, val_type, pair_data, _float());
    else if ( key_type.substr(0,6) == "double" )
      insert_map_data(b, object, val_type, pair_data, _double());
#endif
    else if ( key_type.substr(0,6) == "string" )
      insert_map_data(b, object, val_type, pair_data, _string());
    else if ( key_type == "std::string" )
      insert_map_data(b, object, val_type, pair_data, _string());
    else {
      printout(INFO,"OpaqueDataBinder","++ Unknown MAP-conditions key-type:%s",key_type.c_str());
      insert_map_data(b, object, val_type, pair_data, _string());
    }
    return true;
  }

  /// Instantiate the data binder for OpaqueData
  template bool OpaqueDataBinder::bind_sequence<OpaqueDataBlock>(OpaqueDataBlock& object,const string& typ,const string& val);
  template bool OpaqueDataBinder::bind_map<MapBinder,OpaqueDataBlock>(  const MapBinder& b, OpaqueDataBlock& object,
                                                                        const string& typ,const string& val);
  template bool OpaqueDataBinder::insert_map<MapBinder,OpaqueDataBlock>(const MapBinder& b, OpaqueDataBlock& object,
                                                                        const string& key_type, const string& key,
                                                                        const string& val_type, const string& val);
  template bool OpaqueDataBinder::insert_map<MapBinder,OpaqueDataBlock>(const MapBinder& b, OpaqueDataBlock& object,
                                                                        const string& key_type, const string& val_type,
                                                                        const string& pair_data);

  /// Conditions binding function for STL maps
  template <> bool OpaqueDataBinder::bind_map(const MapBinder& b, Conditions::Condition& object,
                                              const string& key_type, const string& val_type)
  {    return bind_map(b, object->data, key_type, val_type);  }

  /// Conditions: Filling function for STL maps.
  template <> bool OpaqueDataBinder::insert_map(const MapBinder& b, Conditions::Condition& object,
                                                const string& key_type, const string& key,
                                                const string& val_type, const string& val)
  {    return insert_map(b, object->data, key_type, key, val_type, val);    }

  /// Conditions: Filling function for STL maps.
  template <> bool OpaqueDataBinder::insert_map(const MapBinder& b, Conditions::Condition& object,
                                                const string& key_type, const string& val_type, const string& pair_data)
  {    return insert_map(b, object->data, key_type, val_type, pair_data);   }

  /// Instantiation for Conditions:
  template bool
  OpaqueDataBinder::bind_sequence<Conditions::Condition>(Conditions::Condition& object,const string& typ,const string& val);
  
}
