////////////////////////////////////////////////////////////////////////
//
// ChannelFilter class:
//
// This class provides methods for returning the condition of
// a wire, as to whether it is bad and is to be ignored or perhaps
// if it has some known problem.  This allows the removal of detector
// specific code from a few places in LArSoft.  Right now is is only 
// implemented for Argoneut.
//  
//
// pagebri3@msu.edu
//
////////////////////////////////////////////////////////////////////////
#ifndef CHANNELFILTER_H
#define CHANNELFILTER_H

#include <set>
#include <stdint.h>

///filters for channels, events, etc
namespace filter {

  class ChannelFilter {

  public:

    ChannelFilter();
    ~ChannelFilter();

    bool BadChannel(uint32_t channel);
    bool NoisyChannel(uint32_t channel);
    std::set<uint32_t> SetOfBadChannels()   const { return fBadChannels;   }
    std::set<uint32_t> SetOfNoisyChannels() const { return fNoisyChannels; }
  private:

    std::set<uint32_t> fBadChannels;   ///< list of bad channels
    std::set<uint32_t> fNoisyChannels; ///< list of bad channels

  }; //class ChannelFilter
}
#endif // CHANNELFILTER_H


