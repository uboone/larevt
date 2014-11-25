/**
 * @file   SimpleChannelFilterService.h
 * @brief  Service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    ChannelFilterServiceInterface.h SimpleChannelFilter.h
 *
 * Service serving a SimpleChannelFilter.
 */


#ifndef SIMPLECHANNELFILTERSERVICE_H
#define SIMPLECHANNELFILTERSERVICE_H

// LArSoft libraries
#include "Filters/SimpleChannelFilter.h"
#include "Filters/ChannelFilterServiceInterface.h"


namespace filter {

  /**
   * @brief Service providing information about the quality of channels
   *
   * This is a simple implementation of the ChannelFilterServiceInterface
   * service interface.
   * Channel lists are passed by FHiCL configuration.
   * Note that there is no support for conditions varying with time.
   * 
   * Configuration parameters
   * =========================
   * 
   * - *BadChannels* (list, default: empty): list of the IDs of bad channels
   * - *NoisyChannels* (list, default: empty): list of the IDs of noisy channels
   * 
   */
  class SimpleChannelFilterService: public ChannelFilterServiceInterface {
      public:
    
    /// Constructor: reads the channel IDs from the configuration
    SimpleChannelFilterService
      (fhicl::ParameterSet const& pset, art::ActivityRegistry&);
    
    
    /// Returns a pointer to the information provider (nullptr if not available)
    virtual SimpleChannelFilter const* GetFilter() const override
      {
        return static_cast<const SimpleChannelFilter*>
          (ChannelFilterServiceInterface::GetFilter());
      } // GetFilter()
    
    
      protected:
    
    SimpleChannelFilter* filter()
      { return const_cast<SimpleChannelFilter*>(GetFilter()); }
    
  }; // class SimpleChannelFilterService
  
  
} // namespace filter

DECLARE_ART_SERVICE_INTERFACE_IMPL(filter::SimpleChannelFilterService, filter::ChannelFilterServiceInterface, LEGACY)

#endif // SIMPLECHANNELFILTERSERVICE_H
