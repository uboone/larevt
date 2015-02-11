/**
 * \file Snapshot.h
 *
 * \ingroup Snapshot
 * 
 * \brief Class def header for a class Snapshot
 *
 * @author kterao
 */

/** \addtogroup Snapshot

    @{*/
#ifndef WEBDB_WEBDATA_H
#define WEBDB_WEBDATA_H

#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <TTimeStamp.h>
#include <TObject.h>
#include "ChData.h"
#include "IOVDataError.h"
#include "IOVDataConstants.h"
namespace lariov {

  template <class T>
  class WebReader;

  /**
     \class Snapshot
  */
  template <class T>
  class Snapshot : public std::vector< lariov::ChData<T> > 
#ifdef CALIBDB_LOCAL_BUILD
		 , public TObject
#endif
  {
		   
    friend class WebReader<T>;
  public:
    
    /// Default constructor
    Snapshot(std::string folder="noname" );
    
    /// Default destructor
    virtual ~Snapshot(){}

    /// Alternative ctor ... for creating Snapshot to be uploaded
    Snapshot(const std::string& folder,
	     const std::vector<std::string>& field_name,
	     const std::vector<std::string>& field_type);

    Snapshot(const std::string& name,
	     const std::vector<std::string>& field_name,
	     const std::vector< ::lariov::ValueType_t>& field_type);

    void clear();

    const std::string& Folder() const;
    const TTimeStamp&  Start() const;
    const TTimeStamp&  End()   const;
    const std::string& Tag() const;
    bool   Valid(const TTimeStamp& ts) const;
    size_t NChannels()    const;
    size_t NFields() const;

    const std::string& FieldName(const size_t column) const;
    ValueType_t        FieldType(const size_t column) const;
    const std::string  FieldTypeString(const size_t column) const;
    const std::vector<std::string>& FieldName() const;
    const std::vector<lariov::ValueType_t>& FieldType() const;
    const std::vector<std::string>  FieldTypeString() const;
    size_t Name2Index(const std::string& field_name) const;

    bool Compat(const Snapshot<T>& data) const;

    bool Compat(const std::vector<std::string>& field_name,
		const std::vector<std::string>& field_type) const;

    bool Compat(const std::vector<std::string>& field_name,
		const std::vector< ::lariov::ValueType_t> field_type) const;

    void Reset(const TTimeStamp start = ::lariov::kMIN_TIME);

  private:
    
    void Reset(const TTimeStamp& start, const TTimeStamp& end, const std::string tag="");
    
    //
    // Template functions
    //
  public:

    const lariov::ChData<T>& Row(const size_t n) const;

    const lariov::ChData<T>& ChData(const unsigned int ch) const;

    void push_back(const lariov::ChData<T>& data);

  private:
    std::string _folder;
    TTimeStamp  _iov_start;
    TTimeStamp  _iov_end;
    std::vector<std::string> _field_name;
    std::vector<lariov::ValueType_t> _field_type;
    std::string _tag;
    std::map<std::string,size_t> _field_name_to_index;

#ifdef CALIBDB_LOCAL_BUILD
    ClassDef(Snapshot,1)
#endif
  };
}

template class lariov::Snapshot< std::string >;
template class lariov::Snapshot< float  >;
template class lariov::Snapshot< double >;
template class lariov::Snapshot< short  >;
template class lariov::Snapshot< int    >;
template class lariov::Snapshot< long   >;
template class lariov::Snapshot< unsigned short >;
template class lariov::Snapshot< unsigned int   >;
template class lariov::Snapshot< unsigned long  >;

#endif
/** @} */ // end of doxygen group 

