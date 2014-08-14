///////////////////////////////////////////////////////
//
// ChannelFilter Class
//
//
//  pagebri3@msu.edu
//
///////////////////////////////////////////////////////
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Handle.h" 
#include "art/Framework/Services/Registry/ServiceHandle.h" 
#include "art/Framework/Services/Optional/TFileService.h" 
#include "art/Framework/Services/Optional/TFileDirectory.h" 
#include "messagefacility/MessageLogger/MessageLogger.h" 
#include "cetlib/exception.h"

#include "Filters/ChannelFilter.h"
#include "Geometry/Geometry.h"


///////////////////////////////////////////////////////
filter::ChannelFilter::ChannelFilter()
{
  art::ServiceHandle<geo::Geometry> geochan;

  LOG_WARNING("ChannelFilter") << "ChannelFilter is not defined for any experiment. "
			       << "\n Each experiment should implement its own filter for "
			       << "bad channels in its experimental code base";
  
  // populate the set of bad channels for this detector
  // This code should eventually hook up to a database
  
}

///////////////////////////////////////////////////////
filter::ChannelFilter::~ChannelFilter()
{
}

///////////////////////////////////////////////////////
bool filter::ChannelFilter::BadChannel(uint32_t channel) 
{
  if(fBadChannels.find(channel) != fBadChannels.end()) return true;
  return false;  
}

///////////////////////////////////////////////////////
bool filter::ChannelFilter::NoisyChannel(uint32_t channel) 
{
  if(fNoisyChannels.find(channel) != fNoisyChannels.end()) return true;
  return false;
}


