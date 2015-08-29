///////////////////////////////////////////////////////
//
// ChannelFilter Class
// 
// This class has been obsoleted and is now a deprecated interface for
// ChannelFilterServiceInterface.
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

<<<<<<< HEAD
// LArSoft libraries
#include "Filters/ChannelFilterServiceInterface.h"
#include "Filters/ChannelFilterBaseInterface.h"


filter::ChannelFilter::ChannelFilter() {
  
  if (!&*(art::ServiceHandle<filter::ChannelFilterServiceInterface>())) {
    throw art::Exception(art::errors::Configuration)
      << "Failed to obtain an instance of ChannelFilterServiceInterface service"
      ;

  }
  LOG_ERROR("ChannelFilter") << "ChannelFilter is now deprecated."
    " Replace it with ChannelFilterServiceInterface";
  
} // filter::ChannelFilter::ChannelFilter()


///////////////////////////////////////////////////////
bool filter::ChannelFilter::BadChannel(uint32_t channel) const {
  return art::ServiceHandle<filter::ChannelFilterServiceInterface>()
    ->GetFilter().isBad(channel);
}

///////////////////////////////////////////////////////
bool filter::ChannelFilter::NoisyChannel(uint32_t channel) const{
  return art::ServiceHandle<filter::ChannelFilterServiceInterface>()
    ->GetFilter().isNoisy(channel);
}

///////////////////////////////////////////////////////
std::set<uint32_t> filter::ChannelFilter::SetOfBadChannels() const {
  return art::ServiceHandle<filter::ChannelFilterServiceInterface>()
    ->GetFilter().BadChannels();
}

///////////////////////////////////////////////////////
std::set<uint32_t> filter::ChannelFilter::SetOfNoisyChannels() const {
  return art::ServiceHandle<filter::ChannelFilterServiceInterface>()
    ->GetFilter().NoisyChannels();
}

///////////////////////////////////////////////////////
filter::ChannelFilter::ChannelStatus filter::ChannelFilter::GetChannelStatus(uint32_t channel) const
{
  
  ChannelFilterBaseInterface const& filter
    = art::ServiceHandle<filter::ChannelFilterServiceInterface>()->GetFilter();
  
  if (!filter.isPresent(channel)) return NOTPHYSICAL;
  if (filter.isBad(channel)) return DEAD;
  if (filter.isNoisy(channel)) return NOISY;
  return GOOD;
}
