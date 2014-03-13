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
  
  // populate the set of bad channels for this detector
  ///\todo This code should eventually hook up to a database
  if(geochan->DetId() == geo::kArgoNeuT){
    fBadChannels.insert(22);
    fBadChannels.insert(65);
    fBadChannels.insert(237);
    fBadChannels.insert(307);
    fBadChannels.insert(308);
    fBadChannels.insert(309);
    fBadChannels.insert(310);
    fBadChannels.insert(311);
    fBadChannels.insert(410);
    fBadChannels.insert(412);
    fBadChannels.insert(438);
    fBadChannels.insert(439);
    fBadChannels.insert(448);
    

    fNoisyChannels.insert(31);
    fNoisyChannels.insert(41);
    fNoisyChannels.insert(108);
    fNoisyChannels.insert(120);
    fNoisyChannels.insert(121);
    fNoisyChannels.insert(124);
    fNoisyChannels.insert(392);
    fNoisyChannels.insert(399);
  }

  if(geochan->DetId() == geo::kBo){
    fBadChannels.insert(64);
    fBadChannels.insert(65);
    fBadChannels.insert(66);
    fBadChannels.insert(67);
    fBadChannels.insert(68);
    fBadChannels.insert(69);
    fBadChannels.insert(70);
    fBadChannels.insert(71);
    fBadChannels.insert(72);
    fBadChannels.insert(73);
    fBadChannels.insert(74);
    fBadChannels.insert(75);
    fBadChannels.insert(76);
    fBadChannels.insert(77);
    fBadChannels.insert(78);
    fBadChannels.insert(79);
  }
  
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


