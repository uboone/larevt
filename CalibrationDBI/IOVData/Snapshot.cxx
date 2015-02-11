#ifndef IOVDATA_SNAPSHOT_CXX
#define IOVDATA_SNAPSHOT_CXX

#include "Snapshot.h"
#include "UtilFunc.h"
#include "IOVDataError.h"

namespace lariov {

  template <class T>
  Snapshot<T>::Snapshot(std::string folder)
    : _folder(folder)
  {this->clear();}

  template <class T>
  Snapshot<T>::Snapshot(const std::string& folder,
			const std::vector<std::string>& field_name,
			const std::vector<std::string>& field_type)
    : Snapshot(folder,field_name,::lariov::Str2ValueType(field_type))
  {}

  template <class T>
  Snapshot<T>::Snapshot(const std::string& folder,
			const std::vector<std::string>& field_name,
			const std::vector< ::lariov::ValueType_t>& field_type)
    : _folder(folder)
    , _field_name(field_name)
    , _field_type(field_type)
  {

    if(field_name.size()!=field_type.size())
      throw IOVDataError("Name & Type column array has mis-match in length!");

    size_t ctr=0;
    for(size_t i=0; i<_field_name.size(); ++i) {
      if(_field_name[i] == "channel") continue;
      if(_field_name_to_index.find(_field_name[i]) != _field_name_to_index.end())
	throw IOVDataError("Duplicate name used!");
      _field_name_to_index[_field_name[i]] = ctr;
      ctr++;
    }
  }

  template <class T>
  void Snapshot<T>::clear()
  { 
    std::vector<lariov::ChData<T> >::clear();
    _iov_start = kMAX_TIME;
    _iov_end = kMAX_TIME;
  }

  template <class T>
  const std::string& Snapshot<T>::Folder () const { return _folder;      }

  template <class T>
  const TTimeStamp&  Snapshot<T>::Start() const { return _iov_start; }

  template <class T>
  const TTimeStamp&  Snapshot<T>::End()   const { return _iov_end;   }

  template <class T>
  const std::string& Snapshot<T>::Tag() const { return _tag; }

  template <class T>
  bool  Snapshot<T>::Valid(const TTimeStamp& ts) const 
  { return (_iov_start < ts && ts < _iov_end); }

  template <class T>
  size_t Snapshot<T>::NChannels() const { return this->size();      }

  template <class T>
  size_t Snapshot<T>::NFields()   const { return _field_name.size(); }

  template <class T>
  const std::string& Snapshot<T>::FieldName(const size_t column) const
  {
    if(column >= _field_name.size()) 
      throw IOVDataError("Invalid column number requested!");
    return _field_name[column];
  }

  template <class T>
  ValueType_t Snapshot<T>::FieldType(const size_t column) const
  {
    if(column >= _field_type.size())
      throw IOVDataError("Invalid column number requested!");
    return _field_type[column];
  }

  template <class T>
  const std::string Snapshot<T>::FieldTypeString(const size_t column) const
  {
    if(column >= _field_type.size())
      throw IOVDataError("Invalid column number requested!");
    return ValueType2Str(_field_type[column]);
  }

  template <class T>
  const std::vector<std::string>& Snapshot<T>::FieldName() const { return _field_name; }

  template <class T>
  const std::vector<ValueType_t>& Snapshot<T>::FieldType() const { return _field_type; }

  template <class T>
  const std::vector<std::string> Snapshot<T>::FieldTypeString() const 
  { 
    std::vector<std::string> res;
    res.reserve(_field_type.size());
    for(auto const& v : _field_type)
      res.push_back(ValueType2Str(v));
    return res;
  }

  template <class T>
  size_t Snapshot<T>::Name2Index(const std::string& field_name) const
  {
    auto const& iter = _field_name_to_index.find(field_name);
    if(iter == _field_name_to_index.end()) {
      std::string msg("Field not found" + field_name);
      throw IOVDataError(msg.c_str());
    }
    return (*iter).second;
  }

  template <class T>
  bool Snapshot<T>::Compat(const Snapshot<T>& data) const
  {
    if(data.Folder() != _folder) return false;
    if(data.Tag() != _tag) return false;
    return Compat(data.FieldName(),data.FieldType());
  }

  template <class T>
  bool Snapshot<T>::Compat(const std::vector<std::string>& field_name,
			   const std::vector<std::string>& field_type) const
  { return Compat(field_name,Str2ValueType(field_type)); }

  template <class T>
  bool Snapshot<T>::Compat(const std::vector<std::string>& field_name,
			   const std::vector< ::lariov::ValueType_t> field_type) const
  {
    if(FieldName().size() != field_name.size() ||
       FieldType().size() != field_type.size() ) return false;
    
    for(size_t i=0; i<this->FieldName().size(); ++i) {
      
      auto const& name1 = this->FieldName()[i];
      auto const& name2 = field_name[i];
      auto const& type1 = this->FieldType()[i];
      auto const& type2 = field_type[i];
      
      if(name1 != name2 || type1 != type2)
	return false;
    }
    return true;
  }

  template <class T>
  void Snapshot<T>::Reset (const TTimeStamp iov_start)
  {
    this->clear();
    _iov_start = iov_start;
    _iov_end   = kMAX_TIME;
  }

  template <class T>
  void Snapshot<T>::Reset(const TTimeStamp& start, const TTimeStamp& end, const std::string tag)
  {
    if(start >= end)
      throw IOVDataError("Cannot set start time >= end time!");
    this->clear();
    _iov_start = start;
    _iov_end   = end;
    _tag = tag;
  }

  template <class T>
  const lariov::ChData<T>& Snapshot<T>::Row(const size_t n) const
  {
    if(n >= this->size())
      throw IOVDataError("Invalid row number requested!");
    return (*this)[n];
  }

  template <class T>
  const lariov::ChData<T>& Snapshot<T>::ChData(const unsigned int ch) const
  {
    auto iter = std::lower_bound (this->begin(), this->end(), ch);
    if(iter == this->end()) {
      std::string msg("Channel not found: ");
      msg += std::to_string(ch);
      throw IOVDataError(msg);
    }
    return (*iter);
  }
  
  template <class T>
  void Snapshot<T>::push_back(const lariov::ChData<T>& data)
  {
    
    if(!(_field_type.size())) throw IOVDataError("Not configured yet toadd ChData!!");
    
    if(data.size() != (_field_type.size()))
      throw IOVDataError("Invalid number of columns in the new row!");
    
    bool sort = (this->size() && data < this->back());
    
    std::vector<lariov::ChData<T> >::push_back(data);
    
    if(sort) std::sort(this->begin(),this->end());
    
  }

}

#endif
