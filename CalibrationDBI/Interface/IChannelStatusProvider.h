/**
 * @file   IChannelStatusProvider.h
 * @brief  Interface for experiment-specific channel quality info provider
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    IChannelStatusService.h
 *
 * This is the interface of ChannelStatus core object.
 * A ChannelStatus object (with the interface of ChannelStatus core object)
 * provides the actual information about channels.
 * It can be instanciated by a art service (IChannelStatusService)
 * or from whatever system needs it.
 */


#ifndef ICHANNELSTATUSPROVIDER_H
#define ICHANNELSTATUSPROVIDER_H 1

#include "CalibrationDBIFwd.h"


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
        
    virtual ~IChannelStatusProvider() = default;
    
      /// Returns whether the specified channel is physical and connected to wire
      virtual bool IsPresent(DBChannelID_t channel) const = 0;

      /// Returns whether the specified channel is bad in the current run
      virtual bool IsBad(DBChannelID_t channel) const = 0;

      /// Returns whether the specified channel is noisy in the current run
      virtual bool IsNoisy(DBChannelID_t channel) const = 0;
      
      /// Returns whether the specified channel is physical and good
      virtual bool IsGood(DBChannelID_t channel) const {
        return IsPresent(channel) && !IsBad(channel) && !IsNoisy(channel);
      }

      /// Returns a status integer 
      virtual unsigned short Status(DBChannelID_t channel) const
        { return 99;}


      /// Returns a copy of set of good channel IDs for the current run
      virtual DBChannelSet_t const GoodChannels() const = 0;

      /// Returns a copy of set of bad channel IDs for the current run
      virtual DBChannelSet_t const BadChannels() const = 0;

      /// Returns a copy of set of noisy channel IDs for the current run
      virtual DBChannelSet_t const NoisyChannels() const = 0;

      
      /// Prepares the object to provide information about the specified time
      /// @return whether information is available for the specified time
      virtual bool Update(DBTimeStamp_t ts) = 0;
    
  }; // class IChannelStatusProvider
  
} // namespace lariov


#endif // ICHANNELSTATUSPROVIDER_H
