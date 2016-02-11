/**
 * \file IDetPedestalProvider
 * 
 * \brief Class def header for a class IDetPedestalProvider
 *
 * @author eberly@slac.stanford.edu
 */

#ifndef IDETPEDESTALPROVIDER_H
#define IDETPEDESTALPROVIDER_H

// LArSoft libraries
#include "larcore/SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t


namespace lariov {

  /**
     \class IDetPedestalProvider
     Pure abstract interface class for retrieving detector pedestals.
     Includes a feature to encourage database use: an Update method that can be used to update 
     an implementation's local state to ensure that the correct information is retrieved
  */
  class IDetPedestalProvider {
  
    public:
    
      virtual ~IDetPedestalProvider() = default;
       
      /// Retrieve pedestal information     
      virtual float PedMean(raw::ChannelID_t ch) const = 0;
      virtual float PedRms(raw::ChannelID_t ch) const = 0;
      virtual float PedMeanErr(raw::ChannelID_t ch) const = 0;
      virtual float PedRmsErr(raw::ChannelID_t ch) const = 0;
      
    /* TODO DELME
      /// Update local state of implementation
      virtual bool Update(DBTimeStamp_t ts) = 0;
    */
  };
}//end namespace lariov

#endif
