/**
 * @file   IChannelStatusProvider.h
 * @brief  Interface for experiment-specific channel quality info provider
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    IChannelStatusService.h
 *
 * This is the interface of ChannelStatus service provider core object.
 * A ChannelStatus service provider object (with the interface of ChannelStatus
 * service provider core object) provides the actual information about channels.
 * It can be instanciated by a art service (an implementation of
 * IChannelStatusService) or from whatever system needs it.
 */


#ifndef ICHANNELSTATUSPROVIDER_H
#define ICHANNELSTATUSPROVIDER_H 1

// C/C++ standard libraries
#include <set>
#include <limits> // std::numeric_limits<>

// LArSoft libraries
#include "SimpleTypesAndConstants/RawTypes.h" // raw::ChannelID_t


/// Filters for channels, events, etc
namespace lariov {
  
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
  class IChannelStatusProvider {
    
    public:
      
      using Status_t = unsigned short; ///< type representing channel status
      
      /// Type of set of channel IDs
      using ChannelSet_t = std::set<raw::ChannelID_t>;
      
      /// Value or invalid status
      static constexpr Status_t InvalidStatus
        = std::numeric_limits<Status_t>::max();
      
      
      /// Virtual destructor; destructs nothing
      virtual ~IChannelStatusProvider() = default;
      
      /// Returns whether the specified channel is physical and connected to wire
      virtual bool IsPresent(raw::ChannelID_t channel) const = 0;
      
      /// Returns whether the specified channel is bad in the current run
      virtual bool IsBad(raw::ChannelID_t channel) const = 0;
      
      /// Returns whether the specified channel is noisy in the current run
      virtual bool IsNoisy(raw::ChannelID_t channel) const = 0;
      
      /// Returns whether the specified channel is physical and good
      virtual bool IsGood(raw::ChannelID_t channel) const {
        return IsPresent(channel) && !IsBad(channel) && !IsNoisy(channel);
      }
      
      /// Returns a status integer 
      virtual Status_t Status(raw::ChannelID_t channel) const
        { return InvalidStatus;}
      
      
      /// Returns a copy of set of good channel IDs for the current run
      virtual ChannelSet_t GoodChannels() const = 0;
      
      /// Returns a copy of set of bad channel IDs for the current run
      virtual ChannelSet_t BadChannels() const = 0;
      
      /// Returns a copy of set of noisy channel IDs for the current run
      virtual ChannelSet_t NoisyChannels() const = 0;
      
      
      /* TODO DELME
      /// Prepares the object to provide information about the specified time
      /// @return whether information is available for the specified time
      virtual bool Update(DBTimeStamp_t ts) = 0;
      */
  }; // class IChannelStatusProvider
  
} // namespace lariov


#endif // ICHANNELSTATUSPROVIDER_H