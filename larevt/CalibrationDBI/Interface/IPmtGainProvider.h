/**
 * \file IPmtGainProvider
 * 
 * \brief Class def header for a class IPmtGainProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef IPMTGAINPROVIDER_H
#define IPMTGAINPROVIDER_H

#include "CalibrationDBIFwd.h"

namespace lariov {

  /**
   * @brief Abstract interface class providing PMT Gain information
   *
   * Currently, the class provides interface for the following information:
   * - single photo-electron height and its error
   * - single photo-electron width and its error
   * - single photo-electron area and its error
   */
  class IPmtGainProvider {
  
    public:
    
      virtual ~IPmtGainProvider() = default;
       
      /// Retrieve pmt gain information     
      virtual float SpeHeight(DBChannelID_t ch) const = 0;
      virtual float SpeHeightErr(DBChannelID_t ch) const = 0;
      
      virtual float SpeWidth(DBChannelID_t ch) const = 0;
      virtual float SpeWidthErr(DBChannelID_t ch) const = 0;
      
      virtual float SpeArea(DBChannelID_t ch) const = 0;
      virtual float SpeAreaErr(DBChannelID_t ch) const = 0;
      
      /// Update local state of implementation
      virtual bool Update(DBTimeStamp_t ts) = 0;
  };
}//end namespace lariov

#endif
