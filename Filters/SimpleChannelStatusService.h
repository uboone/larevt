/**
 * @file   SimpleChannelStatusService.h
 * @brief  Service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    IChannelStatusService.h SimpleChannelStatus.h
 *
 * Service serving a SimpleChannelStatus.
 */


#ifndef SIMPLECHANNELFILTERSERVICE_H
#define SIMPLECHANNELFILTERSERVICE_H

// LArSoft libraries
#include "Filters/SimpleChannelStatus.h"
#include "CalibrationDBI/Interface/IChannelStatusService.h"

// C/C++ standard libraries
#include <memory> //std::unique_ptr<>

namespace art {
  class ActivityRegistry;
} // namespace art


namespace lariov {

  /**
   * @brief Service providing information about the quality of channels
   *
   * This is a simple implementation of the IChannelStatusService
   * service interface.
   * Channel lists are passed by FHiCL configuration.
   * Note that there is no support for conditions varying with time.
   * 
   * @note This implementation requires Geometry service
   * 
   * Configuration parameters
   * =========================
   * 
   * In addition to the parameters supported by filter::SimpleChannelStatus,
   * this service supports:
   * 
   * - *service_type* (string): must be set to "SimpleChannelStatusService"
   * 
   */
  class SimpleChannelStatusService: public IChannelStatusService {
      public:
    
    /// Constructor: reads the channel IDs from the configuration
    SimpleChannelStatusService
      (fhicl::ParameterSet const& pset, art::ActivityRegistry&);
    
    
      private:
      
    virtual IChannelStatusProvider const& DoGetProvider() const override
     { return *DoGetProviderPtr(); }
    virtual IChannelStatusProvider const* DoGetProviderPtr() const override
     { return fProvider.get(); } 
    
    std::unique_ptr<SimpleChannelStatus> fProvider;
    
  }; // class SimpleChannelStatusService
  
  
} // namespace lariov

DECLARE_ART_SERVICE_INTERFACE_IMPL
  (lariov::SimpleChannelStatusService, lariov::IChannelStatusService, LEGACY)

#endif // SIMPLECHANNELFILTERSERVICE_H
