/**
 * \file ChannelStatus.h
 *
 * \ingroup IOVData
 * 
 * \brief Class def header for a class ChannelStatus
 *
 * @author eberly@slac.stanford.edu
 */

/** \addtogroup IOVData

    @{*/
#ifndef IOVDATA_CHANNELSTATUS_H
#define IOVDATA_CHANNELSTATUS_H 1

#include "ChData.h"

namespace lariov {
  
  enum chStatus {kDISCONNECTED=0, kBAD=1, kLOWNOISE=2, kNOISY=3, kGOOD=4, kUNKNOWN=5};
  
  /**
     \class ChannelStatus
  */
  class ChannelStatus : public ChData {
    
    public:
    
      /// Constructor
      ChannelStatus(unsigned int ch) : ChData(ch) {}
      
      /// Default destructor
      ~ChannelStatus() = default;
            
      bool IsBad()       const { return fStatus == kBAD          ? true : false; }
      bool IsLowNoise()  const { return fStatus == kLOWNOISE     ? true : false; }
      bool IsNoisy()     const { return fStatus == kNOISY        ? true : false; }
      bool IsPresent()   const { return fStatus == kDISCONNECTED ? false : true; }
      bool IsGood()      const { return fStatus == kGOOD         ? true : false; }
      chStatus Status()  const { return fStatus; }

      void SetStatus(chStatus status) { fStatus = status; }
      
    private:
      chStatus fStatus;
  }; //end class
} //end namespace lariov

#endif
/** @} */ //end doxygen group
