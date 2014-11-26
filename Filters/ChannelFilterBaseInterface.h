/**
 * @file   ChannelFilterBaseInterface.h
 * @brief  Interface for experiment-specific channel quality info provider
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    ChannelFilterServiceInterface.h
 *
 * This is the interface of ChannelFilter core object.
 * A ChannelFilter object (with the interface of ChannelFilter core object)
 * provides the actual information about channels.
 * It can be instanciated by a art service (ChennelFilterServiceInterface)
 * or from whatever system needs it.
 */


#ifndef CHANNELFILTERBASEINTERFACE_H
#define CHANNELFILTERBASEINTERFACE_H 1

// C/C++ standard libraries
#include <cstdlib> // std::uint32_t
#include <set>
#include <vector>

// Additional libraries
#include "fhiclcpp/ParameterSet.h"


/// Filters for channels, events, etc
namespace filter {


#ifdef art_Persistency_Provenance_RunID_h
  using RunNumber_t = art::RunNumber_t;
#else
  using RunNumber_t = std::uint32_t;
#endif

  /** **************************************************************************
   * @brief Class providing information about the quality of channels
   *
   * This class provides only a simple interface.
   * Experiments need to implement and configure their own class implementing
   * this interface.
   * The simplest implementation is provided in LArSoft:
   * SimpleChannelFilter.
   * 
   * Currently, the class provides interface for the following information:
   * - goodness of the channel: good or bad (dead or unusable)
   * - noisiness of the channel: good or noisy (or compromised in some way)
   * 
   * It also has a stub interface to inform the object of which run we are
   * interested in.
   * 
   */
  class ChannelFilterBaseInterface {
      public:
    using channel_type = std::uint32_t; ///< type of channel ID
    using channel_list_type = std::set<channel_type>;
      ///< type of a list of channel IDs
    
    
    virtual ~ChannelFilterBaseInterface() = default;
    
    /// Returns whether the specified channel is bad in the current run
    virtual bool BadChannel(channel_type channel) const
      { return SetOfBadChannels().count(channel) > 0; }
    
    /// Returns whether the specified channel is noisy in the current run
    virtual bool NoisyChannel(channel_type channel) const
      { return SetOfNoisyChannels().count(channel) > 0; }
    
    /// Returns a copy of set of bad channel IDs for the current run
    virtual channel_list_type SetOfBadChannels()   const = 0;
    
    /// Returns a copy of set of noisy channel IDs for the current run
    virtual channel_list_type SetOfNoisyChannels() const = 0;
    
    /// Prepares the object to provide information about the specified run
    /// @return whether information is available for the specified run
    virtual bool SetRun(RunNumber_t run) = 0;
    
  }; // class ChannelFilterBaseInterface
  
} // namespace filter


#endif // CHANNELFILTERBASEINTERFACE_H
