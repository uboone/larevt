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

// LArSoft libraries
#include "SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t
#include "CalibrationDBI/IOVData/IOVTimeStamp.h" // lariov::IOVTimeStamp

// Additional libraries
#include "fhiclcpp/ParameterSet.h"

// C/C++ standard libraries
#include <set>


/// Filters for channels, events, etc
namespace filter {
  
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
   * - physical channel: physically connected to a wire or not
   * 
   * It also has a stub interface to inform the object of which time we are
   * interested in.
   * 
   */
  class ChannelFilterBaseInterface {
      public:
    /// type of a list of channel IDs
    using ChannelSet_t = std::set<raw::ChannelID_t>;
    
    
    virtual ~ChannelFilterBaseInterface() = default;
    
    /// Returns whether the specified channel is physical and connected to wire
    virtual bool isPresent(raw::ChannelID_t channel) const = 0;
    
    /// Returns whether the specified channel is physical and good
    virtual bool isGood(raw::ChannelID_t channel) const
      { return isPresent(channel) && !isBad(channel) && !isNoisy(channel); }
    
    /// Returns whether the specified channel is bad in the current run
    virtual bool isBad(raw::ChannelID_t channel) const = 0;
    
    /// Returns whether the specified channel is noisy in the current run
    virtual bool isNoisy(raw::ChannelID_t channel) const = 0;
    
    
    /// Returns a copy of set of good channel IDs for the current run
    virtual ChannelSet_t GoodChannels() const = 0;
    
    /// Returns a copy of set of bad channel IDs for the current run
    virtual ChannelSet_t BadChannels() const = 0;
    
    /// Returns a copy of set of noisy channel IDs for the current run
    virtual ChannelSet_t NoisyChannels() const = 0;
    
    /// Prepares the object to provide information about the specified time
    /// @return whether information is available for the specified time
    virtual bool Update(lariov::IOVTimeStamp const& ts) = 0;
    
  }; // class ChannelFilterBaseInterface
  
} // namespace filter


#endif // CHANNELFILTERBASEINTERFACE_H
