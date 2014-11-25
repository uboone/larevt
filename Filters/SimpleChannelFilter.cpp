/**
 * @file   SimpleChannelFilter.cpp
 * @brief  Channel quality provider with information from configuration file
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 25th, 2014
 * @see    SimpleChannelFilter.h
 */

// Our header
#include "Filters/SimpleChannelFilter.h"


// C/C++ standard libraries
#include <vector>
#include <algorithm> // std::copy()
#include <iterator> // std::inserter()


namespace filter {

  //----------------------------------------------------------------------------
  SimpleChannelFilter::SimpleChannelFilter(fhicl::ParameterSet const& pset) {
    
    using chan_vect_t = std::vector<channel_type>;
    
    // Read the bad channels as a vector, then convert it into a set
    chan_vect_t BadChannels
      = pset.get<chan_vect_t>("BadChannels", chan_vect_t());
    std::copy(
      BadChannels.begin(), BadChannels.end(),
      std::inserter(fBadChannels, fBadChannels.begin())
      );
    
    // Read the noise channels as a vector, then convert it into a set
    chan_vect_t NoisyChannels
      = pset.get<chan_vect_t>("NoisyChannels", chan_vect_t());
    std::copy(
      NoisyChannels.begin(), NoisyChannels.end(),
      std::inserter(fNoisyChannels, fNoisyChannels.begin())
      );
    
  } // SimpleChannelFilter::SimpleChannelFilter()
  
  
} // namespace filter
