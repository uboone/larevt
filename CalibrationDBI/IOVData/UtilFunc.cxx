#ifndef IOVDATA_UTILFUNC_CXX
#define IOVDATA_UTILFUNC_CXX

#include "IOVDataError.h"
#include "UtilFunc.h"
namespace lariov {

  ValueType_t Str2ValueType(const std::string& ft)
  {
    ValueType_t vt(kUNKNOWN);
    if     ( ft == "smallint" || ft == "smallserial" || ft == "short") vt = kSHORT;
    else if( ft == "integer"  || ft == "serial"      || ft == "int"  ) vt = kINT;
    else if( ft == "bigint"   || ft == "bigserial"   || ft == "long" ) vt = kLONG;
    else if( ft == "real"     || ft == "float"          ) vt = kFLOAT;
    else if( ft == "double precision" || ft == "double" ) vt = kDOUBLE;
    else if( ft == "text"     || ft == "string"         ) vt = kSTRING;
    else {
      std::string msg("Unknown data type: ");
      msg += "\"" + ft + "\"";
      throw IOVDataError(msg.c_str());
    }
    return vt;
  }
  
  std::string ValueType2Str(const ValueType_t& vt)
  {
    std::string name("");
    switch(vt) {
    case kSHORT:
      name = "smallint"; break;
    case kINT:
      name = "integer"; break;
    case kLONG:
      name = "bigint"; break;
    case kFLOAT:
      name = "real"; break;
    case kDOUBLE:
      name = "double precision"; break;
    case kSTRING:
      name = "text"; break;
    default:
      std::string msg("Unknown data type: ");
      msg += "\"" + std::to_string(vt) + "\"";
      throw IOVDataError(msg.c_str());	
    }

    return name;
  }

  std::vector< ::lariov::ValueType_t> Str2ValueType(const std::vector<std::string>& field_type)
  {
    std::vector< ::lariov::ValueType_t> res(field_type.size(),kUNKNOWN);
    for(size_t i=0; i<field_type.size(); ++i)
      res[i] = Str2ValueType(field_type[i]);
    return res;
  }
  
  std::vector<std::string> ValueType2Str(const std::vector< ::lariov::ValueType_t>& field_type)
  {
    std::vector<std::string> res(field_type.size(),"");
    for(size_t i=0; i<field_type.size(); ++i)
      res[i] = ValueType2Str(field_type[i]);
    return res;
  }

  template<> std::string FromString( const std::string& value)
  { return value; }

  template<> float FromString( const std::string& value )
  { return std::stof(value); }

  template<> double FromString( const std::string& value )
  { return std::stod(value); }

  template<> unsigned short FromString( const std::string& value)
  { return std::stoul(value); }

  template<> unsigned int FromString( const std::string& value)
  { return std::stoul(value); }

  template<> unsigned long FromString( const std::string& value)
  { return std::stoul(value); }

  template<> short FromString( const std::string& value )
  { return std::stoi(value); }

  template<> int FromString( const std::string& value )
  { return std::stoi(value); }

  template<> long FromString( const std::string& value )
  { return std::stol(value); }

  template <class T> std::string ToString(const T& value)
  { return std::to_string(value); }

  template<> std::string ToString<std::string>(const std::string& value)
  { return std::string(value); }
  

}
#endif
