#ifndef IOVDATA_UTILFUNC_H
#define IOVDATA_UTILFUNC_H

#include <string>
#include <vector>
#include "IOVDataConstants.h"
namespace lariov{

  std::vector<lariov::ValueType_t> Str2ValueType(const std::vector<std::string>& field_type);
  
  std::vector<std::string> ValueType2Str(const std::vector<lariov::ValueType_t>& field_type);

  lariov::ValueType_t Str2ValueType(const std::string& field_type);
  
  std::string ValueType2Str(const lariov::ValueType_t& field_type);

  template <class T>
  T FromString( const std::string& value );
  template<> std::string    FromString< std::string    > (const std::string& value );
  template<> float          FromString< float          > (const std::string& value );
  template<> double         FromString< double         > (const std::string& value );
  template<> short          FromString< short          > (const std::string& value );
  template<> int            FromString< int            > (const std::string& value );
  template<> long           FromString< long           > (const std::string& value );
  template<> unsigned short FromString< unsigned short > (const std::string& value );
  template<> unsigned int   FromString< unsigned int   > (const std::string& value );
  template<> unsigned long  FromString< unsigned long  > (const std::string& value );

  template <class T>
  std::string ToString(const T& value);

  template<> std::string ToString<std::string>(const std::string& value);

}

#endif
