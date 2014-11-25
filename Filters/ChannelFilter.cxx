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

// LArSoft libraries
#include "Filters/ChannelFilterServiceInterface.h"


///////////////////////////////////////////////////////
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
    ->BadChannel(channel);
}

///////////////////////////////////////////////////////
bool filter::ChannelFilter::NoisyChannel(uint32_t channel) const{
  return art::ServiceHandle<filter::ChannelFilterServiceInterface>()
    ->NoisyChannel(channel);
}

///////////////////////////////////////////////////////
std::set<uint32_t> filter::ChannelFilter::SetOfBadChannels() const {
  return art::ServiceHandle<filter::ChannelFilterServiceInterface>()
    ->SetOfBadChannels();
}

///////////////////////////////////////////////////////
std::set<uint32_t> filter::ChannelFilter::SetOfNoisyChannels() const {
  return art::ServiceHandle<filter::ChannelFilterServiceInterface>()
    ->SetOfNoisyChannels();
}

