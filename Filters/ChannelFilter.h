////////////////////////////////////////////////////////////////////////
//
// ChannelFilter class:
// 
// This class has been obsoleted and is now a deprecated interface for
// ChannelFilterServiceInterface.
// 
// Please update your code to use the service directly.
// 
// 
// Original class: pagebri3@msu.edu
// 
////////////////////////////////////////////////////////////////////////
#ifndef CHANNELFILTER_H
#define CHANNELFILTER_H

// C/C++ standard libraries
#include <set>
#include <stdint.h>


namespace filter {

  class ChannelFilter {

  public:
    
    enum ChannelStatus { GOOD        = 0,
                         NOISY       = 1,
                         DEAD        = 2,
                         NOTPHYSICAL = 3
                       };

    ChannelFilter();
    
    bool BadChannel(uint32_t channel) const;
    bool NoisyChannel(uint32_t channel) const;
    std::set<uint32_t> SetOfBadChannels() const;
    std::set<uint32_t> SetOfNoisyChannels() const;
    ChannelStatus GetChannelStatus(uint32_t channel) const;
    
  }; //class ChannelFilter
}
#endif // CHANNELFILTER_H
