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
  // This code should eventually hook up to a database
  // put argoneut list back in temporarily, this will be removed when a service is implemented
  if(geochan->DetectorName() == "argoneut"){
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
  else if(geochan->DetectorName().find("microboone")==0)
  {
      const size_t nNoisy = 81;
      size_t noisyChan[nNoisy] = {
            2351, 2350, 2349, 2348, 2347, 2346, 2345, 2344, 2343, 2342, 2341, 2340, 2339, 2338, 2337
          , 623 , 622 , 621 , 620 , 619 , 618 , 617 , 611 , 628 , 627 , 626 , 625 , 624 , 648 , 647 , 646 , 645 , 644 , 643 , 642
          , 641 , 640 , 671 , 670 , 669 , 666 , 663 , 662 , 661 , 660 , 659 , 658 , 657 , 656 , 303 , 302 , 301 , 300 , 319 , 307
          , 306 , 305 , 304 , 399 , 384 , 400 , 5328, 5327, 2992, 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 3078
          , 5   , 4   , 3   , 2   , 1   , 0
      };
      
      for (size_t idx = 0; idx < nNoisy; idx++) fChannelStatusMap[noisyChan[idx]] = NOISY;
      
      LOG_DEBUG("ChannelFilter") << fChannelStatusMap.size() << " wires with status other than GOOD for MicroBooNE";
  }
  else
      // Issue the following warning only if not argoneut or microboone
      // (sorry Gianluca, I know this module should/will be replaced so we don't need to see the message!)
        LOG_WARNING("ChannelFilter") << "ChannelFilter is not defined for any experiment. "
			                         << "\n Each experiment should implement its own filter for "
			                         << "bad channels in its experimental code base";
  
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

///////////////////////////////////////////////////////
filter::ChannelFilter::ChannelStatus filter::ChannelFilter::GetChannelStatus(uint32_t channel) const
{
    // In theory ChannelFilter should be a service in which case one can
    // keep a data member reference for the geometry service... which can
    // be updated as needed.
    // Until then we need to recover each time we call this routine... painful!
    art::ServiceHandle<geo::Geometry> geochan;
    
    // Make sure the channel is in range!
    if (channel > geochan->Nchannels()) return ChannelFilter::NOTPHYSICAL;
    
    if (fChannelStatusMap.find(channel) != fChannelStatusMap.end())
        return fChannelStatusMap.find(channel)->second;
    
    return ChannelFilter::GOOD;
}



