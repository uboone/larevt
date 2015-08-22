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
              2401, 2402, 2403, 2404, 2405, 2406, 2407, 2408, 2409, 2410, 2411, 2412, 2413, 2414, 2415,
        3028,
              4801, 4802, 4803, 4804, 4805, 4806, 4807, 4808, 4809, 4810, 4811, 4812, 4813, 4814, 4815,
        5428
          
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
           627,  628,
           636,
           640,
           647,
           650,
           658,
           660,
           666,
           670,
           872,  873,  874,  875,  876,  877,  878,  879,
          1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351,
          1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439,
          1936, 1937, 1938, 1939, 1940, 1941, 1942, 1943, 1944, 1945, 1946, 1947, 1948, 1949, 1950, 1951,
          2161, 2162,
          2164,
          2168, 2169, 2170, 2171, 2172, 2173, 2175,
          2512, 2513, 2514, 2515, 2516, 2517, 2518, 2519,
          2705, 2706,
          2720, 2721, 2722, 2723, 2724, 2725, 2726, 2727,
          2817,
          3015,
          3019,
          3021, 3022,
          3027,
          3036,
          3066,
          3070,
          3272, 3273, 3274, 3275, 3276, 3277, 3278, 3279,
          3744, 3745, 3746, 3747, 3748, 3749, 3750, 3751,
          3824, 3825, 3826, 3827, 3828, 3829, 3830, 3831, 3832, 3833, 3834, 3835, 3836, 3837, 3838, 3839,
          4336, 4337, 4338, 4339, 4340, 4341, 4342, 4343, 4344, 4345, 4346, 4347, 4348, 4349, 4350, 4351,
          4561, 4562,
          4564,
          4568, 4569, 4570, 4571, 4572, 4573, 4574, 4575,
          4912, 4913, 4914, 4915, 4916, 4917, 4918, 4919,
          5105, 5106,
          5120, 5121, 5122, 5123, 5124, 5125, 5126, 5127,
          5217,
          5415,
          5419,
          5421, 5422,
          5427,
          5436,
          5466,
          5470,
          5672, 5673, 5674, 5675, 5676, 5677, 5678, 5679,
          6144, 6145, 6146, 6147, 6148, 6149, 6150, 6151,
          6224, 6225, 6226, 6227, 6228, 6229, 6230, 6231, 6232, 6233, 6234, 6235, 6236, 6237, 6238, 6239,
          6736, 6737, 6738, 6739, 6740, 6741, 6742, 6743, 6744, 6745, 6746, 6747, 6748, 6749, 6750, 6751,
          6961, 6962,
          6964,
          6968, 6969, 6970, 6971, 6972, 6973, 6974, 6975
      };
      
      for(const auto& channel : lowNoise) fChannelStatusMap[channel] = LOWNOISE;
      
      // Finally, the following are a list of really dead channels
      std::vector<size_t> dead = {
           640,
           647,
           650,
           654,
           658,
           660,
          3040,
          3047,
          3050,
          3054,
          3058,
          3060,
          5440,
          5447,
          5450,
          5454,
          5458,
          5460
      };
      
      for(const auto& channel : dead) fChannelStatusMap[channel] = DEAD;
      
      // In addition to the above list, we know that channels 864-911 and 1536-1583 are not connected
      for(size_t channel =  864; channel <  912; channel++) fChannelStatusMap[channel] = DEAD;
      for(size_t channel = 1536; channel < 1584; channel++) fChannelStatusMap[channel] = DEAD;
      
      // Now make backward compatible
      for(const auto& chanItr : fChannelStatusMap)
      {
          if      (chanItr.second >= LOWNOISE) fBadChannels.insert(chanItr.first);
          else if (chanItr.second == NOISY)    fNoisyChannels.insert(chanItr.first);
      }
      
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



