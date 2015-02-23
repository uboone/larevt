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
#ifndef IOVDATA_SNAPSHOT_H
#define IOVDATA_SNAPSHOT_H

#include <algorithm>
#include <vector>
#include <TTimeStamp.h>
#include "ChData.h"
#include "IOVDataError.h"
#include "IOVDataConstants.h"

#ifdef CALIBDB_LOCALL_BUILD
  #include <TObject.h>
#endif

namespace lariov {

  /**
     \class Snapshot
  */
  template <class T>
  class Snapshot : public std::vector<T> 
  #ifdef CALIBDB_LOCAL_BUILD
		 , public TObject
  #endif
  {
		   
    public:

      /// Default constructor
      Snapshot() {}

      /// Default destructor
      ~Snapshot(){}

      void clear();

      const TTimeStamp&  Start() const {return fStart;}
      const TTimeStamp&  End()   const {return fEnd;}
      void SetIoV(const TTimeStamp& start, const TTimeStamp& end);
      
      bool  IsValid(const TTimeStamp& ts) const;
      
      size_t NChannels() const {return this->size();}

      
      /// Only valid if T has base class ChData
      typename std::enable_if<std::is_base_of<lariov::ChData, T>::value, const T&>::type
      GetRow(const unsigned int ch) const
      {
        for (typename std::vector<T>::iterator it=this->begin(); it!=this->end(); ++it) {
	  if ( it->Channel() == ch ) return *it;
	}
	
	std::string msg("Channel not found: ");
	msg += std::to_string(ch);
	throw IOVDataError(msg);
      }

      /// keep vector sorted just in case it is useful
      inline void push_back(const T& data)
      {
	bool sort = (this->size() && data < this->back());
	std::vector<T>::push_back(data);
	if(sort) std::sort(this->begin(),this->end());
      }

    private:

      TTimeStamp  fStart;
      TTimeStamp  fEnd;
    
    //needed to derive from TObject
    #ifdef CALIBDB_LOCAL_BUILD
      ClassDef(Snapshot,1)
    #endif
  };

  //=============================================
  // Class implementation
  //=============================================
  template <class T>
  void Snapshot<T>::clear() {
    this->std::vector<T>::clear();
    fStart = kMAX_TIME;
    fEnd = kMAX_TIME;
  }
  
  template <class T>
  void Snapshot<T>::SetIoV(const TTimeStamp& start, const TTimeStamp& end) {
    if (start >= end) {
      throw IOVDataError("Called Snapshot::SetIoV with start timestamp >= end timestamp!");
    }
    
    fStart = start;
    fEnd   = end;
  }
      
  template <class T>
  bool Snapshot<T>::IsValid(const TTimeStamp& ts) const {
    return (fStart < ts && ts < fEnd); 
  }

}//end namespace lariov
#endif
/** @} */ // end of doxygen group 

