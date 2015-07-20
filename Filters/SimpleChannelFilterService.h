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


namespace art {
  class ActivityRegistry;
} // namespace art


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
   * In addition to the parameters supported by filter::SimpleChannelFilter,
   * this service supports:
   * 
   * - *service_type* (string): must be set to "SimpleChannelFilterService"
   * 
   */
  class SimpleChannelFilterService: public ChannelFilterServiceInterface {
      public:
    
    /// Constructor: reads the channel IDs from the configuration
    SimpleChannelFilterService
      (fhicl::ParameterSet const& pset, art::ActivityRegistry&);
    
    
      protected:
    
  }; // class SimpleChannelFilterService
  
  
} // namespace filter

DECLARE_ART_SERVICE_INTERFACE_IMPL(filter::SimpleChannelFilterService, filter::ChannelFilterServiceInterface, LEGACY)

#endif // SIMPLECHANNELFILTERSERVICE_H
