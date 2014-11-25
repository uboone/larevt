/**
 * @file   SimpleChannelFilter.h
 * @brief  Channel quality provider with information from configuration file
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 25th, 2014
 * @see    ChannelFilterServiceInterface.h SimpleChannelFilter.cpp
 */


#ifndef SIMPLECHANNELFILTER_H
#define SIMPLECHANNELFILTER_H 1

// Utility libraries
#include "fhiclcpp/ParameterSet.h"

// LArSoft libraries
#include "Filters/ChannelFilterBaseInterface.h"


/// Filters for channels, events, etc
namespace filter {


  /** **************************************************************************
   * @brief Class providing information about the quality of channels
   *
   * This class serves information read from a FHiCL configuration file.
   * Time-dependent (or, run-dependent) information is not supported.
   * 
   * LArSoft interface to this class is through the service
   * SimpleChannelFilterService.
   */
  class SimpleChannelFilter: public ChannelFilterBaseInterface {
      public:
    using channel_type = ChannelFilterBaseInterface::channel_type;
      ///< type of channel ID
    using channel_list_type = ChannelFilterBaseInterface::channel_list_type;
      ///< type of a list of channel IDs
    
    /// Configuration
    explicit SimpleChannelFilter(fhicl::ParameterSet const& pset);
    
    /// Returns a copy of set of bad channel IDs for the current run
    virtual channel_list_type SetOfBadChannels() const override
      { return fBadChannels; }
    
    /// Returns a copy of set of noisy channel IDs for the current run
    virtual channel_list_type SetOfNoisyChannels() const override
      { return fNoisyChannels; }
    
    /// Prepares the object to provide information about the specified run
    /// @return whether information is available for the specified run
    virtual bool SetRun(RunNumber_t run) override { return false; }
    
      protected:
    
    channel_list_type fBadChannels; ///< set of bad channels
    channel_list_type fNoisyChannels; ///< set of noisychannels
    
  }; // class SimpleChannelFilter
  
  
} // namespace filter


#endif // SIMPLECHANNELFILTER_H
