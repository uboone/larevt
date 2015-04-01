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
#include "IOVTimeStamp.h"
#include "ChData.h"
#include <sstream>
#include "IOVDataError.h"
#include "IOVDataConstants.h"

namespace lariov {

  /**
     \class Snapshot
  */
  template <class T>
  class Snapshot : public std::vector<T> {
		   
    public:

      /// Default constructor
      Snapshot() : 
        fStart(0,0), fEnd(0,0) {}

      /// Default destructor
      ~Snapshot(){}

      void clear();

      const IOVTimeStamp&  Start() const {return fStart;}
      const IOVTimeStamp&  End()   const {return fEnd;}
      void SetIoV(const IOVTimeStamp& start, const IOVTimeStamp& end);
      
      bool  IsValid(const IOVTimeStamp& ts) const;
      
      size_t NChannels() const {return this->size();}

      
      /// Only included with class if T has base class ChData
      template< class U = T,
                typename std::enable_if<std::is_base_of<ChData, U>::value, int>::type = 0>
      const T& GetRow(unsigned int ch) const {
       
        typename Snapshot<T>::const_iterator it = std::lower_bound(this->begin(), this->end(), ch);
	
	if ( it == this->end() || it->Channel() != ch ) {
          std::string msg("Channel not found: ");
	  msg += std::to_string(ch);
	  throw IOVDataError(msg);
	}
	
	return *it;
      }

      template< class U = T,
      		typename std::enable_if<std::is_base_of<ChData, U>::value, int>::type = 0>
      void AddOrReplaceRow(const T& data) {
        typename Snapshot<T>::iterator it = std::lower_bound(this->begin(), this->end(), data.Channel());	
        if (it == this->end() || data.Channel() != it->Channel() ) {
	  bool sort = ( !(this->empty()) && data < this->back());
	  this->std::vector<T>::push_back(data);
	  if (sort) std::sort(this->begin(), this->end());
        }
        else {
	  *it = data;
	}
      }

      template< class U = T,
      		typename std::enable_if<std::is_base_of<ChData, U>::value, int>::type = 0>
      void push_back(const T& data) {
        size_t original_size = this->size();
        this->AddOrReplaceRow(data);
	if (original_size == this->size()) {
	  std::stringstream msg;
	  msg <<"Warning: You just called push_back() and overwrote the cached data for channel "<<data.Channel();
	  msg <<".  Was this intended?";
	  throw IOVDataError(msg.str());
	}
      }
        
    private:

      IOVTimeStamp  fStart;
      IOVTimeStamp  fEnd;
  };

  //=============================================
  // Class implementation
  //=============================================
  template <class T>
  void Snapshot<T>::clear() {
    this->std::vector<T>::clear();
    fStart  = fEnd = IOVTimeStamp::MaxTimeStamp();
    fStart.SetStamp(fStart.Stamp()-1, fStart.SubStamp());
  }
  
  template <class T>
  void Snapshot<T>::SetIoV(const IOVTimeStamp& start, const IOVTimeStamp& end) {
    if (start >= end) {
      throw IOVDataError("Called Snapshot::SetIoV with start timestamp >= end timestamp!");
    }
    
    fStart = start;
    fEnd   = end;
  }
      
  template <class T>
  bool Snapshot<T>::IsValid(const IOVTimeStamp& ts) const {
    return (ts >= fStart && ts < fEnd); 
  }
    
}//end namespace lariov
#endif
/** @} */ // end of doxygen group 

