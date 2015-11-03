///////////////////////////////////////////////////////
//
// ChannelFilter Class
// 
// This class has been obsoleted and is now a deprecated interface for
// IChannelStatusService.
// 
// Please update your code to use the service directly.
// 
// 
// Original class: pagebri3@msu.edu
//
///////////////////////////////////////////////////////


// Our header
#include "Filters/ChannelFilter.h"


// Framework libraries
#include "messagefacility/MessageLogger/MessageLogger.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Utilities/Exception.h"

// LArSoft libraries
#include "CalibrationDBI/Interface/IChannelStatusService.h"
#include "CalibrationDBI/Interface/IChannelStatusProvider.h"

#include <iostream>


filter::ChannelFilter::ChannelFilter() {
  
  if (!&*(art::ServiceHandle<lariov::IChannelStatusService>())) {
    throw art::Exception(art::errors::Configuration)
      << "Failed to obtain an instance of IChannelStatusService service"
      ;

  }
  LOG_ERROR("ChannelFilter") << "ChannelFilter is now deprecated."
    " Replace it with IChannelStatusService";
  
} // filter::ChannelFilter::ChannelFilter()


///////////////////////////////////////////////////////
bool filter::ChannelFilter::BadChannel(uint32_t channel) const {
  return art::ServiceHandle<lariov::IChannelStatusService>()
    ->GetFilter().IsBad(channel);
}

///////////////////////////////////////////////////////
bool filter::ChannelFilter::NoisyChannel(uint32_t channel) const{
  return art::ServiceHandle<lariov::IChannelStatusService>()
    ->GetFilter().IsNoisy(channel);
}

///////////////////////////////////////////////////////
std::set<uint32_t> filter::ChannelFilter::SetOfBadChannels() const {
  return art::ServiceHandle<lariov::IChannelStatusService>()
    ->GetFilter().BadChannels();
}

///////////////////////////////////////////////////////
std::set<uint32_t> filter::ChannelFilter::SetOfNoisyChannels() const {
  return art::ServiceHandle<lariov::IChannelStatusService>()
    ->GetFilter().NoisyChannels();
}

///////////////////////////////////////////////////////
filter::ChannelFilter::ChannelStatus filter::ChannelFilter::GetChannelStatus(uint32_t channel) const
{
  
  lariov::IChannelStatusProvider const& filter
    = art::ServiceHandle<lariov::IChannelStatusService>()->GetFilter();

  if (filter.IsGood(channel))          return GOOD;
  else if (!filter.IsPresent(channel)) return NOTPHYSICAL;
  else if (filter.IsBad(channel))      return DEAD;
  else if (filter.IsNoisy(channel))    return NOISY;
  else return DEAD; //assume all other status are equivalent to DEAD
}
