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
      //      const size_t nNoisy = 81;
      //      size_t noisyChan[nNoisy] = {
      //          2351, 2350, 2349, 2348, 2347, 2346, 2345, 2344, 2343, 2342, 2341, 2340, 2339, 2338, 2337
      //          , 623 , 622 , 621 , 620 , 619 , 618 , 617 , 611 , 628 , 627 , 626 , 625 , 624 , 648 , 647 , 646 , 645 , 644 , 643 , 642
      //          , 641 , 640 , 671 , 670 , 669 , 666 , 663 , 662 , 661 , 660 , 659 , 658 , 657 , 656 , 303 , 302 , 301 , 300 , 319 , 307
      //          , 306 , 305 , 304 , 399 , 384 , 400 , 5328, 5327, 2992, 15  , 14  , 13  , 12  , 11  , 10  , 9   , 8   , 7   , 6   , 3078
      //          , 5   , 4   , 3   , 2   , 1   , 0
      //      };
      
      
      // Divide channel status into groups according to noise levels, signals seen, etc.
      
      // Start with intermittent
      std::vector<size_t> intermittent;
      
      for(const auto& channel : intermittent) fChannelStatusMap[channel] = INTERMITTENT;
      
      // The following are a list of "high noise" channels
      std::vector<size_t> noisyChan = {
           0,    1,    2,    3,    4,    5,    6,    7,    8,    9,  10,    11,   12,   13,   14,   15,
         628,
        2671
      };
      
      for (const auto& channel : noisyChan) fChannelStatusMap[channel] = NOISY;
      
      // The following are a list of low noise channels which are likely really "dead"
      std::vector<size_t> lowNoise = {
           112,  113,  114,  115,  116,  117,  118,  119,
           305,  306,
           320,  321,  322,  323,  324,  325,  326,  327,
           417,
           615,
           619,
           621,  622,
           627,
           636,
           666,
           670,
           872,  873,  874,  875,  876,  877,  878,  879,
          1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351,
          1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439,
          1936, 1937, 1938, 1939, 1940, 1941, 1942, 1943, 1944, 1945, 1946, 1947, 1948, 1949, 1950, 1951,
          2161, 2162,
          2164,
          2168, 2169, 2170, 2171, 2172, 2173, 2175,
          3184, 3185, 3186, 3187, 3188, 3189, 3190, 3191,
          3392, 3393, 3394, 3395, 3396, 3397, 3398, 3399,
          3944, 3945, 3946, 3947, 3948, 3949, 3950, 3951,
          4416, 4417, 4418, 4419, 4420, 4421, 4422, 4423,
          4496, 4497, 4498, 4499, 4500, 4501, 4502, 4503, 4504, 4505, 4506, 4507, 4508, 4509, 4510, 4511,
          4819,
          4882,
          4884,
          4976, 4977, 4978, 4979, 4980, 4981, 4982, 4983, 4984, 4985, 4986, 4987, 4988, 4989, 4990, 4991,
          5104, 5105, 5106, 5107, 5108, 5109, 5110, 5111, 5112, 5113, 5114, 5115, 5116, 5117, 5118, 5119,
          5136, 5137, 5138, 5139, 5140, 5141, 5142, 5143, 5144, 5145, 5146, 5147, 5148, 5149, 5150, 5151,
          5200, 5201, 5202, 5203, 5204, 5205, 5206, 5207, 5208, 5209, 5210, 5211, 5212, 5213, 5214, 5215,
          5554,
          5556,
          5616, 5617, 5618, 5619, 5620, 5621, 5622, 5623, 5624, 5625, 5626, 5627, 5628, 5629, 5630, 5631,
          5760, 5761, 5762, 5763, 5764, 5765, 5766, 5767, 5768, 5769, 5770, 5771, 5772, 5773, 5774, 5775,
          5952, 5953, 5954,
          5956, 5957,
          5959, 5960,
          5963, 5964,
          6176, 6177, 6178, 6179, 6180, 6181, 6182, 6183, 6184, 6185, 6186, 6187, 6188, 6189, 6190, 6191,
          6392,
          7010,
          7013,
          7017, 7018, 7019,
          7022, 7023,
          7027,
          7138,
          7147,
          7152,
          7166,
          7168, 7169, 7170, 7171, 7172, 7173, 7174, 7175, 7176, 7177, 7178, 7179, 7180, 7181, 7182, 7183,
          7198
      };
      
      for(const auto& channel : lowNoise) fChannelStatusMap[channel] = LOWNOISE;
      
      // Finally, the following are a list of really dead channels
      std::vector<size_t> dead = {
           640,
           647,
           650,
           654,
           658,
           660
      };
      
      for(const auto& channel : dead) fChannelStatusMap[channel] = DEAD;
      
      // In addition to the above list, we know that channels 864-911 and 1536-1583 are not connected
      for(size_t channel =  864; channel <  912; channel++) fChannelStatusMap[channel] = DEAD;
      for(size_t channel = 3936; channel < 3984; channel++) fChannelStatusMap[channel] = DEAD;
      
      // Now make backward compatible
      for(const auto& chanItr : fChannelStatusMap)
      {
          if      (chanItr.second >= LOWNOISE) fBadChannels.insert(chanItr.first);
          else if (chanItr.second == NOISY)    fNoisyChannels.insert(chanItr.first);
      }
      
      LOG_DEBUG("ChannelFilter") << fChannelStatusMap.size() << " wires with status other than GOOD for MicroBooNE";
      LOG_DEBUG("ChannelFilter") << "   Channels labeled as dead: " << dead.size() << std::endl;
      LOG_DEBUG("ChannelFilter") << "   Channels labeled as lownoise: " << lowNoise.size() << std::endl;
      LOG_DEBUG("ChannelFilter") << "   Channels labeled as noisy: " << noisyChan.size() << std::endl;
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



